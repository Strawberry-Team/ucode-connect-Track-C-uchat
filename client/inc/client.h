#pragma once

#include "libmx.h"
#include <arpa/inet.h>
#include <netinet/in.h>

typedef struct s_server {
    struct sockaddr_in address;
//    bool is_run;
//    bool connected;
//    bool loaded;
} t_server;

typedef struct s_client {
    int client_socket;
//    struct sockaddr_in address;
//    int id;
//    char *login;
//    const char *password;
//    char *avatarname;
} t_client;

extern t_server *server;