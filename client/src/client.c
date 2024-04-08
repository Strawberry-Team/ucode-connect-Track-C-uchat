// DEPRECATED --- cd client/src/ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic -c *.c -o client.o -I ../inc -I ../../libraries/libmx/inc && cd ../../ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic client/src/*.o -I client/inc -I libraries/libmx/inc -L libraries/libmx -lmx -o uchat && ./uchat 127.0.0.1 8090
// cd client/obj/ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic -c ../src/*.c ../api/*.c -I ../inc -I ../../libraries/libmx/inc -I /opt/homebrew/include -I /opt/homebrew/opt/ -I /usr/bin/&& cd ../../ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic client/obj/*.o -I client/inc -I libraries/libmx/inc -L libraries/libmx -lmx -L /opt/homebrew/lib -lssl -lcrypto -lcjson -L /opt/homebrew/opt/sqlite/lib -lsqlite3 -o uchat && ./uchat 127.0.0.1 8090


#include "client.h"

t_server *server;
t_client *client;

void log_to_file(char *message) {
    FILE *log_file = fopen(LOG_FILE, "a");
    time_t current_time;
    struct tm *time_info;
    char time_string[80];
    time(&current_time); // Get the current time
    time_info = localtime(&current_time); // Convert the current time to local time
    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", time_info); // Formatting the time into a string
    fprintf(log_file, "[%s]\tPID %d\t%s: %s\n", time_string, getpid(), message, strerror(errno)); //Write a log message
    free(time_info);
    fclose(log_file);
}

void log_ssl_err_to_file(char *message) {
    FILE *log_file = fopen(LOG_FILE, "a");
    log_to_file(message);
    ERR_print_errors_fp(log_file); // todo або використовувати -- void ERR_print_errors_cb(int (*cb)(const char *str, size_t len, void *u), void *u);
    fclose(log_file);
}

SSL_CTX *create_context(void) {
    const SSL_METHOD *method;
    method = TLS_client_method();
    SSL_CTX *context;
    context = SSL_CTX_new(method); // todo NULL for testing log_ssl_err_to_file()

    return context;
}

void free_and_exit(void) {
    if (client->client_socket) {
        shutdown(client->client_socket, SHUT_RDWR);
        close(client->client_socket);
    }

    if (client->ssl) {
        SSL_shutdown(client->ssl);
        SSL_free(client->ssl);
    }

    if (client->context) {
        SSL_CTX_free(client->context);
    }

    free(client);
    free(server);
    exit(EXIT_FAILURE);
}

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
        log_to_file("inet_pton error occured");
        exit(EXIT_FAILURE);
    }

    client = (t_client *) malloc(sizeof(t_client));
    client->client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client->client_socket < 0) {
        perror("Socket creation failed");
        log_to_file("Socket creation failed");
        free_and_exit();
    }

    if (connect(client->client_socket,(struct sockaddr *)&(server->address), sizeof(server->address)) != 0) {
        perror("Couldn't connect with the server");
        log_to_file("Couldn't connect with the server");
        free_and_exit();
    }

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    SSL_CTX *context = create_context();
    SSL *ssl;

    if (!context) {
        perror("Unable to create SSL context");
        log_to_file("Unable to create SSL context");
        free_and_exit();
    }

    client->context = context;
    ssl = SSL_new(context);

    if (!ssl) {
        perror("Creation of a new SSL structure failed");
        log_to_file("Creation of a new SSL structure failed");
        free_and_exit();
    }

    client->ssl = ssl;

    if (!SSL_set_fd(client->ssl, client->client_socket)) {
        perror("Unable to set file descriptor as input/output device for TLS/SSL side");
        log_to_file("Unable to set file descriptor as input/output device for TLS/SSL side");
        free_and_exit();
    }

    if (SSL_connect(client->ssl) != 1) {
        perror("The TLS/SSL handshake was not successful");
        log_to_file("The TLS/SSL handshake was not successful");
        free_and_exit();
    }

    t_user_data *user_data = NULL;
    user_data = (t_user_data *) malloc(sizeof(t_user_data));
    user_data->request_type = LOGIN;
    const char *username = "Inessa";
    user_data->username = strdup(username); // or mx_strdup()
    char *password = "PaSsWoRd";
    user_data->password = strdup(password); // or mx_strdup()

    send_login_to_server(client->ssl, LOGIN, user_data);

    // todo create reconnect foo

    // todo request_handler
    controller(client);

    return 0;
}
