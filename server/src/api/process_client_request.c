#include "api.h"
#include "database.h"

void process_client_request(t_client *client_info, t_request_type request_type, char *json_string) {
    switch (request_type) {
        case REGISTER:
            handle_registration_request(client_info, json_string);
            break;
        case LOGIN:
            handle_login_request(client_info, LOGIN, json_string);
            break;
        case UPDATE_USER:
            handle_update_user_request(client_info, json_string);
            break;
        case GET_USERS_FOR_SEARCH:
            handle_get_users_for_search_request(client_info);
            break;
        case ADD_CHAT:
            handle_add_chat_request(client_info, json_string);
            break;
        case GET_CHATS:
            handle_get_chats_request(client_info);
            break;
        case GET_MESSAGES:
            handle_get_messages_request(client_info, json_string);
            break;
        case ADD_MESSAGE:
            handle_add_message_request(client_info, json_string);
            break;
        case UPDATE_MESSAGE:
            handle_update_message_request(client_info, json_string);
            break;
        case DELETE_MESSAGE:
            handle_delete_message_request(client_info, json_string);
            break;
        case UNKNOWN_REQUEST:
            log_to_file("Could not process a client request. A non-existent request type was received", ERROR);
            break;
        default:
            break;
    }

    return;
}

void log_status_to_file(char *message, char *value) {
    char *main_message = (char *) malloc(200 * sizeof(char));
    char *data = (char *) malloc(100 * sizeof(char));
    sprintf(data, ": %s", value);
    strcat(main_message, message);
    strcat(main_message, data);
    log_to_file(main_message, INFO);
    free(data);
    free(main_message);
}

char *hash_password(const char *password) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_get_digestbyname("SHA256");

    EVP_DigestInit(ctx, md);
    EVP_DigestUpdate(ctx, password, strlen(password));
    EVP_DigestFinal(ctx, hash, &hash_len);
    EVP_MD_CTX_free(ctx);
    char *output = malloc(sizeof(char) * 64);

    for (unsigned int i = 0; i < hash_len; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }

    return output;
}

void handle_registration_request(t_client *client_info, char *json_string) {
    t_request_type request_type = REGISTER;
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        log_to_file("Could not parse the cJSON object from client", JSON_ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_JSON);
        return;
    }

    const cJSON *json_data_obj = cJSON_GetObjectItemCaseSensitive(json, "data");
    t_user_data *user_data = (t_user_data *)malloc(sizeof(t_user_data));

    if (cJSON_IsObject(json_data_obj)) {
        const cJSON *json_username = cJSON_GetObjectItemCaseSensitive(json_data_obj, "username");
        const cJSON *json_password = cJSON_GetObjectItemCaseSensitive(json_data_obj, "password");

        if (cJSON_IsString(json_username)
            && cJSON_IsString(json_password)) {
            user_data->username = mx_strdup((const char *)json_username->valuestring);
            user_data->password = hash_password((const char *) mx_strdup((const char *) json_password->valuestring));
        } else {
            log_to_file("Could not parse the \"data\" fields from a cJSON object", JSON_ERROR);
            send_status_response(client_info->ssl, request_type, ERROR_JSON);
            free(user_data);
            cJSON_Delete(json);
            return;
        }
    } else {
        log_to_file("Could not parse the \"data\" object from a cJSON object", JSON_ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_JSON);
        free(user_data);
        cJSON_Delete(json);
        return;
    }

    user_data->created_at = time(NULL);

    if (!db_add_user(user_data)) {
        log_to_file("Could not post user data to database", ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_DB);
        free(user_data);
        cJSON_Delete(json);
        return;
    }

    t_user_data *new_user_data = db_get_user_data(user_data->username);

    if (!new_user_data) {
        log_to_file("Could not get user data from the database", ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_DB);
        free(user_data);
        cJSON_Delete(json);
        return;
    }

    client_info->id = new_user_data->id;
    client_info->username = mx_strdup((const char *)new_user_data->username);

    send_auth_response(client_info->ssl, request_type, SUCCESS, new_user_data);
    free(user_data);
    free(new_user_data);
    cJSON_Delete(json);
    return;
}

