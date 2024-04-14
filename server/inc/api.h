#pragma once

#include "server.h"

/* Functions from "get_client_request.c" file */
void *thread_controller(void *arg);
char *read_client_socket(SSL *ssl);
t_request_type parse_request_type(char *json_string);

/* Functions from "process_client_request.c" file */
void process_client_request(t_client *client_info, t_request_type request_type, char *json_string);
void log_status_to_file(char *message, char *value);
void handle_registration(t_client *client_info, char *json_string);
bool handle_login(t_client *client_info, char *json_string);

/* Functions from "send_response_to_client.c" file */
void send_status_response(SSL *ssl, t_request_type request_type, t_status_type status);
void send_auth_response(SSL *ssl, t_request_type request_type, t_status_type status, t_user_data *user_data);

