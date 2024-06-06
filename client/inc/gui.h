#pragma once

#include <glib.h>
#include <gtk/gtk.h>

#define CHAT_GLADE "client/resources/gui/chat.glade"
#define ICON_MESSAGE_PNG "client/resources/gui/icon.png"
#define RECONNECTION_GIF "client/resources/gui/reconnection.gif"
#define REG_LOGIN_GLADE "client/resources/gui/reg_login.glade"
#define STYLES_1_CSS "client/resources/gui/style_day.css"
#define STYLES_CSS "client/resources/gui/style_night.css"


enum {
    CHAT_ID_COLUMN,
    CHAT_CHATNAME_COLUMN,
    N_CHATS_COLUMNS
};

enum {
    MESSAGE_TIME_COLUMN,
    ICON_COLUMN,
    MESSAGE_SENDER_COLUMN,
    MESSAGE_TEXT_COLUMN,
    MESSAGE_EDITED_COLUMN,
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
extern bool being_edited;
extern bool first_load;
extern bool is_clear;
extern char message_buffer[1024];
extern char message_old[1024];
extern char selected_message_sender[40];
extern char selected_message_text[1024];
extern guint timer_id;
extern int selected_chat;
extern int selected_message_id;
extern int selected_username;
extern GAsyncQueue *data_queue;
extern GtkBuilder *builder_chat;
extern GtkBuilder *builder;
extern GtkCellRenderer *renderer_time;
extern GtkListStore *chats_store;
extern GtkListStore *messages_store;
extern GtkListStore *users_store;
extern GtkListStore *users_store;
extern GtkScrolledWindow *messages_scroller;
extern GtkSearchEntry *search_entry_username;
extern GtkSwitch *change_theme;
extern GtkTreeSelection *chats_selection;
extern GtkTreeSelection *messages_selection;
extern GtkTreeSelection *users_selection;
extern GtkTreeView *tree_view_chatbar;
extern GtkTreeView *tree_view_messegabar;
extern GtkTreeView *users;
extern GtkWidget *add_chat_button;
extern GtkWidget *add_chat_window;
extern GtkWidget *change_button_on_passw_change_window;
extern GtkWidget *change_passw_button;
extern GtkWidget *chat_user_username;
extern GtkWidget *chat_username;
extern GtkWidget *create_button;
extern GtkWidget *entry_message;
extern GtkWidget *error_label;
extern GtkWidget *failed_passw_label_sign_up;
extern GtkWidget *failed_username_label_sign_up;
extern GtkWidget *failed_username_passw_label_sign_up;
extern GtkWidget *go_back_button_addchat_window;
extern GtkWidget *go_back_button_chng_window;
extern GtkWidget *inform_label_sign_up;
extern GtkWidget *inform_label;
extern GtkWidget *log_out_button;
extern GtkWidget *menu_for_ed_del;
extern GtkWidget *our_chat;
extern GtkWidget *password_entry_signin;
extern GtkWidget *password_entry_signup_confirm;
extern GtkWidget *password_entry_signup;
extern GtkWidget *profile_button;
extern GtkWidget *profile_go_back;
extern GtkWidget *profile_page;
extern GtkWidget *profile_username;
extern GtkWidget *reconection_label;
extern GtkWidget *send_message_button;
extern GtkWidget *show_passw_button;
extern GtkWidget *sign_in_button;
extern GtkWidget *sign_in_mini_button;
extern GtkWidget *sign_in_window;
extern GtkWidget *sign_up_button;
extern GtkWidget *sign_up_mini_button;
extern GtkWidget *username_entry_signin;
extern GtkWidget *username_entry_signup;


gboolean chats_store_update(gpointer *data);
gboolean check_username(const char *username);
gboolean filter_func(GtkTreeModel *model, GtkTreeIter *iter, gpointer data);
gboolean getChangeDataPasswords(void);
gboolean getSignInData(void);
gboolean getSignUpDataPasswords(void);
gboolean getSignUpDataUsername(void);
gboolean massage_store_update(gpointer data);
gboolean on_enter_press(GdkEventKey *event, gpointer user_data);
gboolean onAddChatWindowClose(GtkWidget *widget);
gboolean onChangePasswordWindowClose(GtkWidget *change_password_window);
gboolean onProfilePageClose(GtkWidget *widget);
gboolean send_add_chat_request_idle(gpointer data);
gboolean send_get_users_for_search_request_idle(gpointer data);
gboolean tree_view_messegabar_button_press_event(GtkWidget *treeview, GdkEventButton *event, gpointer data);
GdkPixbuf *load_pixbuf_from_file(const gchar *file_path);
void create_message_side(void);
void day_theme(GtkWidget *button, gboolean state);
void delete_messages(void);
void edit_messages(void);
void getMessage(void);
void gtk_initialisation(void);
void messages_entry_changed(GtkEntry *entry_message);
void messages_selection_changed(GtkWidget *messages_selection);
void on_chats_selection_changed(GtkTreeSelection *s);
void on_users_selection_changed(GtkWidget *s);
void on_window_destroy(void);
void onAddChatButtonClicked(void);
void onAddChatGoBackButtonClicked(void);
void onBtnSignInMiniClicked(GtkButton *sign_in_mini_button);
void onBtnSignUpMiniClicked(GtkButton *sign_up_mini_button);
void onChangeButtonOnPasswButtonClicked(void);
void onChangePasswButtonClicked(void);
void onChangePasswGoBackButtonClicked(GtkButton *go_back_button_chng_window);
void onCreateChatButtonClicked(void);
void onLogOutButtonClicked(void);
void onProfileButtonClicked(void);
void onProfileGoBackButtonClicked(void);
void onShowPasswButtonClicked(GtkToggleButton *show_passw_button);
void onSignInClicked(void);
void onSignUpClicked(void);
void recon_closed(void);
void show_reconnect(void);
void show_user_icon(void);
void signUpClose(void);
void update_model(int selected_message_id, const char *message_buffer, bool deleted);
void users_entry_search(GtkSearchEntry *search_entry_username);

