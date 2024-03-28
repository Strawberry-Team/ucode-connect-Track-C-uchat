#include "server.h"
//  COMPILE:
// clang -std=c11 -Wall -Wextra -Werror -Wpedantic src/*.c -o server -I inc
// cd server/src/ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic -c *.c -o server.o -I ../inc -I ../../libraries/libmx/inc -I /opt/homebrew/include && cd ../../ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic server/src/*.o -I server/inc -I libraries/libmx/inc -L libraries/libmx -lmx -L /opt/homebrew/lib -lssl -lcrypto -o uchat_server && ./uchat_server 8090
// kill PID

pthread_mutex_t clients_mutex;
t_list *user_list;

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

void create_deamon(void) {
    pid_t pid = fork();
    pid_t sid = 0;

    if (pid < 0) {
        log_to_file("Failed to create child process");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        printf("Deamon started with pid %d\n", pid);
        exit(EXIT_SUCCESS);
    }

    umask(0);
    sid = setsid();

    if (sid < 0) {
        log_to_file("Failed to create session");
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int create_socket(void) {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0); // creating the socket with IPv4 domain and TCP protocol

    if (server_socket < 0) {
        log_to_file("Socket creation failed");
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
        log_to_file("Couldn't bind socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
}

void listen_socket(int server_socket) {
    // listen on specified port with a maximum of 4 requests
    if (listen(server_socket, BACKLOG) < 0) {
        log_to_file("Couldn't listen for connections");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
}

void *handle_client(void *args) {
    printf("Test functional of handle_client %s", (char *)args);
    return NULL;
}

void free_clients(void) {
    if (user_list == NULL) {
        return;
    }

    pthread_mutex_lock(&clients_mutex);

    while (user_list != NULL) {
        t_list *current = user_list->next;

        if (((t_client *)(user_list->data))->client_socket) {
            shutdown(((t_client *)(user_list->data))->client_socket, SHUT_RDWR);
            close(((t_client *)(user_list->data))->client_socket);
        }

        if (((t_client *)(user_list->data))->ssl) {
            SSL_shutdown(((t_client *)(user_list->data))->ssl);
            SSL_free(((t_client *)(user_list->data))->ssl);
        }

        free(user_list);
        user_list = current;
    }

    user_list = NULL;
    pthread_mutex_unlock(&clients_mutex);
}

SSL_CTX *create_context(void) {
    const SSL_METHOD *method;
    method = TLS_server_method();
    SSL_CTX *context;
    context = SSL_CTX_new(method); // todo NULL for testing log_ssl_err_to_file()

    if (!context) {
        log_ssl_err_to_file("Unable to create SSL context");
    }

    return context;
}

bool configure_context(SSL_CTX *context) {
    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(context, CERTIFICATE, SSL_FILETYPE_PEM) <= 0) {
        log_ssl_err_to_file("Couldn't load the certificate into the SSL_CTX");
        return false;
    }

    if (SSL_CTX_use_PrivateKey_file(context, PRIVATE_KEY, SSL_FILETYPE_PEM) <= 0 ) {
        log_ssl_err_to_file("Couldn't load the private key into the SSL_CTX");
        return false;
    }

    if (SSL_CTX_check_private_key(context) != 1) {
        log_ssl_err_to_file("The private key has no consistency with the certificate");
        return false;
    }

    return true;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("usage: %s [port]\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    create_deamon();
    int server_socket = create_socket();
    bind_socket(server_socket, argv[1]);
    listen_socket(server_socket);

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    SSL_CTX *context = create_context();
    SSL *ssl;

    if (!context || !configure_context(context)) {
        SSL_CTX_free(context);
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    pthread_mutex_init(&clients_mutex, NULL);

    while (1) {
        int client_socket = accept(server_socket, NULL, NULL);

        if (client_socket < 0) {
            log_to_file("Couldn't establish connection with client");
            break;
        }

        t_client *new_client = (t_client *) malloc(sizeof(t_client));
        new_client->client_socket = client_socket;
        mx_push_back(&user_list, new_client);

        ssl = SSL_new(context);

        if (!ssl) {
            log_ssl_err_to_file("Creation of a new SSL structure failed");
            break;
        }

        if (SSL_set_fd(ssl, client_socket)) {
            log_ssl_err_to_file("Unable to set file descriptor as input/output device for TLS/SSL side");
            break;
        }

        if (SSL_accept(ssl) <= 0) {
            log_ssl_err_to_file("The TLS/SSL handshake was not successful");
            break;
        }

        new_client->ssl = ssl;
        pthread_t thread;

        if (pthread_create(&thread, NULL, handle_client, new_client) != 0) {
            log_to_file("Failed to create a thread");
            break;
        }

        pthread_detach(thread);
    }

    SSL_CTX_free(context);
    free_clients();
    close(server_socket);
    exit(EXIT_FAILURE);
}
