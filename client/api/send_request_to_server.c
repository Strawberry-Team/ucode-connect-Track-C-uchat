#include "client.h"

void send_login_req_to_server(SSL *ssl, t_request_type request_type, t_user_data *user_data) {
    char *json_string = NULL;
    cJSON *json = cJSON_CreateObject();
    cJSON *json_credentials_obj = cJSON_CreateObject();

    if (!json
        || !json_credentials_obj) {
        log_to_file("Could not create the cJSON object to send a login request to the server", ERROR);
        cJSON_Delete(json_credentials_obj);
        cJSON_Delete(json);
        return;
    }

    if (!cJSON_AddNumberToObject(json, "request_type", request_type)
        || !cJSON_AddItemToObject(json, "credentials", json_credentials_obj)
        || !cJSON_AddStringToObject(json_credentials_obj, "username", user_data->username)
        || !cJSON_AddStringToObject(json_credentials_obj, "password", user_data->password)) {
        log_to_file("Could not add the data in the cJSON object to send a login request to the server", ERROR);
        cJSON_Delete(json);
        return;
    }

    json_string = cJSON_Print(json); // todo Друкує JSON-об'єкт або масив у форматі, що включає відступи та нові рядки. альтернатива cJSON_PrintUnformatted() - без відступів та нових рядків.

    int bytes_written = SSL_write(ssl, json_string, strlen(json_string));

    if (bytes_written <= 0) {
        log_to_file("Could not write JSON string over the TLS/SSL connection to send a login request to the server", ERROR);
    }

    free(json_string); // todo або mx_strdel(&json_string);
    cJSON_Delete(json);
    return;
}

bool handle_login_response(char *json_string) {
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        const char *error_ptr = cJSON_GetErrorPtr();

        if (error_ptr != NULL) {
            char msg[200];
            sprintf(msg, "Could not parse the cJSON object from server. A pointer to the error location: [%s]", error_ptr);
            log_to_file(msg, ERROR);
        }

        cJSON_Delete(json);
        return false;
    }

    const cJSON *json_status = cJSON_GetObjectItemCaseSensitive(json, "status");
    int request_status;

    if (!cJSON_IsNumber(json_status)
        || json_status->valueint < 0
        || json_status->valueint >= STATUS_TYPE_COUNT) {
        request_status = UNKNOWN_STATUS;
        log_to_file("Could not parse the \"status\" field from a cJSON object", ERROR);
        cJSON_Delete(json);
        return false;
    }

    request_status = json_status->valueint;

    if (request_status != SUCCESS_VALID_CREDENTIALS) {
//        handle_error_response(); // todo CREATE THIS FOO
        // todo создать вывод сообщения об ошибке на фронте
        log_to_file("Wrong status for login request. Expected: SUCCESS_VALID_CREDENTIALS", ERROR);
        cJSON_Delete(json);
        return false;
    }

    const cJSON *json_data_obj = cJSON_GetObjectItemCaseSensitive(json, "data");

    if (cJSON_IsObject(json_data_obj)) {
        const cJSON *json_id = cJSON_GetObjectItemCaseSensitive(json_data_obj, "id");
        const cJSON *json_username = cJSON_GetObjectItemCaseSensitive(json_data_obj, "username");
        const cJSON *json_password = cJSON_GetObjectItemCaseSensitive(json_data_obj, "password");
        const cJSON *json_icon_id = cJSON_GetObjectItemCaseSensitive(json_data_obj, "icon_id");

        if (cJSON_IsNumber(json_id)
            && cJSON_IsString(json_username)
            && cJSON_IsString(json_password)
            && cJSON_IsNumber(json_icon_id)) {
            client->id = json_id->valueint;
            client->username = strdup((const char *)json_username->valuestring);
            client->password = strdup((const char *)json_password->valuestring);
            client->icon_id = json_icon_id->valueint;
        }
    } else {
        log_to_file("Could not parse the \"data\" field from a cJSON object", ERROR);
        cJSON_Delete(json);
        return false;
    }

    // todo printf for testing
    printf("RESPONSE:\nUser data:\nid: %d\nusername: %s\npassword: %s\nicon_id: %d\n", client->id, client->username, client->password, client->icon_id);
    char msg[200];
    sprintf(msg, "RESPONSE:\nUser data:\nid: %d\nusername: %s\npassword: %s\nicon_id: %d\n", client->id, client->username, client->password, client->icon_id);
    log_to_file(msg, INFO);
    cJSON_Delete(json);
    return true;
}


