#pragma once

#include "client.h"

// functions from "get_server_response.c" file
void controller(void);
int process_data_from_controller(gpointer data);
char *read_client_socket(void);
void reconnect_to_server(void);
t_request_type parse_request_type(char *json_string);

// functions from "process_server_response.c" file
void process_server_response(t_request_type request_type, char *json_string);
bool handle_auth_response(char *json_string);

// functions from "send_request_to_server.c" file
void send_registration_request(SSL *ssl, t_request_type request_type, t_user_data *user_data);
void send_login_request(SSL *ssl, t_request_type request_type, t_user_data *user_data);