bool handle_login_request(t_client *client_info, t_request_type request_type, char *json_string) {
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        log_to_file("Could not parse the cJSON object from client", JSON_ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_JSON);
        return false;
    }

    const cJSON *json_credentials_obj = cJSON_GetObjectItemCaseSensitive(json, "credentials");
    t_user_data *user_data = (t_user_data *)malloc(sizeof(t_user_data));

    if (cJSON_IsObject(json_credentials_obj)) {
        const cJSON *json_username = cJSON_GetObjectItemCaseSensitive(json_credentials_obj, "username");
        const cJSON *json_password = cJSON_GetObjectItemCaseSensitive(json_credentials_obj, "password");

        if (cJSON_IsString(json_username)
            && cJSON_IsString(json_password)) {
            user_data->username = mx_strdup((const char *) json_username->valuestring);
            user_data->password = mx_strdup((const char *) hash_password((const char *) json_password->valuestring));
        } else {
            log_to_file("Could not parse the \"credentials\" fields from a cJSON object", JSON_ERROR);
            send_status_response(client_info->ssl, request_type, ERROR_JSON);
            free(user_data);
            cJSON_Delete(json);
            return false;
        }
    } else {
        log_to_file("Could not parse the \"credentials\" object from a cJSON object", JSON_ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_JSON);
        free(user_data);
        cJSON_Delete(json);
        return false;
    }

    t_user_data *new_user_data = db_get_user_data(user_data->username);

    if (!new_user_data) {
        log_to_file("The passed user's credentials is invalid", ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_INVALID_CREDENTIALS);
        free(user_data);
        cJSON_Delete(json);
        return false;
    }

    client_info->id = new_user_data->id;
    client_info->username = mx_strdup((const char *) new_user_data->username);

    if (mx_strcmp(new_user_data->password, user_data->password) != 0) {
        log_to_file("The passed user's credentials is invalid", ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_INVALID_CREDENTIALS);
        free(user_data);
        free(new_user_data);
        cJSON_Delete(json);
        return false;
    }

    if (request_type == LOGIN) {
        send_auth_response(client_info->ssl, request_type, SUCCESS, new_user_data);
    }

    free(user_data);
    free(new_user_data);
    cJSON_Delete(json);
    return true;
}

void handle_update_user_request(t_client *client_info, char *json_string) {
    t_request_type request_type = UPDATE_USER;
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        log_to_file("Could not parse the cJSON object from client", JSON_ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_JSON);
        return;
    }

    const cJSON *json_data_obj = cJSON_GetObjectItemCaseSensitive(json, "data");
    t_user_data *user_data = (t_user_data *)malloc(sizeof(t_user_data));

    if (cJSON_IsObject(json_data_obj)) {
        const cJSON *json_password = cJSON_GetObjectItemCaseSensitive(json_data_obj, "password");

        if (cJSON_IsString(json_password)) {
            user_data->password = mx_strdup((const char *) hash_password((const char *) json_password->valuestring));
        } else {
            log_to_file("Could not parse the \"data\" fields from a cJSON object", JSON_ERROR);
            send_status_response(client_info->ssl, request_type, ERROR_JSON);
            cJSON_Delete(json);
            free(user_data);
            return;
        }
    } else {
        log_to_file("Could not parse the \"data\" object from a cJSON object", JSON_ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_JSON);
        cJSON_Delete(json);
        free(user_data);
        return;
    }

    user_data->id = client_info->id;
    user_data->username = mx_strdup((const char *) client_info->username);
    user_data->updated_at = time(NULL);

    if (!db_update_user(user_data)) {
        log_to_file("Could not update user data in the database", ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_DB);
        cJSON_Delete(json);
        free(user_data);
        return;
    }

    t_user_data *new_user_data = db_get_user_data(user_data->username);

    if (!new_user_data) {
        log_to_file("Could not get user data from the database", ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_DB);
        cJSON_Delete(json);
        free(user_data);
        return;
    }

    send_auth_response(client_info->ssl, request_type, SUCCESS, new_user_data);
    free(user_data);
    free(new_user_data);
    cJSON_Delete(json);
    return;
}

void handle_get_users_for_search_request(t_client *client_info) {
    t_request_type request_type = GET_USERS_FOR_SEARCH;
    t_list *user_list = db_get_user_list(client_info->id);

    if (!user_list) {
        log_to_file("Users are missing from the database", ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_DB);
        return;
    }

    send_get_users_for_search_response(client_info->ssl, SUCCESS, user_list);
    free(user_list);
    return;
}

