#pragma once

//#include "socket.h"
//#include "threads.h"
//#include "database.h"
//#include "request_handlers.h"
#include <stdio.h>
#include <stdlib.h>
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
    int counter;
//    int id;
//    char *login;
//    char *passwd;
//    bool connected;
} t_client;

/**
 * @return nothing but turns server into the daemon state
*/
void create_deamon(void);
int create_socket(void);
void bind_socket(int server_socket, char *port);
void listen_socket(int server_socket);
int accept_socket(int server_socket);
//t_client *create_new_client(const struct sockaddr_in client_address, int client_socket);
int *handle_client(t_client *client);

// #include "threads.h"
void create_detached_thread(void *(*func)(void *), void *arg);

