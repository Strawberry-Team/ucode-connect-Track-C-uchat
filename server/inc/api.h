#pragma once

#include "server.h"
#include "database.h"

// functions from "process_client_request.c" file
bool handle_login(cJSON *json);

// functions from "send_response_to_client.c" file
void send_status_response(SSL *ssl, t_request_type request_type, t_status_type status);
void send_login_response(SSL *ssl, t_request_type request_type, t_status_type status, t_user_data *user_data);
