#include "client.h"

t_server *server;

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
        printf("inet_pton error occured");
        exit(EXIT_FAILURE);
    }

}