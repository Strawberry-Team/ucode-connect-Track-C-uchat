#include "client.h"

void send_login_to_server(SSL *ssl, t_request_type request_type, t_user_data *user_data) {
    char *json_string = NULL;
    cJSON *json = cJSON_CreateObject();
    cJSON *json_credentials_obj = cJSON_CreateObject();

    if (!json
        || !json_credentials_obj) {
        log_to_file("Could not create the cJSON object to send a login request to the server");
        cJSON_Delete(json_credentials_obj);
        cJSON_Delete(json);
        return;
    }

    if (!cJSON_AddItemToObject(json, "credentials", json_credentials_obj)
        || !cJSON_AddNumberToObject(json, "request_type", request_type)
        || !cJSON_AddStringToObject(json_credentials_obj, "username", user_data->username)
        || !cJSON_AddStringToObject(json_credentials_obj, "password", user_data->password)) {
        log_to_file("Could not add the data in the cJSON object to send a login request to the server");
        cJSON_Delete(json_credentials_obj);
        cJSON_Delete(json);
        return;
    }

    json_string = cJSON_Print(json); // Друкує JSON-об'єкт або масив у форматі, що включає відступи та нові рядки. альтернатива cJSON_PrintUnformatted() - без відступів та нових рядків.

    int bytes_written = SSL_write(ssl, json_string, strlen(json_string)); // або mx_strlen()

    if (bytes_written <= 0) {
        log_to_file("Could not write JSON string over the TLS/SSL connection to send a login request to the server");
//        free(json_string); // або mx_strdel(&json_string);
    }

    free(json_string); // або mx_strdel(&json_string);
    cJSON_Delete(json_credentials_obj);
    cJSON_Delete(json);
    return;
}

bool handle_login_response(cJSON *json) {
    int status;
    cJSON *json_status = cJSON_GetObjectItemCaseSensitive(json, "status");

    if (cJSON_IsNull(json_status)
        || !cJSON_IsNumber(json_status)
        || json_status->valueint >= STATUS_TYPE_COUNT
        || json_status->valueint < 0) {
//        status = UNKNOWN_TYPE;
        log_to_file("Could not parse the \"status\" field from a cJSON object");
        cJSON_Delete(json_status);
        return false;
    }

    status = json_status->valueint;

    if (status != SUCCESS_VALID_CREDENTIALS) {
//        handle_error_response(); // todo CREATE THIS FOO
        log_to_file("Wrong status for login request. Expected: SUCCESS_VALID_CREDENTIALS");
        cJSON_Delete(json_status);
        return false;
    }

    cJSON *json_data_obj = cJSON_GetObjectItemCaseSensitive(json, "data");

    if (!cJSON_IsObject(json_data_obj)
        || cJSON_IsNull(json_data_obj)) {
        log_to_file("Could not parse the \"data\" field from a cJSON object");
        cJSON_Delete(json_data_obj);
        cJSON_Delete(json_status);
        return false;
    }

//    pthread_mutex_lock(&client->mutex); // todo do we need it???
    client->id = cJSON_GetObjectItem(json_data_obj, "id")->valueint;
    client->username = mx_strdup(cJSON_GetObjectItemCaseSensitive(json_data_obj, "username")->valuestring);
    client->password = mx_strdup(cJSON_GetObjectItemCaseSensitive(json_data_obj, "password")->valuestring);
    client->icon_id = cJSON_GetObjectItem(json_data_obj, "icon_id")->valueint;
//    pthread_mutex_unlock(&client->mutex); // todo do we need it???

    cJSON_Delete(json_status);
    return true;
}

char *read_client_socket(SSL *ssl) {
    char buffer[BUF_SIZE] = ""; // Initialize an empty buffer
    int bytes_read = 0;
    int total_bytes_read = 0;

    while (true) {
        bytes_read = SSL_read(ssl, buffer + total_bytes_read, sizeof(buffer) - total_bytes_read);

        if (bytes_read <= 0) {
            if (SSL_get_error(ssl, bytes_read) == SSL_ERROR_WANT_READ) {
                log_ssl_err_to_file("There is still unprocessed data available at the TLS/SSL connection. Continue reading...");
                continue;
            }

            log_ssl_err_to_file("Failed to read data from the TLS/SSL connection to buffer");
            return NULL;
        }

        total_bytes_read += bytes_read;

        // Перевірка, чи дані повністю прочитані
        if (total_bytes_read == sizeof(buffer)) { // Буфер переповнений
            return NULL;
        } else if (buffer[total_bytes_read - 1] == '\0') { // Зустріли символ кінця файлу, значить дані повністю прочитані
            break;
        }
    }

    buffer[total_bytes_read] = '\0';

    return strdup(buffer);
}

