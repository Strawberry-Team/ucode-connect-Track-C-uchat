// COMPILE && RUN:
// DEPRECATED --- cd client/src/ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic -c *.c -o client.o -I ../inc -I ../../libraries/libmx/inc && cd ../../ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic client/src/*.o -I client/inc -I libraries/libmx/inc -L libraries/libmx -lmx -o uchat && ./uchat 127.0.0.1 8090
// cd client/obj/ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic -c ../src/*.c ../api/*.c -I ../inc -I ../../libraries/libmx/inc -I /opt/homebrew/include -I /opt/homebrew/opt/ -I /usr/bin/&& cd ../../ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic client/obj/*.o -I client/inc -I libraries/libmx/inc -L libraries/libmx -lmx -L /opt/homebrew/lib -lssl -lcrypto -lcjson -L /opt/homebrew/opt/sqlite/lib -lsqlite3 -o uchat && ./uchat 127.0.0.1 8090

#include "client.h"

t_server *server;
t_client *client;

void log_to_file(char *message, t_log_type log_type) {
    FILE *log_file = fopen(LOG_FILE, "a");
    time_t current_time;
    struct tm *time_info;
    char time_string[80];
    /* Get the current time */
    time(&current_time);
    /* Convert the current time to local time */
    time_info = localtime(&current_time);
    /* Formatting the time into a string */
    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", time_info);

    /* Write a log message */
    if (log_type == INFO) {
        fprintf(log_file, "[%s]\tINFO\tPID %d\t%s\n", time_string, getpid(), message);
    } else {
        fprintf(log_file, "[%s]\tERROR\tPID %d\t%s: %s\n", time_string, getpid(), message, strerror(errno));
    }
//    free(time_info); //todo occurs the memory errors
    fclose(log_file);
}

void log_ssl_err_to_file(char *message) {
    FILE *log_file = fopen(LOG_FILE, "a");
    log_to_file(message, ERROR);
    ERR_print_errors_fp(log_file);
    fclose(log_file);
}

SSL_CTX *create_context(void) {
    const SSL_METHOD *method;
    method = TLS_client_method();
    SSL_CTX *context;
    context = SSL_CTX_new(method);

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

    printf("Thread joined\n");
    log_to_file("Thread joined", INFO);
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

    if (inet_pton(AF_INET, argv[1], &(server->address.sin_addr)) <= 0) {
        perror("Could not convert the network address");
        log_to_file("Could not convert the network address", ERROR);
        exit(EXIT_FAILURE);
    }

    client = (t_client *) malloc(sizeof(t_client));
    client->client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client->client_socket < 0) {
        perror("Socket creation failed");
        log_to_file("Socket creation failed", ERROR);
        free_and_exit();
    }

    if (connect(client->client_socket,(struct sockaddr *)&(server->address), sizeof(server->address)) != 0) {
        perror("Couldn't connect with the server");
        log_to_file("Couldn't connect with the server", ERROR);
        free_and_exit();
    }

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    SSL_CTX *context = create_context();
    SSL *ssl;

    if (!context) {
        perror("Unable to create SSL context");
        log_to_file("Unable to create SSL context", ERROR);
        free_and_exit();
    }

    client->context = context;
    ssl = SSL_new(context);

    if (!ssl) {
        perror("Creation of a new SSL structure failed");
        log_to_file("Creation of a new SSL structure failed", ERROR);
        free_and_exit();
    }

    client->ssl = ssl;

    if (!SSL_set_fd(client->ssl, client->client_socket)) {
        perror("Unable to set file descriptor as input/output device for TLS/SSL side");
        log_to_file("Unable to set file descriptor as input/output device for TLS/SSL side", ERROR);
        free_and_exit();
    }

    if (SSL_connect(client->ssl) != 1) {
        perror("The TLS/SSL handshake was not successful");
        log_to_file("The TLS/SSL handshake was not successful", ERROR);
        free_and_exit();
    }

    t_user_data *user_data = NULL;
    user_data = (t_user_data *) malloc(sizeof(t_user_data));
    user_data->request_type = LOGIN;
    const char *username = "Inessa";
    user_data->username = strdup(username);
    char *password = "PaSsWoRd";
    user_data->password = strdup(password);

    send_login_req_to_server(client->ssl, LOGIN, user_data);

    // todo request_handler. do we need threads or something else?
    controller();

    return 0;
}