void process_server_response(t_request_type request_type, char *json_string) {
    if (request_type != REGISTER
        && request_type != LOGIN
        && request_type != UNKNOWN_REQUEST) {
        if (!handle_login_response(json_string)) {
            log_to_file("Could not authenticate the user before processing the client request", ERROR);
            free_and_exit();
        }
    }

    switch (request_type) {
//        case REGISTER:
//            handle_registration(json_string);
//            break;
        case LOGIN:
            handle_login_response(json_string);
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

    return;
}

t_request_type parse_request_type(char *json_string) {
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        const char *error_ptr = cJSON_GetErrorPtr();

        if (error_ptr != NULL) {
            char msg[200];
            sprintf(msg, "Could not parse the cJSON object from server. A pointer to the error location: [%s]", error_ptr);
            log_to_file(msg, ERROR);
        }
        cJSON_Delete(json);
        return UNKNOWN_REQUEST;
    }

    t_request_type request_type;
    const cJSON *json_req_type = cJSON_GetObjectItemCaseSensitive(json, "request_type");

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

void reconnect_to_server(void) {
    log_to_file("The reconnection to server is started", INFO);

    while (true) {
        sleep(3);

        client->client_socket = socket(AF_INET, SOCK_STREAM, 0);

        if (client->client_socket < 0) {
            perror("Socket creation failed");
            continue;
        }

        if (connect(client->client_socket, (struct sockaddr *) &(server->address), sizeof(server->address)) != 0) {
            perror("Couldn't connect with the server");
            close(client->client_socket);
            continue;
        }

        client->ssl = SSL_new(client->context);

        if (!client->ssl) {
            perror("Creation of a new SSL structure failed");
            close(client->client_socket);
            continue;
        }

        if (!SSL_set_fd(client->ssl, client->client_socket)) {
            perror("Unable to set file descriptor as input/output device for TLS/SSL side");
            close(client->client_socket);
            SSL_free(client->ssl);
            continue;
        }

        if (SSL_connect(client->ssl) != 1) {
            perror("The TLS/SSL handshake was not successful");
            close(client->client_socket);
            SSL_free(client->ssl);
            continue;
        }

        perror("The reconnection to server was successful");
        log_to_file("The reconnection to server was successful", INFO);
        break;
    }
}

/* Read the information received from the server and write it as a string */
char *read_client_socket(void) {
    char *buffer = (char *) malloc(BUF_SIZE * sizeof(char));
    int bytes_read = 0;
    int total_bytes_read = 0;

    while (true) {
        bytes_read = SSL_read(client->ssl, buffer + total_bytes_read, BUF_SIZE - total_bytes_read);

        if (bytes_read <= 0) {
            int error_code = SSL_get_error(client->ssl, bytes_read);

            if (error_code == SSL_ERROR_WANT_READ
                 || error_code == SSL_ERROR_WANT_WRITE) {
                log_ssl_err_to_file("There is still unprocessed data available at the TLS/SSL connection. Continue reading...");
                sleep(1);
                continue;
            } else {
                log_ssl_err_to_file("Connection is closed");
                // todo reconnect_SSL_foo() must be added
                reconnect_to_server();
                return NULL;
            }
        }

        total_bytes_read += bytes_read;

        if (total_bytes_read >= BUF_SIZE) { /* буфер переповнений */
            log_to_file("Buffer for reading is overflowing", ERROR);
            return NULL;
        } else if (buffer[total_bytes_read] == '\0') { /* кінець файлу, дані повністю прочитані */
            break;
        }
    }

//    buffer[total_bytes_read] = '\0';
    return strdup(buffer);
}

void controller(void) {
    char *json_string = NULL;
    t_request_type request_type;

    while (true) {
        json_string = read_client_socket();

        if (!json_string) {
            continue;
        }

        request_type = parse_request_type(json_string);
        process_server_response(request_type, json_string);
        mx_strdel(&json_string); // todo change to standard foo?
    }

    return;
}

