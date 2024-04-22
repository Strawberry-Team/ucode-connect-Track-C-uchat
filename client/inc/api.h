#pragma once

#include "client.h"
#include "gui.h"
#include <unistd.h>


bool handle_auth_response(char *json_string);
char *read_client_socket(void);
int process_data_from_controller(gpointer data);
t_chat_data *find_chat_in_list(int chat_id);
t_msg_data *find_message_in_list(int message_id);
t_request_type parse_request_type(char *json_string);
void convert_timestamp_to_string(char **string, time_t timestamp);
void handle_add_chat_response(char *json_string);
void handle_get_chats_response(char *json_string);
void handle_get_messages_response(char *json_string);
void handle_get_users_for_search_response(char *json_string);
void handle_message_response(char *json_string);
void log_status_to_file(char *message, char *value);
void process_server_response(t_request_type request_type, char *json_string);
void reconnect_to_server(void);
void send_add_chat_request(SSL *ssl, t_user_data *user_data, int invitee_user_id);
void send_add_message_request(SSL *ssl, t_user_data *user_data, t_msg_data *message_data);
void send_auth_with_request_type(SSL *ssl, t_request_type request_type, t_user_data *user_data);
void send_delete_message_request(SSL *ssl, t_user_data *user_data, t_msg_data *msg_data);
void send_get_chats_request(SSL *ssl, t_user_data *user_data);
void send_get_messages_request(SSL *ssl, t_user_data *user_data, int chat_id);
void send_get_users_for_search_request(SSL *ssl, t_user_data *user_data);
void send_login_request(SSL *ssl, t_user_data *user_data);
void send_registration_request(SSL *ssl, t_user_data *user_data);
void send_update_message_request(SSL *ssl, t_user_data *user_data, t_msg_data *msg_data);
void send_update_user_request(SSL *ssl, t_user_data *user_data, char *password);
void* controller(void);

