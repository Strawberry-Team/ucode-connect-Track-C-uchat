#include "server.h"

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
        perror("Failed to create child process");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        printf("Deamon started with pid %d\n", pid);
        exit(EXIT_SUCCESS);
    }

    umask(0);
    sid = setsid();

    if (sid < 0) {
        perror("Failed to create session");
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int create_socket() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0); // creating the socket with IPv4 domain and TCP protocol

    if (server_socket < 0) {
        perror("Socket creation failed");
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
        perror("Couldn't bind socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
}

void listen_socket(int server_socket) {
    // listen on specified port with a maximum of 4 requests
    if (listen(server_socket, BACKLOG) < 0) {
        perror("Couldn't listen for connections");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
}

int accept_socket(int server_socket) {
    struct sockaddr_in client_address;
    int length_of_address = sizeof(client_address);
    // accept connection signals from the client
    int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &length_of_address);

    if (client_socket < 0) {
        perror("Couldn't establish connection with client");
//        close(server_socket); // todo чи треба закривати сервер, коли не вдалось accept одно клієнта?
        exit(EXIT_FAILURE); // todo можна замінити на return та continue у while
    }

    return client_socket;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        perror("usage: ./uchat_server [port]\n, %s");
//        printf("usage: ./uchat_server [port]\n, %s", argv[0]);
        return EXIT_FAILURE;
    }

    create_deamon();
//    for (; ;) pause();

    int server_socket = create_socket();
    bind_socket(server_socket, argv[1]);
    listen_socket(server_socket);

    while (true) {
        int client_socket = accept_socket(server_socket);
        int *allocated_client_socket = malloc(sizeof(int));
        *allocated_client_socket = client_socket;
        create_detached_thread(handle_client_request_thread, allocated_client_socket);
    }

    return EXIT_SUCCESS;
}

