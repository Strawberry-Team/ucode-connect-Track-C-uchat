// cd client/src/ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic -c *.c -o client.o -I ../inc -I ../../libraries/libmx/inc && cd ../../ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic client/src/*.o -I client/inc -I libraries/libmx/inc -L libraries/libmx -lmx -o uchat && ./uchat 127.0.0.1 8090


#include "client.h"

t_server *server;
t_client *client;

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("usage: %s [ip] [port]\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    server = (t_server *) malloc(sizeof(t_server));
    server->address.sin_family = AF_INET;
    server->address.sin_port = htons(atoi(argv[2]));
//    server->address.sin_addr.s_addr = argv[1];

    if (inet_pton(AF_INET, argv[1], &(server->address.sin_addr)) <= 0) {
        perror("inet_pton error occured");
        exit(EXIT_FAILURE);
    }

    client = (t_client *) malloc(sizeof(t_client));
    client->client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client->client_socket < 0) {
        perror("Socket creation failed");
        free(server);
        free(client);
        exit(EXIT_FAILURE);
    }

    if (connect(client->client_socket,(struct sockaddr *)&(server->address), sizeof(server->address)) != 0) {
        perror("Couldn't connect with the server");
        close(client->client_socket);
        free(client);
        free(server);
        exit(EXIT_FAILURE);
    }

    // todo create reconnect foo

    // todo request_handler

    return 0;
}
