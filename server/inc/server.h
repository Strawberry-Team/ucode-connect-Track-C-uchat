#pragma once
#include "libmx.h"
#include <signal.h> // todo
#include <sys/signal.h> // todo
//#include "socket.h"
//#include "threads.h"
//#include "database.h"
//#include "request_handlers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
// #include "threads.h"
#include <pthread.h>
// socket.h
#define _GNU_SOURCE
#include <poll.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BACKLOG 4



typedef struct s_client {
//    int serv_fd;
//    SSL *ssl; // client ssl structure with coneection to server
//    struct sockaddr_in address;
    int client_socket;
//    char *login;
//    char *passwd;
//    bool connected;
} t_client;

extern pthread_mutex_t clients_mutex;
extern t_list *user_list;

void create_deamon(void);
int create_socket(void);
void bind_socket(int server_socket, char *port);
void listen_socket(int server_socket);
int accept_socket(int server_socket);
//t_client *create_new_client(const struct sockaddr_in client_address, int client_socket);
void *handle_client(void *args);
void free_clients(void);
void signal_handler(int signal);

// #include "threads.h"
void create_detached_thread(void *(*func)(void *), void *arg);

