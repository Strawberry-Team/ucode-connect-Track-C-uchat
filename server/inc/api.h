#pragma once

#include "server.h"


bool handle_login_request(t_client *client_info, t_request_type request_type, char *json_string);
char *hash_password(const char *password);
char *read_client_socket(SSL *ssl);
t_request_type parse_request_type(char *json_string);
void *thread_controller(void *arg);
void handle_add_chat_request(t_client *client_info, char *json_string);
void handle_add_message_request(t_client *client_info, char *json_string);
void handle_delete_message_request(t_client *client_info, char *json_string);
void handle_get_chats_request(t_client *client_info);
void handle_get_messages_request(t_client *client_info, char *json_string);
void handle_get_users_for_search_request(t_client *client_info);
void handle_registration_request(t_client *client_info, char *json_string);
void handle_update_message_request(t_client *client_info, char *json_string);
void handle_update_user_request(t_client *client_info, char *json_string);
void log_status_to_file(char *message, char *value);
void process_client_request(t_client *client_info, t_request_type request_type, char *json_string);
void send_add_chat_response(SSL *ssl, t_status_type status, t_chat_data *chat_data);
void send_auth_response(SSL *ssl, t_request_type request_type, t_status_type status, t_user_data *user_data);
void send_get_chats_response(SSL *ssl, t_status_type status, t_list *chat_list);
void send_get_messages_response(SSL *ssl, t_status_type status, t_list *msg_list);
void send_get_users_for_search_response(SSL *ssl, t_status_type status, t_list *user_list);
void send_message_response(SSL *ssl, t_request_type request_type, t_status_type status, t_msg_data *message_data);
void send_status_response(SSL *ssl, t_request_type request_type, t_status_type status);

