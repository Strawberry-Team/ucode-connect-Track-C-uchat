#pragma once

#include "libmx.h"
#include "gui.h"
#include <arpa/inet.h>
#include <cjson/cJSON.h>
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>

#define BUF_SIZE 1000000
#define LOG_FILE "client/client.log"
#define REQUEST_TYPE_COUNT 12
#define STATUS_TYPE_COUNT 5


typedef enum e_log_type {
    INFO,
    ERROR,
    JSON_ERROR,
    SSL_ERROR
} t_log_type;

typedef enum e_status_type {
    SUCCESS,
    ERROR_JSON,
    ERROR_DB,
    ERROR_INVALID_CREDENTIALS,
    UNKNOWN_STATUS = -1
} t_status_type;

typedef enum e_request_type {
    REGISTER,
    LOGIN,
    UPDATE_USER,
    GET_USERS_FOR_SEARCH,
    ADD_CHAT,
    GET_CHATS,
    GET_MESSAGES,
    ADD_MESSAGE,
    UPDATE_MESSAGE,
    DELETE_MESSAGE,
    LOGOUT,
    UNKNOWN_REQUEST = -1
} t_request_type;

typedef struct s_server {
    struct sockaddr_in address;
} t_server;

typedef struct s_client {
    int client_socket;
    SSL *ssl;
    SSL_CTX *context;
    int id;
    char *username;
    char *password;
} t_client;

typedef struct s_user_data {
    int id;
    char *username;
    char *password;
} t_user_data;

typedef struct s_chat_data {
    int id;
    char *title;
} t_chat_data;

typedef struct s_msg_data {
    int message_id;
    int user_id;
    char *username;
    int chat_id;
    char *body;
    time_t created_at;
    time_t updated_at;
    time_t deleted_at;
} t_msg_data;


extern t_client *client_info;
extern t_list *chat_list;
extern t_list *message_list;
extern t_server *server_info;


gboolean check_and_process_data(void);
gpointer controller_thread(gpointer data);
SSL_CTX *create_context(void);
void free_and_exit(void);
void log_to_file(char *message, t_log_type log_type);

