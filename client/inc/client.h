#pragma once

#include "libmx.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/ssl.h> // -I /opt/homebrew/include
#include <openssl/err.h> // -I /opt/homebrew/include
#include <cjson/cJSON.h> // -I /opt/homebrew/include

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
//    struct sockaddr_in address;
//    int id;
//    char *login;
//    const char *password;
//    char *avatarname;
} t_client;

//typedef struct s_client {
//    int serv_fd;
//    SSL *ssl;// client ssl structure with coneection to server
//    struct sockaddr_in adr;
//    int cl_socket;
//    int id;
//    char *login;
//    const char *password;
//    char *avatarname;
//} t_client;

extern t_server *server;
extern t_client *client;
