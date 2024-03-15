#pragma once

#include "libmx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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

#define BACKLOG 4
#define LOG_FILE "server/server_errors.log"

typedef struct s_client {
//    int serv_fd;
//    struct sockaddr_in address;
    int client_socket;
//    char *login;
//    char *passwd;
//    bool connected;
} t_client;

extern pthread_mutex_t clients_mutex;
extern t_list *user_list;

void log_to_file(char *message);
void create_deamon(void);
int create_socket(void);
void bind_socket(int server_socket, char *port);
void listen_socket(int server_socket);
int accept_socket(int server_socket);
void *handle_client(void *args);
void free_clients(void);

