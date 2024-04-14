#pragma once

#include "libmx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <poll.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <openssl/ssl.h> // -I /opt/homebrew/include
#include <openssl/err.h> // -I /opt/homebrew/include
#include <cjson/cJSON.h> // -I /opt/homebrew/include

//#ifdef MACOS_VER
//#define SSL_CERTIFICATE "server/ssl/cert.pem"
//#else
//#define SSL_CERTIFICATE "server/ssl/certificate.crt"
//#endif
//
//#ifdef MACOS_VER
//#define SSL_PRIVATE_KEY "server/ssl/cert-key.pem"
//#else
//#define SSL_PRIVATE_KEY "server/ssl/private_key.key"
//#endif

#define BACKLOG 4
#define BUF_SIZE 100000
#define REQUEST_TYPE_COUNT 15
#define STATUS_TYPE_COUNT 10
#define LOG_FILE "server/server.log"
//#define SSL_CERTIFICATE "server/ssl/certificate.crt" // for NOT Mac OS
//#define SSL_PRIVATE_KEY "server/ssl/private_key.key" // for NOT Mac OS
#define SSL_CERTIFICATE "server/ssl/cert.pem" // for Mac OS
#define SSL_PRIVATE_KEY "server/ssl/cert-key.pem" // for Mac OS

/* Enumeration of status types for logging data to the log file */
typedef enum e_log_type {
    INFO,
    ERROR,
    CJSON_ERROR,
    SSL_ERROR,
    DB_ERROR,
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
    SEND_MESSAGE,
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

typedef struct s_client {
//    int serv_fd;
//    struct sockaddr_in address;
    int client_socket;
    SSL *ssl;
    int id;
    char *username;
    char *password;
    int icon_id;
    bool is_connected;
} t_client;

typedef struct s_user_data {
    int id;
    char *username;
    char *password; // якщо "unsigned char *password", то для SHA-256 хеша розмір поля буде 32 байти.
    int icon_id;
} t_user_data;

/*
 * Який тип даних вибрати для поля "password":
 * char: Цей тип даних може зберігати значення від -128 до 127. Якщо ваш алгоритм хешування генерує хеш-суми в цьому діапазоні, то char є кращим вибором.
 * unsigned char: Цей тип даних може зберігати значення від 0 до 255. Якщо ваш алгоритм хешування генерує хеш-суми в цьому діапазоні, то unsigned char є кращим вибором.
 */

extern pthread_mutex_t logging_mutex;
extern t_list *user_list;

/* Functions from "server.c" file */
void log_to_file(char *message, t_log_type log_type);
void create_deamon(void);
int create_socket(void);
void bind_socket(int server_socket, char *port);
void listen_socket(int server_socket);
void free_clients(void);
SSL_CTX *create_context(void);
bool configure_context(SSL_CTX *context);

