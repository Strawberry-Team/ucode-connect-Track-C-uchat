#pragma once

#include "libmx.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <openssl/ssl.h> // -I /opt/homebrew/include
#include <openssl/err.h> // -I /opt/homebrew/include
#include <cjson/cJSON.h> // -I /opt/homebrew/include

#define BUF_SIZE 100000
#define REQUEST_TYPE_COUNT 15
#define STATUS_TYPE_COUNT 10
#define LOG_FILE "client/client.log"

/* Enumeration of status types for logging data to the log file */
typedef enum e_log_type {
    INFO,
    ERROR,
    CJSON_ERROR,
    SSL_ERROR,
    GTK_ERROR
} t_log_type;

/* Enumeration of request processing status types */
typedef enum e_status_type {
    SUCCESS,
    SUCCESS_VALID_CREDENTIALS,
    SUCCESS_CHAT_EXIST,
    SUCCESS_USER_IN_CHAT,
    ERROR_JSON,
    ERROR_DB,
    ERROR_INVALID_CREDENTIALS,
    ERROR_CHAT_NONEXIST,
    ERROR_USER_NOT_IN_CHAT,
    ERROR_USER_DONT_HAVE_PERMISSION,
    UNKNOWN_STATUS = -1
} t_status_type;

/* Enumeration of request types */
typedef enum e_request_type { // todo підтримувати акутальним значення REQUEST_TYPE_COUNT
    REGISTER,
    LOGIN,
    CREATE_CHAT,
    ADD_MEMBER_TO_CHAT,
    GET_USER_CHATS,
    GET_MESSAGES_IN_CHAT,
    SEND_MESSAGE_AND_GET_MESSAGE_UPDATES,
    DELETE_MESSAGE_AND_GET_MESSAGE_UPDATES,
    CHANGE_MESSAGE_AND_GET_MESSAGE_UPDATES,
    REPLY_TO_MESSAGE_AND_GET_MESSAGE_UPDATES,
    GET_MESSAGE_UPDATES,
    GET_CHAT_MEMBERS,
    LOGOUT,
    UNKNOWN_REQUEST = -1
} t_request_type;

typedef struct s_server {
    struct sockaddr_in address;
//    bool is_run;
//    bool connected;
//    bool loaded;
} t_server;

typedef struct s_client {
    int client_socket;
    SSL *ssl;
    SSL_CTX *context;
    int id;
    char *username;
    char *password;
    int icon_id;
} t_client;

typedef struct s_user_data {
    char *username;
    char *password; // якщо "unsigned char *password", то для SHA-256 хеша розмір поля буде 32 байти.
    int icon_id; // todo do we need it in GTK?
} t_user_data;

extern t_server *server_info;
extern t_client *client_info;

// functions from "client.c" file
void log_to_file(char *message, t_log_type log_type);
//void log_ssl_err_to_file(char *message);
SSL_CTX *create_context(void);
void free_and_exit(void);

