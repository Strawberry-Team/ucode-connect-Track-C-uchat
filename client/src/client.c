// COMPILE && RUN:
// DEPRECATED --- cd client/src/ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic -c *.c -o client.o -I ../inc -I ../../libraries/libmx/inc && cd ../../ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic client/src/*.o -I client/inc -I libraries/libmx/inc -L libraries/libmx -lmx -o uchat && ./uchat 127.0.0.1 8090
// FOR LINUX & MAC OS
// cd client/obj/ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic -c ../src/*.c ../src/api/*.c ../src/gui/*.c -I ../inc -I ../../libraries/libmx/inc -I /opt/homebrew/include -I /opt/homebrew/opt/ -I /usr/bin/ $(pkg-config --cflags gtk+-3.0) && cd ../../ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic client/obj/*.o -I client/inc -I libraries/libmx/inc -L libraries/libmx -lmx -L /opt/homebrew/lib -lssl -lcrypto -lcjson -L /opt/homebrew/opt/sqlite/lib -lsqlite3 -o uchat $(pkg-config --libs gtk+-3.0) && ./uchat 127.0.0.1 8090

#include "client.h"
#include "api.h"

t_server *server_info;
t_client *client_info;
GAsyncQueue *data_queue;

void log_to_file(char *message, t_log_type log_type) {
    FILE *log_file = fopen(LOG_FILE, "a");
    time_t current_time;
    struct tm *time_info;
    char time_string[80];
    time(&current_time);
    time_info = localtime(&current_time);
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
        default:
            break;
    }

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
    if (client_info->client_socket) {
        shutdown(client_info->client_socket, SHUT_RDWR);
        close(client_info->client_socket);
    }

    if (client_info->ssl) {
        SSL_shutdown(client_info->ssl);
        SSL_free(client_info->ssl);
    }

    if (client_info->context) {
        SSL_CTX_free(client_info->context);
    }

    free(client_info);
    free(server_info);
    exit(EXIT_FAILURE);
}

gpointer controller_thread(gpointer data) {
    controller();
    (void)data;
    return NULL;
}

gboolean check_and_process_data(void) {
    char *json_string = (char *)g_async_queue_try_pop(data_queue);

    if (json_string != NULL) {
        g_idle_add_full(G_PRIORITY_HIGH_IDLE, process_data_from_controller, json_string, NULL);
    }

    return TRUE;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("usage: %s [ip] [port]\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    server_info = (t_server *) malloc(sizeof(t_server));
    server_info->address.sin_family = AF_INET;
    server_info->address.sin_port = htons(atoi(argv[2]));

    if (inet_pton(AF_INET, argv[1], &(server_info->address.sin_addr)) <= 0) {
        perror("Could not convert the network address");
        log_to_file("Could not convert the network address", ERROR);
        exit(EXIT_FAILURE);
    }

    client_info = (t_client *) malloc(sizeof(t_client));
    client_info->client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_info->client_socket < 0) {
        perror("Socket creation failed");
        log_to_file("Socket creation failed", ERROR);
        free_and_exit();
    }

    if (connect(client_info->client_socket,(struct sockaddr *)&(server_info->address), sizeof(server_info->address)) != 0) {
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

    client_info->context = context;
    ssl = SSL_new(context);

    if (!ssl) {
        perror("Creation of a new SSL structure failed");
        log_to_file("Creation of a new SSL structure failed", ERROR);
        free_and_exit();
    }

    client_info->ssl = ssl;

    if (!SSL_set_fd(client_info->ssl, client_info->client_socket)) {
        perror("Unable to set file descriptor as input/output device for TLS/SSL side");
        log_to_file("Unable to set file descriptor as input/output device for TLS/SSL side", ERROR);
        free_and_exit();
    }

    if (SSL_connect(client_info->ssl) != 1) {
        perror("The TLS/SSL handshake was not successful");
        log_to_file("The TLS/SSL handshake was not successful", ERROR);
        free_and_exit();
    }

    gtk_init(&argc, &argv);
    data_queue = g_async_queue_new();
    GThread *thread;
    thread = g_thread_new("worker_thread", controller_thread, NULL);
    g_timeout_add(100, (GSourceFunc)check_and_process_data, NULL);
    gtk_initialisation();
    g_async_queue_unref(data_queue);
    g_thread_join(thread);
    return 0;
}