void handle_add_chat_request(t_client *client_info, char *json_string) {
    t_request_type request_type = ADD_CHAT;
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        log_to_file("Could not parse the cJSON object from client", JSON_ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_JSON);
        return;
    }

    const cJSON *json_data_obj = cJSON_GetObjectItemCaseSensitive(json, "data");
    t_chat_data *chat_data = (t_chat_data *) malloc(sizeof(t_chat_data));
    chat_data->title = NULL;
    chat_data->current_user.id = client_info->id;

    if (cJSON_IsObject(json_data_obj)) {
        const cJSON *json_invitee_user_id = cJSON_GetObjectItemCaseSensitive(json_data_obj, "invitee_user_id");

        if (cJSON_IsNumber(json_invitee_user_id)) {
            chat_data->invitee_user.id = json_invitee_user_id->valueint;
        } else {
            log_to_file("Could not parse the \"data\" fields from a cJSON object", JSON_ERROR);
            send_status_response(client_info->ssl, request_type, ERROR_JSON);
            cJSON_Delete(json);
            free(chat_data);
            return;
        }
    } else {
        log_to_file("Could not parse the \"data\" object from a cJSON object", JSON_ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_JSON);
        cJSON_Delete(json);
        free(chat_data);
        return;
    }

    char *invitee_username = db_get_username(chat_data->invitee_user.id);

    if (invitee_username == NULL) {
        log_to_file("The user does not exist in the database", DB_ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_DB);
        cJSON_Delete(json);
        free(chat_data);
        return;
    }

    chat_data->created_at = time(NULL);
    chat_data->current_user.created_at = time(NULL);
    chat_data->invitee_user.created_at = time(NULL);
    t_chat_data *new_chat_data = db_add_chat(chat_data);

    if (!new_chat_data) {
        log_to_file("Could not add chat to the database", ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_DB);
        cJSON_Delete(json);
        free(chat_data);
        return;
    }

    send_add_chat_response(client_info->ssl, SUCCESS, new_chat_data);
    free(chat_data);
    free(new_chat_data);
    cJSON_Delete(json);
    return;
}

void handle_get_chats_request(t_client *client_info) {
    t_request_type request_type = GET_CHATS;
    t_list *chat_list = db_get_user_chats(client_info->id);

    if (!chat_list) {
        log_to_file("User chats are missing from the database", ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_DB);
        return;
    }

    send_get_chats_response(client_info->ssl, SUCCESS, chat_list);
    free(chat_list);
    return;
}

void handle_get_messages_request(t_client *client_info, char *json_string) {
    t_request_type request_type = GET_MESSAGES;
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        log_to_file("Could not parse the cJSON object from client", JSON_ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_JSON);
        return;
    }

    const cJSON *json_data_obj = cJSON_GetObjectItemCaseSensitive(json, "data");
    int chat_id;

    if (cJSON_IsObject(json_data_obj)) {
        const cJSON *json_chat_id = cJSON_GetObjectItemCaseSensitive(json_data_obj, "chat_id");

        if (cJSON_IsNumber(json_chat_id)) {
            chat_id = json_chat_id->valueint;
        } else {
            log_to_file("Could not parse the \"data\" fields from a cJSON object", JSON_ERROR);
            send_status_response(client_info->ssl, request_type, ERROR_JSON);
            cJSON_Delete(json);
            return;
        }
    } else {
        log_to_file("Could not parse the \"data\" from a cJSON object", JSON_ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_JSON);
        cJSON_Delete(json);
        return;
    }

    t_list *message_list = db_get_chat_messages(chat_id);

    if (!message_list) {
        log_to_file("Chat messages are missing from the database", ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_DB);
        cJSON_Delete(json);
        return;
    }

    send_get_messages_response(client_info->ssl, SUCCESS, message_list);
    free(message_list);
    cJSON_Delete(json);
    return;
}

void handle_add_message_request(t_client *client_info, char *json_string) {
    t_request_type request_type = ADD_MESSAGE;
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        log_to_file("Could not parse the cJSON object from client", JSON_ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_JSON);
        return;
    }

    const cJSON *json_data_obj = cJSON_GetObjectItemCaseSensitive(json, "data");
    t_msg_data *msg_data = (t_msg_data *) malloc(sizeof(t_msg_data));
    msg_data->user_id = client_info->id;

    if (cJSON_IsObject(json_data_obj)) {
        const cJSON *json_chat_id = cJSON_GetObjectItemCaseSensitive(json_data_obj, "chat_id");
        const cJSON *json_body = cJSON_GetObjectItemCaseSensitive(json_data_obj, "body");

        if (cJSON_IsNumber(json_chat_id)
            && cJSON_IsString(json_body)) {
            msg_data->chat_id = json_chat_id->valueint;
            msg_data->body = mx_strdup((const char *)json_body->valuestring);
        } else {
            log_to_file("Could not parse the \"data\" fields from a cJSON object", JSON_ERROR);
            send_status_response(client_info->ssl, request_type, ERROR_JSON);
            free(msg_data);
            cJSON_Delete(json);
            return;
        }
    } else {
        log_to_file("Could not parse the \"data\" object from a cJSON object", JSON_ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_JSON);
        free(msg_data);
        cJSON_Delete(json);
        return;
    }

    msg_data->created_at = time(NULL);
    t_msg_data *new_msg_data = db_add_message(msg_data);

    if (!new_msg_data) {
        log_to_file("Could not post message in the database", ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_DB);
        free(msg_data);
        cJSON_Delete(json);
        return;
    }

    send_message_response(client_info->ssl, request_type, SUCCESS, new_msg_data);
    log_to_file("Successfully sent to client added message", INFO);
    free(msg_data);
    free(new_msg_data);
    cJSON_Delete(json);
    return;
}

