#include "server.h"
//  COMPILE:
// clang -std=c11 -Wall -Wextra -Werror -Wpedantic src/*.c -o server -I inc
// cd server/src/ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic -c *.c -o server.o -I ../inc -I ../../libraries/libmx/inc && cd ../../ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic server/src/*.o -I server/inc -I libraries/libmx/inc -L libraries/libmx -lmx -o uchat_server && ./uchat_server 8090
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

void create_deamon(void) {
    pid_t pid = fork();
    pid_t sid = 0;

    if (pid < 0) {
        char *str = "Failed to create child process";
        log_to_file(str, len(str));
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
        close(((t_client *)(user_list->data))->client_socket);
        free(user_list);
        user_list = current;
    }

    user_list = NULL;
    pthread_mutex_unlock(&clients_mutex);
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
        pthread_t thread;

        if (pthread_create(&thread, NULL, handle_client, new_client) != 0) {
            log_to_file("Failed to create a thread");
            break;
        }

        pthread_detach(thread);
    }

    free_clients();
    close(server_socket);
    exit(EXIT_FAILURE);
}
