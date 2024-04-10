#include "server.h"
#include "api.h"

/* Read the information received from the client and write it as a string */
char *read_client_socket(SSL *ssl) {
//    char buffer[BUF_SIZE]; // Initialize an empty buffer
//    memset(buffer, 0, sizeof(buffer));
    char *buffer = (char *) malloc(BUF_SIZE * sizeof(char));
    int bytes_read = 0;
    int total_bytes_read = 0;

//    pthread_mutex_lock(&clients_mutex);//todo

    while (true) {
        bytes_read = SSL_read(ssl, buffer + total_bytes_read, BUF_SIZE - total_bytes_read);

        if (bytes_read <= 0) {
            int error_code = SSL_get_error(ssl, bytes_read);

            if (error_code == SSL_ERROR_WANT_READ
                || error_code == SSL_ERROR_WANT_WRITE) {
                log_ssl_err_to_file("There is still unprocessed data available at the TLS/SSL connection. Continue reading...");
                continue;
            } else if (error_code == SSL_ERROR_ZERO_RETURN) {
                log_ssl_err_to_file("Connection is closed");
//                pthread_mutex_unlock(&clients_mutex); //todo
                return NULL;
            } else {
                log_ssl_err_to_file("Unexpected error reading from client socket");
//                pthread_mutex_unlock(&clients_mutex); //todo
                return NULL;
            }
        }

        total_bytes_read += bytes_read;

        if (total_bytes_read >= BUF_SIZE) { /* буфер переповнений */
            log_to_file("Buffer for reading is overflowing", ERROR);
//            pthread_mutex_unlock(&clients_mutex); //todo
            return NULL;
        } else if (buffer[total_bytes_read] == '\0') { /* кінець файлу, дані повністю прочитані */
            break;
        }
    }

//    pthread_mutex_unlock(&clients_mutex); //todo
    buffer[total_bytes_read] = '\0';
    return mx_strdup(buffer);
}

t_request_type parse_request_type(char *json_string) {
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        const char *error_ptr = cJSON_GetErrorPtr();

        if (error_ptr != NULL) {
            char msg[200];
            sprintf(msg, "Could not parse the cJSON object from client. A pointer to the error location: [%s]", error_ptr);
            log_to_file(msg, ERROR);
        }
        cJSON_Delete(json);
        return UNKNOWN_REQUEST;
    }

//    pthread_mutex_lock(&clients_mutex); //todo
    t_request_type request_type;
    const cJSON *json_req_type = cJSON_GetObjectItem(json, "request_type");
//    pthread_mutex_unlock(&clients_mutex); //todo

    if (!cJSON_IsNumber(json_req_type)
        || json_req_type->valueint < 0
        || json_req_type->valueint >= REQUEST_TYPE_COUNT) {
        request_type = UNKNOWN_REQUEST;
        log_to_file("Could not parse the \"request_type\" field from a cJSON object", ERROR);
        cJSON_Delete(json);
        return request_type;
    }

    request_type = json_req_type->valueint;
    cJSON_Delete(json);
    return request_type;
}

void *thread_controller(void *arg) {
    t_client *client = (t_client *) arg;
    char *json_string = NULL;
    t_request_type request_type;

    while (true) {
        json_string = read_client_socket(client->ssl);

        if (!json_string) {
            continue;
        }

        request_type = parse_request_type(json_string);
        process_client_request(request_type, json_string);
        mx_strdel(&json_string); // todo change to standard foo?
    }

    pthread_detach(pthread_self()); // todo change to something else?
    return NULL;
}

void process_client_request(t_request_type request_type, char *json_string) {
    if (request_type != REGISTER
        && request_type != LOGIN
        && request_type != UNKNOWN_REQUEST) {
        if (!handle_login(json_string)) {
            log_to_file("Could not authenticate the user before processing the client request", ERROR);
            pthread_exit(NULL);
            return;
        }
    }

    switch (request_type) {
//        case REGISTER:
//            handle_registration(json_string);
//            break;
        case LOGIN:
            handle_login(json_string);
            break;
//        case CREATE_CHAT:
//            handle_chat_creation(json_string);
//            break;
//        case ADD_MEMBER_TO_CHAT:
//            handle_adding_new_member_to_chat(json_string);
//            break;
//        case GET_USER_CHATS:
//            handle_getting_chats(json_string);
//            break;
//        case SEND_MESSAGE:
//            handle_text_message_sending(json_string);
//            break;
//        case GET_MESSAGES_IN_CHAT:
//            handle_messages_in_chat_getting(json_string);
//            break;
//        case SEND_MESSAGE_AND_GET_MESSAGE_UPDATES:
//            handle_message_sending_and_messages_updates_getting(json_string);
//            break;
//        case DELETE_MESSAGE_AND_GET_MESSAGE_UPDATES:
//            handle_message_deleting_and_messages_updates_getting(json_string);
//            break;
//        case CHANGE_MESSAGE_AND_GET_MESSAGE_UPDATES:
//            handle_message_changing_and_message_updates_getting(json_string);
//            break;
//        case REPLY_TO_MESSAGE_AND_GET_MESSAGE_UPDATES:
//            handle_message_replying_and_message_updates_getting(json_string);
//            break;
//        case GET_MESSAGE_UPDATES:
//            handle_message_updates_getting(json_string);
//            break;
//        case GET_CHAT_MEMBERS:
//            handle_getting_chat_members(json_string);
//            break;
        case UNKNOWN_REQUEST:
            log_to_file("Could not process a client request. A non-existent request type was received", ERROR);
            break;
        default:
            break;
    }

    close(client_info->client_socket); //todo do we need to do it?
    pthread_exit(NULL); // todo припиняє виконання поточного потоку, але інші потоки можуть продовжувати працювати
    return;
}
