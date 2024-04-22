#include "api.h"
#include "gui.h"

void *controller(void) {
    char *json_string = NULL;

    while (true) {
        json_string = read_client_socket();

        if (!json_string) {
            continue;
        }

        g_async_queue_push(data_queue, json_string);
    }

    return NULL;
}

int process_data_from_controller(gpointer data) {
    char *json_string = (char *)data;
    t_request_type request_type = parse_request_type(json_string);
    process_server_response(request_type, json_string);
    g_free(json_string);
    return G_SOURCE_REMOVE;
}

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
                continue;
            } else {
                log_to_file("Connection is closed", SSL_ERROR);
                gtk_widget_show(reconection_label);
                gtk_label_set_text(GTK_LABEL(reconection_label), (const gchar*) "Reconecting...");
                gtk_widget_set_opacity(reconection_label, 1.0);
                show_reconnect();
                reconnect_to_server();
                gtk_label_set_text(GTK_LABEL(reconection_label), "");
                gtk_widget_set_opacity(reconection_label, 0.0);
                recon_closed();
                return NULL;
            }
        }

        total_bytes_read += bytes_read;

        if (total_bytes_read >= BUF_SIZE) {
            log_to_file("Buffer for reading is overflowing", SSL_ERROR);
            return NULL;
        } else if (buffer[total_bytes_read] == '\0') {
            break;
        }
    }

    return mx_strdup(buffer);
}

void reconnect_to_server(void) {
    log_to_file("The reconnection to server is started", INFO);
    perror("The reconnection to server is started");

    while (true) {
        sleep(3);
        client_info->client_socket = socket(AF_INET, SOCK_STREAM, 0);

        if (client_info->client_socket < 0) {
            log_to_file("Socket creation failed", ERROR);
            continue;
        }

        if (connect(client_info->client_socket, (struct sockaddr *) &(server_info->address), sizeof(server_info->address)) != 0) {
            log_to_file("Couldn't connect with the server", ERROR);
            close(client_info->client_socket);
            continue;
        }

        client_info->ssl = SSL_new(client_info->context);

        if (!client_info->ssl) {
            log_to_file("Creation of a new SSL structure failed", ERROR);
            close(client_info->client_socket);
            continue;
        }

        if (!SSL_set_fd(client_info->ssl, client_info->client_socket)) {
            log_to_file("Unable to set file descriptor as input/output device for TLS/SSL side", SSL_ERROR);
            close(client_info->client_socket);
            SSL_free(client_info->ssl);
            continue;
        }

        if (SSL_connect(client_info->ssl) != 1) {
            log_to_file("The TLS/SSL handshake was not successful", SSL_ERROR);
            close(client_info->client_socket);
            SSL_free(client_info->ssl);
            continue;
        }

        log_to_file("The reconnection to server was successful", INFO);
        perror("The reconnection to server was successful");
        break;
    }
}

t_request_type parse_request_type(char *json_string) {
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        const char *error_ptr = cJSON_GetErrorPtr();

        if (error_ptr != NULL) {
            log_to_file("Could not parse the cJSON object from client", JSON_ERROR);
        }

        return UNKNOWN_REQUEST;
    }

    t_request_type request_type;
    const cJSON *json_req_type = cJSON_GetObjectItemCaseSensitive(json, "request_type");

    if (!cJSON_IsNumber(json_req_type)
        || json_req_type->valueint < 0
        || json_req_type->valueint >= REQUEST_TYPE_COUNT) {
        request_type = UNKNOWN_REQUEST;
        log_to_file("Could not parse the \"request_type\" field from a cJSON object", JSON_ERROR);
        cJSON_Delete(json);
        return request_type;
    }

    request_type = json_req_type->valueint;
    cJSON_Delete(json);
    return request_type;
}

