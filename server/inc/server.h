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

#define BACKLOG 4
#define BUF_SIZE 100000
#define REQUEST_TYPE_COUNT 15
#define LOG_FILE "server/server_errors.log"
#define CERTIFICATE "server/ssl/certificate.crt"
#define PRIVATE_KEY "server/ssl/private_key.key"

typedef struct s_client {
//    int serv_fd;
//    struct sockaddr_in address;
    int client_socket;
    SSL *ssl;
    char *username;
    char *password;
//    bool connected;
} t_client;

typedef enum e_request_type { // todo підтримувати акутальним значення REQUEST_TYPE_COUNT
    LOGIN, // -> login -> password
    REGISTER, // -> login -> password
    CREATE_CHAT, // -> chat_name -> user_id
    ADD_MEMBER_TO_CHAT, // -> chat_id -> member_login
    GET_USER_CHATS, // -> user_id
    SEND_MESSAGE, // -> user_id -> chat_id -> text_message
    GET_MESSAGES_IN_CHAT,
    SEND_MESSAGE_AND_GET_MESSAGE_UPDATES,
    DELETE_MESSAGE_AND_GET_MESSAGE_UPDATES,
    CHANGE_MESSAGE_AND_GET_MESSAGE_UPDATES,
    REPLY_TO_MESSAGE_AND_GET_MESSAGE_UPDATES,
    GET_MESSAGE_UPDATES,
    GET_CHAT_MEMBERS,
    LOGOUT,
    UNKNOWN_TYPE = -1
} t_request_type;

extern pthread_mutex_t clients_mutex;
extern t_list *user_list;
extern t_client *client_info;

// functions from "server.c" file
void log_to_file(char *message);
void log_ssl_err_to_file(char *message);
void create_deamon(void);
int create_socket(void);
void bind_socket(int server_socket, char *port);
void listen_socket(int server_socket);
void free_clients(void);

SSL_CTX *create_context(void);
bool configure_context(SSL_CTX *context);
//void *handle_client(void *args);

// functions from "define_client_request.c" file
char *read_client_socket(SSL *ssl);
void parse_request_type(char *json_string);
void *thread_controller(void *arg);
void process_client_request(t_request_type request_type, cJSON *json);

//// functions from "process_client_request.c" file
//bool handle_login(cJSON *json);

