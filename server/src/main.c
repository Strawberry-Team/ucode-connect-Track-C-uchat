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
//#define CONNECTION_PORT 3500

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





int socket() {
    int socket_descriptor;


    // socket created by the bind function
    int client_socket;
    // buffer to store message
    char storage_buffer[80];
    int length_of_address;
    // server and client address structures
    struct sockaddr_in connection_address;
    char* message = "This is a message from the server";



    socket_descriptor = socket(AF_INET, SOCK_STREAM, 0); // creating the socket with IPv4 domain and TCP protocol

    if (socket_descriptor < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    struct sockaddr_in socket_address = {0}; // або це або рядки 78, 79 // todo
    server_address.sin_family = AF_INET; //  initializing structure elements for address
    server_address.sin_port = htons(CONNECTION_PORT); // convert port to network byte order using htons
    server_address.sin_addr.s_addr = INADDR_ANY; // any address available
    server_address.sin_zero[8] = '\0'; // Assigning null character to the last index of the character array

    status = bind(socket_descriptor, (struct sockaddr*)&server_address, sizeof(struct sockaddr)); // bind the socket with the values address and port from the sockaddr_in structure
    if (status < 0) {
        perror("Couldn't bind socket");
        exit(EXIT_FAILURE);
    }

// listen on specified port with a maximum of 4 requests
    status = listen(socket_descriptor, 4);
// check if the socket is listening successfully
    if (status < 0) {
        perror("Couldn't listen for connections");
        exit(EXIT_FAILURE);
    }

    length_of_address = sizeof(connection_address);
// accept connection signals from the client
    client_socket = accept(socket_descriptor, (struct sockaddr*)&connection_address, &length_of_address);
// check if the server is accepting the signals from the client
    if (client_socket < 0) {
        perror("Couldn't establish connection with client");
        exit(EXIT_FAILURE);
    }
// Receive data sent by the client
    read(client_socket, storage_buffer, 80);
// set the last index of the character array as a null character
    storage_buffer[bytes_read] = '\0';
    printf("Message from client: %s \n",storage_buffer);
// Send data to the client
    send(client_socket, message, strlen(message), 0);
// Close all the sockets created
    close(socket_descriptor);
    close(client_socket);
    return 0;
}







int main(int argc, char **argv) {
    if (argc != 2) {
        perror("usage: ./uchat_server [port]\n, %s");
//        printf("usage: ./uchat_server [port]\n, %s", argv[0]);
        return EXIT_FAILURE;
    }

    create_deamon();
//    for (; ;) pause();



    int listening_socket = create_socket();
    bind_socket(listening_socket, atoi(argv[1]));
    listen_socket(listening_socket, 5);

    while (true) {
        int client_socket = accept_socket(listening_socket);
        int *allocated_client_socket = malloc(sizeof(int));
        *allocated_client_socket = client_socket;
        create_detached_thread(handle_client_request_thread, allocated_client_socket);
    }




    return EXIT_SUCCESS;
}

