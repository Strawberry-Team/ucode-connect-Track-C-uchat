#include "api.h"

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

/* Read the information received from the server and write it as a string */
char *read_client_socket(void) {
    char *buffer = (char *) malloc(BUF_SIZE * sizeof(char));
    int bytes_read = 0;
    int total_bytes_read = 0;

    while (true) {
        bytes_read = SSL_read(client_info->ssl, buffer + total_bytes_read, BUF_SIZE - total_bytes_read);

        if (bytes_read <= 0) {
            int error_code = SSL_get_error(client_info->ssl, bytes_read);

            if (error_code == SSL_ERROR_WANT_READ
                 || error_code == SSL_ERROR_WANT_WRITE) {
                log_to_file("There is still unprocessed data available at the TLS/SSL connection. Continue reading...", SSL_ERROR);
                sleep(1);
                continue;
            } else {
                log_to_file("Connection is closed", SSL_ERROR);
                // todo reconnect_SSL_foo() must be added
                reconnect_to_server();
                return NULL;
            }
        }

        total_bytes_read += bytes_read;

        if (total_bytes_read >= BUF_SIZE) { /* буфер переповнений */
            log_to_file("Buffer for reading is overflowing", SSL_ERROR);
            return NULL;
        } else if (buffer[total_bytes_read] == '\0') { /* кінець файлу, дані повністю прочитані */
            break;
        }
    }

//    buffer[total_bytes_read] = '\0';
    return strdup(buffer);
}

void reconnect_to_server(void) {
    log_to_file("The reconnection to server is started", INFO);

    while (true) {
        sleep(3);

        client_info->client_socket = socket(AF_INET, SOCK_STREAM, 0);

        if (client_info->client_socket < 0) {
            perror("Socket creation failed");
            continue;
        }

        if (connect(client_info->client_socket, (struct sockaddr *) &(server_info->address), sizeof(server_info->address)) != 0) {
            perror("Couldn't connect with the server");
            close(client_info->client_socket);
            continue;
        }

        client_info->ssl = SSL_new(client_info->context);

        if (!client_info->ssl) {
            perror("Creation of a new SSL structure failed");
            close(client_info->client_socket);
            continue;
        }

        if (!SSL_set_fd(client_info->ssl, client_info->client_socket)) {
            perror("Unable to set file descriptor as input/output device for TLS/SSL side");
            close(client_info->client_socket);
            SSL_free(client_info->ssl);
            continue;
        }

        if (SSL_connect(client_info->ssl) != 1) {
            perror("The TLS/SSL handshake was not successful");
            close(client_info->client_socket);
            SSL_free(client_info->ssl);
            continue;
        }

        perror("The reconnection to server was successful");
        log_to_file("The reconnection to server was successful", INFO);
        break;
    }
}

t_request_type parse_request_type(char *json_string) {
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        const char *error_ptr = cJSON_GetErrorPtr();

        if (error_ptr != NULL) {
            char msg[200];
            sprintf(msg, "Could not parse the cJSON object from server. A pointer to the error location: [%s]", error_ptr);
            log_to_file(msg, CJSON_ERROR);
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
        log_to_file("Could not parse the \"request_type\" field from a cJSON object", CJSON_ERROR);
        cJSON_Delete(json);
        return request_type;
    }

    request_type = json_req_type->valueint;
    cJSON_Delete(json);
    return request_type;
}


