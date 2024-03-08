#include "server.h"
//  COMPILE:
// cd server/
// clang -std=c11 -Wall -Wextra -Werror -Wpedantic src/*.c -o server -I inc

//    // provides required data types
//#include <sys/types.h>
//// holds address family and socket functions
//#include <sys/socket.h>
//// has the sockaddr_in structure
//#include <netinet/in.h>
//// has functions for read and write operations
//#include <fcntl.h>
//// basic C header
//#include <stdio.h>
//// header to help with strings
//#include <string.h>
//// has macros such as EXIT_FAILURE
//#include<stdlib.h>
//// port through which connection is to be made
//#define CONNECTION_PORT 8090

void create_deamon(void) {
    pid_t pid = fork();
    pid_t sid = 0;

    if (pid < 0) {
        perror("Failed to create child process\n");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        printf("Deamon started with pid %d\n", pid);
        exit(EXIT_SUCCESS);
    }

    umask(0);
    sid = setsid();

    if (sid < 0) {
        perror("Failed to create session\n");
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int create_socket(void) {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0); // creating the socket with IPv4 domain and TCP protocol

    if (server_socket < 0) {
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    return server_socket;
}

void bind_socket(int server_socket, char *port) {
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET; //  initializing structure elements for address
    server_address.sin_port = htons(atoi(port)); // convert port to network byte order using htons
    server_address.sin_addr.s_addr = INADDR_ANY; // any address available

    // bind the socket with the values address and port from the sockaddr_in structure
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(struct sockaddr)) < 0) {
        perror("Couldn't bind socket\n");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
}

void listen_socket(int server_socket) {
    // listen on specified port with a maximum of 4 requests
    if (listen(server_socket, BACKLOG) < 0) {
        perror("Couldn't listen for connections\n");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
}

int accept_socket(int server_socket) {
//    struct sockaddr_in client_address;
//    socklen_t length_of_address = sizeof(client_address);
    // accept connection signals from the client
        int client_socket = accept(server_socket, NULL, NULL);
//    int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &length_of_address);

    if (client_socket < 0) {
        perror("Couldn't establish connection with client\n");
//        close(server_socket); // todo чи треба закривати сервер, коли не вдалось accept одно клієнта?
        exit(EXIT_FAILURE); // todo можна замінити на return та continue у while
    }

    return client_socket;
}

//t_client *create_new_client(struct sockaddr_in client_address, t_client client) {
//    t_client *new_client = (t_client *) malloc(sizeof(t_client));
//    new_client->address = client_address;
//    new_client->client_socket = client_socket;
////    cli_count++;
////    mx_push_back(&users_list, new_client);
//
//    return new_client;
//}

int *handle_client(t_client *client) {
    client->counter++;

    return &client->counter;
}

int main(int argc, char **argv) {
    if (argc != 2) {
//        perror("usage: ./uchat_server [port]\n, %s");
        printf("usage: ./uchat_server [port]\n, %s", argv[0]);
        return EXIT_FAILURE;
    }

    create_deamon();
//    for (; ;) pause();

    int server_socket = create_socket();
    bind_socket(server_socket, argv[1]);
    listen_socket(server_socket);

    pthread_mutex_t clients_mutex;
    pthread_mutex_init(&clients_mutex, NULL);

    while (true) {
//        struct sockaddr_in client_address;
        int client_socket = accept_socket(server_socket);

        t_client *new_client = (t_client *) malloc(sizeof(t_client));
//        new_client->address = client_address;
        new_client->client_socket = client_socket;


        pthread_t thread;
        int result = pthread_create(&thread, NULL, handle_client, new_client);

        if (pthread_create(&thread, NULL, handle_client, new_client) != 0) {
            perror("Failed to create a thread.\n");
            exit(EXIT_FAILURE);
        }

        pthread_detach(thread);
    }

    close(server_socket);

    return EXIT_SUCCESS;
}

