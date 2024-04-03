#include "server.h"

// read the information received from the client and write it as a string
char *read_client_socket(SSL *ssl) {
    char buffer[BUF_SIZE] = ""; // Initialize an empty buffer
    int bytes_read = 0;
    int total_bytes_read = 0;

    while (true) {
        bytes_read = SSL_read(ssl, buffer + total_bytes_read, sizeof(buffer) - total_bytes_read);

        if (bytes_read <= 0) {
            if (SSL_get_error(ssl, bytes_read) == SSL_ERROR_WANT_READ) {
                log_ssl_err_to_file("There is still unprocessed data available at the SSL. Continue reading...\n");
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

void *parse_request_type(char *json_string) { // todo може повертати t_request_type
//    t_map_entry *entery;

    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        const char *error_ptr = cJSON_GetErrorPtr();

        if (error_ptr != NULL) {
            log_to_file("Could not parse the cJSON object from client. A pointer to the error location: [%s]", error_ptr);
        }

        cJSON_Delete(json);
        return;
//        return NULL;
    }

    t_request_type request_type;
    // todo обрати один з двох методів отримання даних з cJSON об'єкту
//    request_type = cJSON_GetObjectItem(json, "request_type")->valueint;
    cJSON *json_req_type = cJSON_GetObjectItem(json, "request_type");

    if (cJSON_IsNull(json_req_type)
        || !cJSON_IsNumber(json_req_type)
        || json_req_type->valueint >= REQUEST_TYPE_COUNT
        || json_req_type->valueint < 0) { // todo или использовать json_req_type->valueint == NULL
        request_type = UNKNOWN_TYPE;
        log_to_file("Could not parse the \"request_type\" field from a cJSON object");
        cJSON_Delete(json);
        return;
//        return request_type;
    }

    request_type = json_req_type->valueint;
    process_client_request(request_type, json);


//    if (request_type > REQUEST_TYPE_COUNT) {
//        mx_logs("Unknown request type", LOG_ERROR);
//        cJSON_Delete(json);
//        return REQ_UNKNOWN;
//    }
//    entery = &request_map[type];
//    entery->handler(json, client);

    cJSON_Delete(json);
    return;
//    return request_type;
}

void *thread_controller(void *arg) {
    t_client *client = (t_client *) arg;
    char *json_string = NULL;

    while (true) {
        json_string = read_client_socket(client->ssl);

        if (!json_string) {
            continue;
        }

        parse_request_type(json_string);
//        t_request_type req_type = parse_request_type(client, json_string);
//
//        if (req_type == UNKNOWN_TYPE || req_type == LOGOUT) {
//            break;
//        }

        mx_strdel(&json_string); // todo change to standard foo?
    }

    pthread_detach(pthread_self());
    return NULL;
}

//void *process_client_request(t_client *client) {
//    t_request_type request_type = thread_controller(client->client_socket); // todo !!!
//if (request_type >= REQUEST_TYPE_COUNT) {
//log_to_file("Could not process a client request. A non-existent request type was received.");
//return; // todo чи треба щось повертати в такому випадку?
//}
void *process_client_request(t_request_type request_type, cJSON *json) {
    if (request_type != REGISTER
        || request_type != LOGIN
        || request_type != UNKNOWN_TYPE) {
        if (!handle_login(json)) {
            log_to_file("Could not authenticate the user before processing the client request");
            pthread_exit(NULL);
            return;
        }
    }

    switch (request_type) {
        case REGISTER:
            handle_registration(json);
            break;
        case LOGIN:
            handle_login(json);
            break;
        case CREATE_CHAT:
            handle_chat_creation(json);
            break;
        case ADD_MEMBER_TO_CHAT:
            handle_adding_new_member_to_chat(json);
            break;
        case GET_USER_CHATS:
            handle_getting_chats(json);
            break;
        case SEND_MESSAGE:
            handle_text_message_sending(json);
            break;
        case GET_MESSAGES_IN_CHAT:
            handle_messages_in_chat_getting(json);
            break;
        case SEND_MESSAGE_AND_GET_MESSAGE_UPDATES:
            handle_message_sending_and_messages_updates_getting(json);
            break;
        case DELETE_MESSAGE_AND_GET_MESSAGE_UPDATES:
            handle_message_deleting_and_messages_updates_getting(json);
            break;
        case CHANGE_MESSAGE_AND_GET_MESSAGE_UPDATES:
            handle_message_changing_and_message_updates_getting(json);
            break;
        case REPLY_TO_MESSAGE_AND_GET_MESSAGE_UPDATES:
            handle_message_replying_and_message_updates_getting(json);
            break;
        case GET_MESSAGE_UPDATES:
            handle_message_updates_getting(json);
            break;
        case GET_CHAT_MEMBERS:
            handle_getting_chat_members(json);
            break;
        case UNKNOWN_TYPE:
            log_to_file("Could not process a client request. A non-existent request type was received.");
            break;
        default:
            break;
    }

//    close(client->client_socket);
    pthread_exit(NULL); // припиняє виконання поточного потоку, але інші потоки можуть продовжувати працювати
}
