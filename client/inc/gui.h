#pragma once

#include <gtk/gtk.h>
#include <glib.h>

// Define files from resources dir
#define REG_LOGIN_GLADE "client/resources/reg_login.glade"
#define CHAT_GLADE "client/resources/chat.glade"
#define ICON_MESSAGE_PNG "client/resources/icon_message.png"
#define STYLES_CSS "client/resources/style.css"
#define STYLES_1_CSS "client/resources/style1.css"


//Gtk
void on_window_destroy(void); /*GtkWidget *widget, gpointer data*/
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

enum {
    CHAT_ID_COLUMN,
    CHAT_CHATNAME_COLUMN,
    CHAT_SENDER_COLUMN,
    CHAT_RECEIVER_COLUMN,
    CHAT_CHATS_COLUMN,
    N_CHATS_COLUMNS
};

enum {
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


void gtk_inital_function(void);