void process_server_response(t_request_type request_type, cJSON *json) {
    if (request_type != REGISTER
        && request_type != LOGIN
        && request_type != UNKNOWN_TYPE) {
        if (!handle_login_response(json)) {
            log_to_file("Could not authenticate the user before processing the client request");
//            pthread_exit(NULL);
            return;
        }
    }

    switch (request_type) {
//        case REGISTER:
//            handle_registration(json);
//            break;
        case LOGIN:
            handle_login_response(json);
            break;
//        case CREATE_CHAT:
//            handle_chat_creation(json);
//            break;
//        case ADD_MEMBER_TO_CHAT:
//            handle_adding_new_member_to_chat(json);
//            break;
//        case GET_USER_CHATS:
//            handle_getting_chats(json);
//            break;
//        case SEND_MESSAGE:
//            handle_text_message_sending(json);
//            break;
//        case GET_MESSAGES_IN_CHAT:
//            handle_messages_in_chat_getting(json);
//            break;
//        case SEND_MESSAGE_AND_GET_MESSAGE_UPDATES:
//            handle_message_sending_and_messages_updates_getting(json);
//            break;
//        case DELETE_MESSAGE_AND_GET_MESSAGE_UPDATES:
//            handle_message_deleting_and_messages_updates_getting(json);
//            break;
//        case CHANGE_MESSAGE_AND_GET_MESSAGE_UPDATES:
//            handle_message_changing_and_message_updates_getting(json);
//            break;
//        case REPLY_TO_MESSAGE_AND_GET_MESSAGE_UPDATES:
//            handle_message_replying_and_message_updates_getting(json);
//            break;
//        case GET_MESSAGE_UPDATES:
//            handle_message_updates_getting(json);
//            break;
//        case GET_CHAT_MEMBERS:
//            handle_getting_chat_members(json);
//            break;
        case UNKNOWN_TYPE:
            log_to_file("Could not process a client request. A non-existent request type was received.");
            break;
        default:
            break;
    }

//    close(client->client_socket);
    return;
}

void parse_request_type(char *json_string) {
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        const char *error_ptr = cJSON_GetErrorPtr();

        if (error_ptr != NULL) {
            char msg[200];
            sprintf(msg, "Could not parse the cJSON object from server. A pointer to the error location: [%s]",
                    error_ptr);
            log_to_file(msg);
        }

        cJSON_Delete(json);
        return;
    }

    t_request_type request_type;
    // todo обрати один з двох методів отримання даних з cJSON об'єкту
//    request_type = cJSON_GetObjectItemCaseSensitive(json, "request_type")->valueint;
    cJSON *json_req_type = cJSON_GetObjectItemCaseSensitive(json, "request_type");

    if (cJSON_IsNull(json_req_type)
        || !cJSON_IsNumber(json_req_type)
        || json_req_type->valueint >= REQUEST_TYPE_COUNT
        || json_req_type->valueint < 0) { // todo или использовать json_req_type->valueint == NULL
        request_type = UNKNOWN_TYPE;
        log_to_file("Could not parse the \"request_type\" field from a cJSON object");
        cJSON_Delete(json_req_type);
        cJSON_Delete(json);
        free(json_string);
        return;
    }

    request_type = json_req_type->valueint;
    process_server_response(request_type, json);

    cJSON_Delete(json);
    return;
}

void *controller(void *arg) {
    t_client *client = (t_client *) arg;
    char *json_string = NULL;

    while (true) {
        json_string = read_client_socket(client->ssl);

        if (!json_string) {
            continue;
        }

        parse_request_type(json_string);
        mx_strdel(&json_string); // todo change to standard foo?
    }

//    pthread_detach(pthread_self());
    return NULL;
}