void handle_update_message_request(t_client *client_info, char *json_string) {
    t_request_type request_type = UPDATE_MESSAGE;
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        log_to_file("Could not parse the cJSON object from client", JSON_ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_JSON);
        return;
    }

    const cJSON *json_data_obj = cJSON_GetObjectItemCaseSensitive(json, "data");
    t_msg_data *msg_data = (t_msg_data *) malloc(sizeof(t_msg_data));

    if (cJSON_IsObject(json_data_obj)) {
        const cJSON *json_msg_id = cJSON_GetObjectItemCaseSensitive(json_data_obj, "message_id");
        const cJSON *json_body = cJSON_GetObjectItemCaseSensitive(json_data_obj, "body");

        if (cJSON_IsNumber(json_msg_id)
            && cJSON_IsString(json_body)) {
            msg_data->message_id = json_msg_id->valueint;
            msg_data->body = json_body->valuestring;
        } else {
            log_to_file("Could not parse the \"data\" fields from a cJSON object", JSON_ERROR);
            send_status_response(client_info->ssl, request_type, ERROR_JSON);
            free(msg_data);
            cJSON_Delete(json);
            return;
        }
    } else {
        log_to_file("Could not parse the \"data\" from a cJSON object", JSON_ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_JSON);
        free(msg_data);
        cJSON_Delete(json);
        return;
    }

    msg_data->updated_at = time(NULL);
    t_msg_data *new_msg_data = db_update_message(msg_data);

    if (!new_msg_data) {
        log_to_file("Message is missing from the database", ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_DB);
        free(msg_data);
        cJSON_Delete(json);
        return;
    }

    send_message_response(client_info->ssl, request_type, SUCCESS, new_msg_data);
    log_to_file("Successfully sent to client updated message", INFO);
    free(msg_data);
    free(new_msg_data);
    cJSON_Delete(json);
    return;
}

void handle_delete_message_request(t_client *client_info, char *json_string) {
    log_to_file("handle_delete_message_request", INFO);
    t_request_type request_type = DELETE_MESSAGE;
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        log_to_file("Could not parse the cJSON object from client", JSON_ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_JSON);
        return;
    }

    const cJSON *json_data_obj = cJSON_GetObjectItemCaseSensitive(json, "data");
    t_msg_data *msg_data = (t_msg_data *) malloc(sizeof(t_msg_data));

    if (cJSON_IsObject(json_data_obj)) {
        const cJSON *json_msg_id = cJSON_GetObjectItemCaseSensitive(json_data_obj, "message_id");

        if (cJSON_IsNumber(json_msg_id)) {
            msg_data->message_id = json_msg_id->valueint;
        } else {
            log_to_file("Could not parse the \"data\" fields from a cJSON object", JSON_ERROR);
            send_status_response(client_info->ssl, request_type, ERROR_JSON);
            free(msg_data);
            cJSON_Delete(json);
            return;
        }
    } else {
        log_to_file("Could not parse the \"data\" from a cJSON object", JSON_ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_JSON);
        free(msg_data);
        cJSON_Delete(json);
        return;
    }

    msg_data->deleted_at = time(NULL);
    t_msg_data *new_msg_data = db_delete_message(msg_data);

    if (!new_msg_data) {
        log_to_file("Message is missing from the database", ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_DB);
        free(msg_data);
        cJSON_Delete(json);
        return;
    }

    send_message_response(client_info->ssl, request_type, SUCCESS, new_msg_data);
    log_to_file("Successfully sent to client deleted message", INFO);
    free(msg_data);
    free(new_msg_data);
    cJSON_Delete(json);
    return;
}

