#include "api.h"
#include "database.h"

void process_client_request(t_client *client_info, t_request_type request_type, char *json_string) {
    if (request_type != REGISTER
        && request_type != LOGIN
        && request_type != UNKNOWN_REQUEST) {
        if (!handle_login(client_info, json_string)) {
            log_to_file("Could not authenticate the user before processing the client request", ERROR);
            pthread_exit(NULL);
            return;
        }
    }

    switch (request_type) {
        case REGISTER:
            handle_registration(client_info, json_string);
            break;
        case LOGIN:
            handle_login(client_info, json_string);
            break;
//        case CREATE_CHAT:
//            handle_chat_creation(client_info, json_string);
//            break;
//        case ADD_MEMBER_TO_CHAT:
//            handle_adding_new_member_to_chat(client_info, json_string);
//            break;
//        case GET_USER_CHATS:
//            handle_getting_chats(client_info, json_string);
//            break;
//        case SEND_MESSAGE:
//            handle_text_message_sending(client_info, json_string);
//            break;
//        case GET_MESSAGES_IN_CHAT:
//            handle_messages_in_chat_getting(client_info, json_string);
//            break;
//        case SEND_MESSAGE_AND_GET_MESSAGE_UPDATES:
//            handle_message_sending_and_messages_updates_getting(client_info, json_string);
//            break;
//        case DELETE_MESSAGE_AND_GET_MESSAGE_UPDATES:
//            handle_message_deleting_and_messages_updates_getting(client_info, json_string);
//            break;
//        case CHANGE_MESSAGE_AND_GET_MESSAGE_UPDATES:
//            handle_message_changing_and_message_updates_getting(client_info, json_string);
//            break;
//        case REPLY_TO_MESSAGE_AND_GET_MESSAGE_UPDATES:
//            handle_message_replying_and_message_updates_getting(client_info, json_string);
//            break;
//        case GET_MESSAGE_UPDATES:
//            handle_message_updates_getting(client_info, json_string);
//            break;
//        case GET_CHAT_MEMBERS:
//            handle_getting_chat_members(client_info, json_string);
//            break;
        case UNKNOWN_REQUEST:
            log_to_file("Could not process a client request. A non-existent request type was received", ERROR);
            break;
        default:
            break;
    }

    return;
}

void log_status_to_file(char *message, char *value) {
    char main_message[200];
    char data[100];
    sprintf(data, ": %s", value);
    strcat(main_message, message);
    strcat(main_message, data);
    log_to_file(main_message, INFO);
}

void handle_registration(t_client *client_info, char *json_string) {
    t_request_type request_type = REGISTER;
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        log_to_file("Could not parse the cJSON object from client", CJSON_ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_JSON);
        return;
    }

    const cJSON *json_data_obj = cJSON_GetObjectItemCaseSensitive(json, "data");
    char *username;
    char *password;

    if (cJSON_IsObject(json_data_obj)) {
        const cJSON *json_username = cJSON_GetObjectItemCaseSensitive(json_data_obj, "username");
        const cJSON *json_password = cJSON_GetObjectItemCaseSensitive(json_data_obj, "password");

        if (cJSON_IsString(json_username)
            && cJSON_IsString(json_password)) {
            username = mx_strdup((const char *)json_username->valuestring);
            password = mx_strdup((const char *)json_password->valuestring);
        } else {
            log_to_file("Could not parse the \"data\" fields from a cJSON object", CJSON_ERROR);
            send_status_response(client_info->ssl, request_type, ERROR_JSON);
            cJSON_Delete(json);
            return;
        }
    } else {
        log_to_file("Could not parse the \"data\" from a cJSON object", CJSON_ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_JSON);
        cJSON_Delete(json);
        return;
    }

    db_put_user_data(username, password);
    t_user_data *user_data = db_get_user_data(username);


    if (!user_data) {
        log_to_file("Could not put user data to database", ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_DB);
        cJSON_Delete(json);
        return;
    }

    client_info->id = user_data->id;
    client_info->username = mx_strdup((const char *)user_data->username);
    client_info->password = mx_strdup((const char *)user_data->password);
    client_info->icon_id = user_data->icon_id;
    log_status_to_file("Successfully registered user", client_info->username);
    send_auth_response(client_info->ssl, request_type, SUCCESS, user_data);
    return;
}

bool handle_login(t_client *client_info, char *json_string) {
    t_request_type request_type = LOGIN;
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        log_to_file("Could not parse the cJSON object from client", CJSON_ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_JSON);
        return false;
    }

    const cJSON *json_credentials_obj = cJSON_GetObjectItemCaseSensitive(json, "credentials");
    char *username;
    char *password;

    if (cJSON_IsObject(json_credentials_obj)) {
        const cJSON *json_username = cJSON_GetObjectItemCaseSensitive(json_credentials_obj, "username");
        const cJSON *json_password = cJSON_GetObjectItemCaseSensitive(json_credentials_obj, "password");

        if (cJSON_IsString(json_username)
            && cJSON_IsString(json_password)) {
            username = mx_strdup((const char *)json_username->valuestring);
            password = mx_strdup((const char *)json_password->valuestring);
        } else {
            log_to_file("Could not parse the \"credentials\" fields from a cJSON object", CJSON_ERROR);
            send_status_response(client_info->ssl, request_type, ERROR_JSON);
            cJSON_Delete(json);
            return false;
        }
    } else {
        log_to_file("Could not parse the \"credentials\" from a cJSON object", CJSON_ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_JSON);
        cJSON_Delete(json);
        return false;
    }

    t_user_data *user_data;
    user_data = db_get_user_data(username);

    if (!user_data) {
        log_to_file("The passed user's credentials is invalid", ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_INVALID_CREDENTIALS);
        return false;
    }

    client_info->username = mx_strdup((const char *)user_data->username);
    log_status_to_file("The database has credentials for the user", client_info->username);

    if (mx_strcmp(user_data->password, password) != 0) {
        log_to_file("The passed user's credentials is invalid", ERROR);
        send_status_response(client_info->ssl, request_type, ERROR_INVALID_CREDENTIALS);
        return false;
    }

    client_info->password = mx_strdup((const char *)user_data->password);
    log_status_to_file("Successfully logged in user", client_info->username);
    send_auth_response(client_info->ssl, request_type, SUCCESS_VALID_CREDENTIALS, user_data);
    return true;
}

