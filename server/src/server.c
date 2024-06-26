// COMPILE && RUN:
// DEPRECATED --- clang -std=c11 -Wall -Wextra -Werror -Wpedantic src/*.c -o server -I inc
// DEPRECATED --- cd server/src/ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic -c *.c -o server.o -I ../inc -I ../../libraries/libmx/inc -I /opt/homebrew/include -I /usr/bin/ && cd ../../ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic server/src/*.o -I server/inc -I libraries/libmx/inc -L libraries/libmx -lmx -L /opt/homebrew/lib -lssl -lcrypto -o uchat_server && ./uchat_server 8090
// kill PID
// DEPRECATED --- cd server/obj && clang -std=c11 -Wall -Wextra -Werror -Wpedantic -c ../src/*.c -I ../inc/ -I ../../libraries/libmx/inc -I /opt/homebrew/include -I /usr/bin/ -I /opt/homebrew/opt/sqlite/include && clang -std=c11 -Wall -Wextra -Werror -Wpedantic -c ../database/*.c -I ../inc/ -I ../../libraries/libmx/inc -I /opt/homebrew/include -I /usr/bin/ -I /opt/homebrew/opt/sqlite/include && cd ../../ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic server/obj/*.o -I server/inc -I libraries/libmx/inc -L libraries/libmx -lmx -L /opt/homebrew/lib -lssl -lcrypto -lcjson -L /opt/homebrew/Cellar/sqlite/3.45.2/lib/ -lsqlite3 -o uchat_server && ./uchat_server 8090
// FOR MAC OS
// cd server/obj && clang -std=c11 -Wall -Wextra -Werror -Wpedantic -c ../src/*.c ../database/*.c ../api/*.c -I ../inc/ -I ../../libraries/libmx/inc -I /opt/homebrew/include -I /opt/homebrew/opt/ -I /usr/bin/ && cd ../../ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic server/obj/*.o -I server/inc -I libraries/libmx/inc -L libraries/libmx -lmx -L /opt/homebrew/lib -lssl -lcrypto -lcjson -L /opt/homebrew/opt/sqlite/lib -lsqlite3 -o uchat_server && ./uchat_server 8090
// FOR LINUX
// cd server/obj && clang -std=c11 -Wall -Wextra -Werror -Wpedantic -c ../src/*.c ../src/database/*.c ../src/api/*.c -I ../inc/ -I ../../libraries/libmx/inc -I /opt/homebrew/include -I /opt/homebrew/opt/ -I /usr/bin/ && cd ../../ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic server/obj/*.o -I server/inc -I libraries/libmx/inc -L libraries/libmx -lmx -L /opt/homebrew/lib -lssl -lcrypto -lcjson -L /opt/homebrew/opt/sqlite/lib -lsqlite3 -o uchat_server && ./uchat_server 8090

#include "server.h"
#include "api.h"

pthread_mutex_t logging_mutex;
t_list *user_list;

void log_to_file(char *message, t_log_type log_type) {
    pthread_mutex_lock(&logging_mutex);
    FILE *log_file = fopen(LOG_FILE, "a");

    time_t current_time;
    time(&current_time);
    struct tm *time_info;
    time_info = localtime(&current_time);
    char time_string[50];
    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", time_info);

    switch (log_type) {
        case INFO:
            fprintf(log_file, "[%s]\tINFO\t\tPID %d\t%s\n", time_string, getpid(), message);
            break;
        case ERROR:
            fprintf(log_file, "[%s]\tERROR\t\tPID %d\t%s: %s\n", time_string, getpid(), message, strerror(errno));
            break;
        case JSON_ERROR:
            fprintf(log_file, "[%s]\tJSON_ERROR\tPID %d\t%s: %s\n", time_string, getpid(), message, strerror(errno));
            break;
        case SSL_ERROR:
            fprintf(log_file, "[%s]\tSSL_ERROR\tPID %d\t%s: %s\n", time_string, getpid(), message, strerror(errno));
            ERR_print_errors_fp(log_file);
            break;
        case DB_ERROR:
            fprintf(log_file, "[%s]\tDB_ERROR\tPID %d\t%s: %s\n", time_string, getpid(), message, strerror(errno));
            break;
        default:
            break;
    }

    fclose(log_file);
    pthread_mutex_unlock(&logging_mutex);
}

