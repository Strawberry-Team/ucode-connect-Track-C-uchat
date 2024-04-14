#pragma once

#include "libmx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <openssl/ssl.h> // -I /opt/homebrew/include
#include <openssl/err.h> // -I /opt/homebrew/include
#include <cjson/cJSON.h> // -I /opt/homebrew/include
#include <gtk/gtk.h>
#include <glib.h>

#define BUF_SIZE 100000
#define REQUEST_TYPE_COUNT 15
#define STATUS_TYPE_COUNT 10
#define LOG_FILE "client/client.log"

typedef enum e_log_type {
    INFO,
    ERROR
} t_log_type;

typedef struct s_server {
    struct sockaddr_in address;
//    bool is_run;
//    bool connected;
//    bool loaded;
} t_server;

typedef struct s_client {
    int client_socket;
    SSL *ssl;
    SSL_CTX *context;
    int id;
    char *username;
    char *password;
    int icon_id;
} t_client;

// enumeration of error types
typedef enum e_status_type {
    SUCCESS,
    SUCCESS_VALID_CREDENTIALS,
    SUCCESS_CHAT_EXIST,
    SUCCESS_USER_IN_CHAT,
    ERROR_JSON_FAILED,
    ERROR_INVALID_CREDENTIALS,
    ERROR_CHAT_NONEXIST,
    ERROR_USER_NOT_IN_CHAT,
    ERROR_USER_DONT_HAVE_PERMISSION,
    UNKNOWN_STATUS = -1
} t_status_type;

typedef enum e_request_type { // todo підтримувати акутальним значення REQUEST_TYPE_COUNT
    LOGIN, // -> login -> password
    REGISTER, // -> login -> password
    CREATE_CHAT, // -> chat_name -> user_id
    ADD_MEMBER_TO_CHAT, // -> chat_id -> member_login
    GET_USER_CHATS, // -> user_id
    SEND_MESSAGE, // -> user_id -> chat_id -> text_message
    GET_MESSAGES_IN_CHAT,
    SEND_MESSAGE_AND_GET_MESSAGE_UPDATES,
    DELETE_MESSAGE_AND_GET_MESSAGE_UPDATES,
    CHANGE_MESSAGE_AND_GET_MESSAGE_UPDATES,
    REPLY_TO_MESSAGE_AND_GET_MESSAGE_UPDATES,
    GET_MESSAGE_UPDATES,
    GET_CHAT_MEMBERS,
    LOGOUT,
    UNKNOWN_REQUEST = -1
} t_request_type;


typedef struct s_user_data {
    //t_request_type request_type;
    char *username;
    char *password; // якщо "unsigned char *password", то для SHA-256 хеша розмір поля буде 32 байти.
} t_user_data;

extern t_server *server;
extern t_client *client;

// functions from "client.c" file
void log_to_file(char *message, t_log_type log_type);
void log_ssl_err_to_file(char *message);
SSL_CTX *create_context(void);
void free_and_exit(void);


// functions from "send_request_to_server.c" file
void send_login_req_to_server(SSL *ssl, t_request_type request_type, t_user_data *user_data);
bool handle_login_response(char *json_string);
void reconnect_to_server(void);
char *read_client_socket(void);
void process_server_response(t_request_type request_type, char *json_string);
t_request_type parse_request_type(char *json_string);
void controller(void);


void send_registration_req_to_server(SSL *ssl, t_request_type request_type, t_user_data *user_data);


//Gtk
void on_window_destroy(/*GtkWidget *widget, gpointer data*/);
extern GtkBuilder *builder;
extern GtkBuilder *builder_chat;

extern GtkWidget *username_entry_signin;
extern GtkWidget *password_entry_signin;
extern GtkWidget *sign_up_mini_button;
extern GtkWidget *sign_in_mini_button;
extern GtkWidget *sign_in_button;
extern GtkWidget *sign_up_button;
extern GtkWidget *failed_passw_label_sign_up;
extern GtkWidget *failed_username_label_sign_up;
extern GtkWidget *failed_username_passw_label_sign_up;
extern GtkWidget *show_passw_button;
extern GtkWidget *sign_in_window;
extern GtkWidget *inform_label_sign_up;

extern GtkWidget *profile_button;
extern GtkWidget *change_passw_button;
extern GtkWidget *change_button_on_passw_change_window;
extern GtkWidget *inform_label;
extern GtkWidget *chat_username;
extern GtkWidget *error_label;
extern GtkWidget *profile_go_back;
extern GtkWidget *go_back_button_chng_window;
extern GtkWidget *add_chat_button;
extern GtkWidget *add_chat_window;
extern GtkWidget *go_back_button_addchat_window;
extern GtkWidget *create_button;
extern GtkWidget *send_message_button;
extern GtkWidget *our_chat;
extern GtkSearchEntry *search_entry_username;
extern GtkWidget *entry_message;

extern GtkTreeSelection *users_selection;
extern GtkListStore *users_store;
extern GtkTreeView *users;


extern GtkTreeView *tree_view_messegabar;
extern GtkListStore *messages_store;
extern GtkScrolledWindow *messages_scroller;
extern GtkTreeSelection *messages_selection;


extern GtkWidget *context_menu;
extern GtkSwitch *change_theme;
extern bool being_edited;


extern GtkTreeView *tree_view_chatbar;
extern GtkListStore *chats_store;
extern GtkTreeSelection *chats_selection;
extern char message_buffer[1024];
extern char message_old[1024];
extern char selected_message_text[1024];
extern char selected_message_sender[40];
extern int selected_message_id;
extern char selected_username[40];
extern int selected_chat_id;
extern char selected_chat[40];
enum
{
    CHAT_ID_COLUMN,
    CHAT_CHATNAME_COLUMN,
    CHAT_SENDER_COLUMN,
    CHAT_RECEIVER_COLUMN,
    CHAT_CHATS_COLUMN,
    N_CHATS_COLUMNS
};

enum
{
    MESSAGE_TIME_COLUMN,
    ICON_COLUMN,
    MESSAGE_SENDER_COLUMN,
    MESSAGE_TEXT_COLUMN,
    MESSAGE_ID_COLUMN,
    N_MESSAGES_COLUMNS
};

enum {
    USER_ID,
    USER_USERNAME,
    N_COLUMNS
};

typedef struct s_user {
    char* username;
    char* password;
    int id;
    char *icon_id;
} t_user;

extern t_user current_user;

