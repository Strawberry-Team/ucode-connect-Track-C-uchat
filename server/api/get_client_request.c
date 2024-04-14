#include "api.h"

void *thread_controller(void *arg) {
    t_client *client_info = (t_client *) arg;
    char *json_string = NULL;
    t_request_type request_type;

    while (true) {
        json_string = read_client_socket(client_info->ssl);

        if (!json_string) {
            continue;
        }

        request_type = parse_request_type(json_string);
        process_client_request(client_info, request_type, json_string);
        mx_strdel(&json_string);
    }

    return NULL;
}

/* Read the information received from the client and write it as a string */
char *read_client_socket(SSL *ssl) {
    char *buffer = (char *) malloc(BUF_SIZE * sizeof(char));
    int bytes_read = 0;
    int total_bytes_read = 0;

    while (true) {
        log_to_file("SSL_read before", INFO);
        bytes_read = SSL_read(ssl, buffer + total_bytes_read, BUF_SIZE - total_bytes_read);
        log_to_file("SSL_read after", INFO);

        if (bytes_read <= 0) {
            int error_code = SSL_get_error(ssl, bytes_read);

            if (error_code == SSL_ERROR_WANT_READ
                || error_code == SSL_ERROR_WANT_WRITE) {
                log_to_file("There is still unprocessed data available at the TLS/SSL connection. Continue reading...", SSL_ERROR);
                sleep(3);
                continue;
            } else {
                log_to_file("Connection is closed", SSL_ERROR);
                pthread_exit(NULL);
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

    t_request_type request_type;
    const cJSON *json_req_type = cJSON_GetObjectItem(json, "request_type");

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