void create_deamon(void) {
    pid_t pid = fork();
    pid_t sid = 0;

    if (pid < 0) {
        log_to_file("Failed to create child process", ERROR);
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        printf("Deamon started with pid %d\n", pid);
        char *msg = (char *) malloc(100 * sizeof(char));
        sprintf(msg, "Deamon started with pid %d", pid);
        log_to_file(msg, INFO);
        free(msg);
        exit(EXIT_SUCCESS);
    }

    umask(0);
    sid = setsid();

    if (sid < 0) {
        log_to_file("Failed to create session", ERROR);
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int create_socket(void) {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0) {
        log_to_file("Socket creation failed", ERROR);
        exit(EXIT_FAILURE);
    }

    return server_socket;
}

void bind_socket(int server_socket, char *port) {
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(port));
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(struct sockaddr)) < 0) {
        log_to_file("Couldn't bind socket", ERROR);
        close(server_socket);
        exit(EXIT_FAILURE);
    }
}

void listen_socket(int server_socket) {
    if (listen(server_socket, BACKLOG) < 0) {
        log_to_file("Couldn't listen for connections", ERROR);
        close(server_socket);
        exit(EXIT_FAILURE);
    }
}

void free_clients(void) {
    if (user_list == NULL) {
        return;
    }

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
}

SSL_CTX *create_context(void) {
    const SSL_METHOD *method;
    method = TLS_server_method();
    SSL_CTX *context;
    context = SSL_CTX_new(method);

    if (!context) {
        log_to_file("Unable to create SSL context", SSL_ERROR);
    }

    return context;
}

bool configure_context(SSL_CTX *context) {
    if (SSL_CTX_use_certificate_file(context, SSL_CERTIFICATE, SSL_FILETYPE_PEM) <= 0) {
        log_to_file("Couldn't load the certificate into the SSL_CTX", SSL_ERROR);
        return false;
    }

    if (SSL_CTX_use_PrivateKey_file(context, SSL_PRIVATE_KEY, SSL_FILETYPE_PEM) <= 0 ) {
        log_to_file("Couldn't load the private key into the SSL_CTX", SSL_ERROR);
        return false;
    }

    if (SSL_CTX_check_private_key(context) != 1) {
        log_to_file("The private key has no consistency with the certificate", SSL_ERROR);
        return false;
    }

    return true;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("usage: %s [port]\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    if (pthread_mutex_init(&logging_mutex, NULL) != 0) {
        log_to_file("Mutex initialization failed", ERROR);
        exit(EXIT_FAILURE);
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

    if (!context
        || !configure_context(context)) {
        SSL_CTX_free(context);
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    while (true) {
        int client_socket = accept(server_socket, NULL, NULL);

        if (client_socket < 0) {
            log_to_file("Couldn't establish connection with client", ERROR);
            break;
        }

        t_client *client_info = (t_client *) malloc(sizeof(t_client));
        client_info->client_socket = client_socket;
        mx_push_back(&user_list, client_info);

        ssl = SSL_new(context);

        if (!ssl) {
            log_to_file("Creation of a new SSL structure failed", SSL_ERROR);
            break;
        }

        client_info->ssl = ssl;

        if (!SSL_set_fd(client_info->ssl, client_socket)) {
            log_to_file("Unable to set file descriptor as input/output device for TLS/SSL side", SSL_ERROR);
            break;
        }

        if (SSL_accept(client_info->ssl) != 1) {
            log_to_file("The TLS/SSL handshake was not successful", SSL_ERROR);
            break;
        }

        pthread_t thread;
        pthread_attr_t thread_attr;
        pthread_attr_init(&thread_attr);

        if (pthread_create(&thread, &thread_attr, thread_controller, client_info) != 0) {
            log_to_file("Failed to create a thread", ERROR);
            break;
        }

        pthread_detach(thread);
    }

    SSL_CTX_free(context);
    free_clients();
    close(server_socket);
    exit(EXIT_FAILURE);
}

