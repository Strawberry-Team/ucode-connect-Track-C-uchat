#include "client.h"
#include "api.h"

typedef struct {
    t_user_data *user_data;
    int selected_chat;
} CallbackData;

t_user_data *user_data = NULL;

bool being_edited;
bool filter_applied = false;
bool is_clear = false;
char message_buffer[1024];
char message_old[1024];
char selected_message_sender[40];
char selected_message_text[1024];
int selected_chat;
int selected_message_id;
int selected_username;

const gchar *password_to_change;
guint timer = 0;
guint timer_id;

GtkBuilder *builder_chat;
GtkBuilder *builder;
GtkCellRenderer *renderer_time;
GtkListStore *chats_store;
GtkListStore *messages_store;
GtkListStore *users_store;
GtkScrolledWindow *messages_scroller;
GtkSearchEntry *search_entry_username;
GtkSwitch *change_theme;
GtkTreeSelection *chats_selection;
GtkTreeSelection *messages_selection;
GtkTreeSelection *users_selection;
GtkTreeView *tree_view_chatbar;
GtkTreeView *tree_view_messegabar;
GtkTreeView *users;
GtkWidget *add_chat_button;
GtkWidget *add_chat_window;
GtkWidget *change_button_on_passw_change_window;
GtkWidget *change_passw_button;
GtkWidget *chat_user_username;
GtkWidget *chat_username;
GtkWidget *create_button;
GtkWidget *entry_message;
GtkWidget *error_label;
GtkWidget *failed_passw_label_sign_up;
GtkWidget *failed_username_label_sign_up;
GtkWidget *failed_username_passw_label_sign_up;
GtkWidget *go_back_button_addchat_window;
GtkWidget *go_back_button_chng_window;
GtkWidget *home_page;
GtkWidget *inform_label_sign_up;
GtkWidget *inform_label;
GtkWidget *log_out_button;
GtkWidget *menu_for_ed_del;
GtkWidget *our_chat;
GtkWidget *password_entry_signin;
GtkWidget *password_entry_signup_confirm;
GtkWidget *password_entry_signup;
GtkWidget *profile_button;
GtkWidget *profile_go_back;
GtkWidget *profile_page;
GtkWidget *profile_username;
GtkWidget *reconection_label;
GtkWidget *reconnection_gif;
GtkWidget *send_message_button;
GtkWidget *show_passw_button;
GtkWidget *sign_in_button;
GtkWidget *sign_in_mini_button;
GtkWidget *sign_in_window;
GtkWidget *sign_up_button;
GtkWidget *sign_up_mini_button;
GtkWidget *user_chat_icon;
GtkWidget *username_entry_signin;
GtkWidget *username_entry_signup;

void on_window_destroy(void) {
    gtk_main_quit();
}

gboolean check_username(const char *username) {
    while (*username) {
        if ((*username < '0'
            || *username > '9')
            && (*username < 'a'
            || *username > 'z')
            && *username != '-'
            && *username != '_') {
            return FALSE;
        }

        username++;
    }

    return TRUE;
}

gboolean getSignInData(void) {
    const char *username = gtk_entry_get_text(GTK_ENTRY(username_entry_signin));
    const char *password = gtk_entry_get_text(GTK_ENTRY(password_entry_signin));

    if (!check_username(username)
        || strlen(username) < 5){
        return FALSE;
    } else {
        user_data = (t_user_data *) malloc(sizeof(t_user_data));
        user_data->username = mx_strdup(username);
        user_data->password = mx_strdup(password);
        return TRUE;
    }
}

gboolean getSignUpDataUsername(void) {
    GtkWidget *username_entry_signup = GTK_WIDGET(gtk_builder_get_object(builder, "username_entry_signup"));
    GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(username_entry_signup));
    gchar *username = mx_strdup(gtk_entry_buffer_get_text(buffer));

    if (!check_username(username)
        || strlen(username) < 6){
        return FALSE;
    } else {
        user_data->username = username;
        return TRUE;
    }
}

gboolean getSignUpDataPasswords(void) {
    GtkWidget *password_entry_signup = GTK_WIDGET(gtk_builder_get_object(builder, "password_entry_signup"));
    GtkWidget *password_entry_signup_confirm = GTK_WIDGET(gtk_builder_get_object(builder, "password_entry_signup_confirm"));

    GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(password_entry_signup));
    gchar *password = mx_strdup(gtk_entry_buffer_get_text(buffer));

    GtkEntryBuffer *buffer1 = gtk_entry_get_buffer(GTK_ENTRY(password_entry_signup_confirm));
    gchar *password_confirm = mx_strdup(gtk_entry_buffer_get_text(buffer1));

    if (strlen(password) == 0
        || strlen(password_confirm) == 0
        || strlen(password) < 6
        || strcmp(password, password_confirm) != 0
        || strchr(password, ' ') != NULL) {
        return FALSE;
    }

    user_data->password = g_strdup(password_confirm);
    return TRUE;
}

void onBtnSignUpMiniClicked(GtkButton *sign_up_mini_button) {
    GtkWidget *sign_in_window = gtk_widget_get_toplevel(GTK_WIDGET(sign_up_mini_button));
    gboolean is_registration_window_open = FALSE;
    gtk_widget_hide(sign_in_window);

    if (is_registration_window_open)
        return;
    if (!builder) {
        builder = gtk_builder_new();
        gtk_builder_add_from_file(builder, REG_LOGIN_GLADE, NULL);
    }

    GtkWidget *sign_up_window = GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_window"));
    g_signal_connect(sign_up_window , "destroy", G_CALLBACK(on_window_destroy), NULL);
    gtk_widget_show_all(sign_up_window);
    gtk_widget_set_opacity(error_label, 0.0);
    gtk_entry_set_text(GTK_ENTRY(username_entry_signin), "");
    gtk_entry_set_text(GTK_ENTRY(password_entry_signin), "");
    is_registration_window_open = TRUE;
}

void signUpClose(void) {
    GtkWidget *sign_in_window = GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_window"));
    gtk_widget_hide(sign_in_window);
    gtk_entry_set_text(GTK_ENTRY(username_entry_signup), "");
    gtk_entry_set_text(GTK_ENTRY(password_entry_signup), "");
    gtk_entry_set_text(GTK_ENTRY(password_entry_signup_confirm), "");
}

void onBtnSignInMiniClicked(GtkButton *sign_in_mini_button) {
    GtkWidget *sign_up_window = gtk_widget_get_toplevel(GTK_WIDGET(sign_in_mini_button));
    gboolean is_login_window_open = FALSE;
    gtk_widget_hide(sign_up_window);

    if (is_login_window_open)
        return;

    if (!builder) {
        builder = gtk_builder_new();
        gtk_builder_add_from_file(builder, REG_LOGIN_GLADE, NULL);
    }

    signUpClose();
    GtkWidget *sign_in_window = GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_window"));
    g_signal_connect(sign_in_window , "destroy", G_CALLBACK(on_window_destroy), NULL);
    gtk_widget_show_all(sign_in_window);
    gtk_widget_set_opacity(inform_label_sign_up, 0.0);
    is_login_window_open = TRUE;
}

void update_model(int selected_message_id, const char *message_buffer, bool deleted) {
    GtkTreeIter iter;
    gboolean found = FALSE;
    found = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(messages_store), &iter);

    while (found) {
        int str_message_id;
        gtk_tree_model_get(GTK_TREE_MODEL(messages_store), &iter, MESSAGE_ID_COLUMN, &str_message_id, -1);

        if (str_message_id == selected_message_id) {
            if (deleted) {
                gtk_list_store_set(messages_store, &iter, MESSAGE_TEXT_COLUMN, "{Deleted message}", -1);
                gtk_list_store_set(messages_store, &iter, MESSAGE_EDITED_COLUMN, NULL, -1);
            } else {
                gtk_list_store_set(messages_store, &iter, MESSAGE_TEXT_COLUMN, message_buffer, -1);
                gtk_list_store_set(messages_store, &iter, MESSAGE_EDITED_COLUMN, "edited", -1);
            }

            gtk_widget_queue_draw(GTK_WIDGET(tree_view_messegabar));
            return;
        }

        found = gtk_tree_model_iter_next(GTK_TREE_MODEL(messages_store), &iter);
    }
}

gboolean tree_view_messegabar_button_press_event(GtkWidget *treeview, GdkEventButton *event, gpointer data) {
    GtkTreePath *path_to;
    GtkTreeViewColumn *column;
    GtkTreeModel *model;

    if (event->button == GDK_BUTTON_SECONDARY) {
        if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), (gint)event->x, (gint)event->y, &path_to, &column, NULL, NULL)
            && gtk_tree_selection_get_selected(GTK_TREE_SELECTION(messages_selection), &model, NULL)) {
            GtkWidget *menu_data = GTK_WIDGET(data);
            GList *children = gtk_container_get_children(GTK_CONTAINER(menu_data));
            GtkWidget *edit_button = GTK_WIDGET(g_list_nth_data(children, 0));
            GtkWidget *delete_button = GTK_WIDGET(g_list_nth_data(children, 1));
            g_list_free(children);

            if (strcmp(selected_message_sender, client_info->username) != 0) {
                if (GTK_IS_MENU(menu_data))
                    gtk_menu_popup_at_pointer(GTK_MENU(menu_data), NULL);

                gtk_widget_hide(edit_button);
                gtk_widget_hide(delete_button);
            } else {
                gtk_widget_show_all(menu_data);

                if (GTK_IS_MENU(menu_data))
                    gtk_menu_popup_at_pointer(GTK_MENU(menu_data), NULL);
            }
        }
    }

    if (event->button == GDK_BUTTON_PRIMARY) {
        if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), (gint)event->x, (gint)event->y, &path_to, &column, NULL, NULL)
            && gtk_tree_selection_get_selected(GTK_TREE_SELECTION(messages_selection), &model, NULL)) {
        }
        else {
            if (!being_edited)
                gtk_tree_selection_unselect_all(messages_selection);
        }
    }

    return FALSE;
}

void edit_messages(void) {
    being_edited = true;
    strcpy(message_old, message_buffer);
    gtk_entry_set_text(GTK_ENTRY(entry_message), selected_message_text);
    gtk_tree_selection_unselect_all(messages_selection);
}

void delete_messages(void) {
    t_user_data *user_data = (t_user_data *) malloc(sizeof(t_user_data));
    t_msg_data *msg_data = (t_msg_data *) malloc(sizeof(t_msg_data));
    user_data->username = mx_strdup(client_info->username);
    user_data->password = mx_strdup(client_info->password);
    msg_data->message_id = selected_message_id;
    send_delete_message_request(client_info->ssl, user_data, msg_data);
    gtk_tree_selection_unselect_all(messages_selection);
}

void messages_selection_changed(GtkWidget *messages_selection) {
    gchar *temp;
    int temp1;
    GtkTreeIter iter;
    GtkTreeModel *model;

    if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(messages_selection), &model, &iter) == FALSE)
        return;

    gtk_tree_model_get(model, &iter, MESSAGE_TEXT_COLUMN, &temp, -1);
    strcpy(selected_message_text, temp);
    gtk_tree_model_get(model, &iter, MESSAGE_SENDER_COLUMN, &temp, -1);
    strcpy(selected_message_sender, temp);
    gtk_tree_model_get(model, &iter, MESSAGE_ID_COLUMN, &temp1, -1);

    selected_message_id = temp1;

    if (being_edited) {
        gtk_entry_set_text(GTK_ENTRY(entry_message), message_old);
        being_edited = false;
    } else
        gtk_entry_set_text(GTK_ENTRY(entry_message), message_buffer);
}

void create_message_side(void) {
    tree_view_messegabar = GTK_TREE_VIEW(gtk_builder_get_object(builder_chat, "tree_view_messegabar"));
    messages_store = gtk_list_store_new(N_MESSAGES_COLUMNS, G_TYPE_STRING, GDK_TYPE_PIXBUF,  G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);
    renderer_time = gtk_cell_renderer_text_new();
    g_object_set(renderer_time, "xalign", 0.0, NULL);
    g_object_set(renderer_time, "yalign", 0.5, NULL);
    g_object_set(renderer_time, "font", "Arial 15", NULL);

    GtkTreeViewColumn *msg_col1 = gtk_tree_view_column_new_with_attributes("Time:", renderer_time, "text", MESSAGE_TIME_COLUMN, NULL);
    gtk_tree_view_append_column(tree_view_messegabar, msg_col1);
    GtkCellRenderer *renderer = gtk_cell_renderer_pixbuf_new();

    gtk_tree_view_set_model(tree_view_messegabar, GTK_TREE_MODEL(messages_store));

    GtkCellRenderer *render_sender = gtk_cell_renderer_text_new();
    g_object_set(render_sender, "xalign", 0.0, NULL);
    g_object_set(render_sender, "yalign", 0.5, NULL);
    g_object_set(render_sender, "font", "Arial 18", NULL);

    GtkCellRenderer *render_message = gtk_cell_renderer_text_new();
    g_object_set(render_message, "xalign", 0.0, NULL);
    g_object_set(render_message, "yalign", 0.5, NULL);
    g_object_set(render_message, "wrap-mode", PANGO_WRAP_WORD_CHAR, NULL);
    g_object_set(render_message, "wrap-width", 530, NULL);
    g_object_set(render_message, "font", "Arial 18", NULL);

    GtkTreeViewColumn *msg_col_icon = gtk_tree_view_column_new_with_attributes("Icon", renderer, "pixbuf", ICON_COLUMN, NULL);
    gtk_tree_view_append_column(tree_view_messegabar, msg_col_icon);
    gtk_tree_view_column_set_visible(msg_col_icon, TRUE);
    GtkTreeViewColumn *msg_col_sender = gtk_tree_view_column_new_with_attributes("Sender:", render_sender, "text", MESSAGE_SENDER_COLUMN, NULL);
    gtk_tree_view_append_column(tree_view_messegabar, msg_col_sender);
    GtkTreeViewColumn *msg_col_message = gtk_tree_view_column_new_with_attributes("Message:", render_message, "text", MESSAGE_TEXT_COLUMN, NULL);
    gtk_tree_view_append_column(tree_view_messegabar, msg_col_message);
    gtk_tree_view_column_set_expand(msg_col_message, TRUE);
    gtk_tree_view_column_set_max_width(msg_col_message, G_MAXINT);

    GtkCellRenderer *renderer_edited = gtk_cell_renderer_text_new();
    g_object_set(renderer_edited, "font", "Arial 15", NULL);
    GtkTreeViewColumn *msg_col_edit_status = gtk_tree_view_column_new_with_attributes("Edited status", renderer_edited, "text", MESSAGE_EDITED_COLUMN, NULL);
    gtk_tree_view_append_column(tree_view_messegabar, msg_col_edit_status);
    gtk_tree_view_column_set_visible(msg_col_edit_status, TRUE);

    GtkTreeViewColumn *msg_col0 = gtk_tree_view_column_new_with_attributes("ID:", gtk_cell_renderer_text_new(), "text", MESSAGE_ID_COLUMN, NULL);
    gtk_tree_view_append_column(tree_view_messegabar, msg_col0);
    gtk_tree_view_column_set_visible(msg_col0, FALSE);

    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree_view_messegabar), FALSE);

    menu_for_ed_del = gtk_menu_new();

    GtkWidget *edit_massage = gtk_menu_item_new_with_label("Edit message");
    g_signal_connect(edit_massage, "activate", G_CALLBACK(edit_messages), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_for_ed_del), edit_massage);

    GtkWidget *delete_massage = gtk_menu_item_new_with_label("Delete message");
    g_signal_connect(delete_massage, "activate", G_CALLBACK(delete_messages), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_for_ed_del), delete_massage);

    gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(tree_view_messegabar), FALSE);
    gtk_tree_view_set_enable_tree_lines(GTK_TREE_VIEW(tree_view_messegabar), FALSE);
    gtk_tree_view_set_enable_search(GTK_TREE_VIEW(tree_view_messegabar), FALSE);
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(tree_view_messegabar), GTK_TREE_VIEW_GRID_LINES_NONE);
    gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(tree_view_messegabar), FALSE);
    g_signal_connect(tree_view_messegabar, "button-press-event", G_CALLBACK(tree_view_messegabar_button_press_event), menu_for_ed_del);

    tree_view_chatbar = GTK_TREE_VIEW(gtk_builder_get_object(builder_chat, "tree_view_chatbar"));
    chats_store = gtk_list_store_new(N_CHATS_COLUMNS, G_TYPE_INT, G_TYPE_STRING);
    gtk_tree_view_set_model(tree_view_chatbar, GTK_TREE_MODEL(chats_store));

    GtkTreeViewColumn *chat_col_id = gtk_tree_view_column_new_with_attributes("ID", gtk_cell_renderer_text_new(), "text", CHAT_ID_COLUMN, NULL);
    gtk_tree_view_append_column(tree_view_chatbar, chat_col_id);
    gtk_tree_view_column_set_visible(chat_col_id, FALSE);

    GtkCellRenderer *render_chat_column = gtk_cell_renderer_text_new();
    g_object_set(render_chat_column, "ypad", 8, NULL);
    GtkTreeViewColumn *chat_col_chatname = gtk_tree_view_column_new_with_attributes("Chatname", render_chat_column, "text", CHAT_CHATNAME_COLUMN, NULL);
    gtk_tree_view_append_column(tree_view_chatbar, chat_col_chatname);
    gtk_tree_view_column_set_visible(chat_col_chatname, TRUE);

    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree_view_chatbar), FALSE);
}

gboolean chats_store_update(gpointer *data) {
    t_user_data *user_data = (t_user_data *)data;
    send_get_chats_request(client_info->ssl, user_data);
    return TRUE;
}

void onSignInClicked(void) {
    if (!builder) {
        builder = gtk_builder_new();
        gtk_builder_add_from_file(builder, CHAT_GLADE, NULL);
    }

    if (getSignInData()) {
        send_login_request(client_info->ssl, user_data);
        client_info->username = mx_strdup(user_data->username);
        client_info->password = mx_strdup(user_data->password);
    } else {
        gtk_widget_show(error_label);
        gtk_label_set_text(GTK_LABEL(error_label), (const gchar*) "Password and username unavailable!");
        gtk_widget_set_opacity(error_label, 1.0);
    }

    gtk_widget_set_opacity(entry_message, 0.0);
    gtk_widget_set_opacity(send_message_button, 0.0);
}

void onSignUpClicked(void) {
    gtk_widget_show(inform_label_sign_up);
    user_data = (t_user_data *) malloc(sizeof(t_user_data));
    gboolean usernameValidation = getSignUpDataUsername();
    gboolean passwordsMatch = getSignUpDataPasswords();

    if (!passwordsMatch
        && !usernameValidation) {
        gtk_label_set_text(GTK_LABEL(inform_label_sign_up), (const gchar*) "Password and username unavailable!");
        gtk_widget_set_opacity(inform_label_sign_up, 1.0);
    } else if (!usernameValidation) {
        gtk_label_set_text(GTK_LABEL(inform_label_sign_up), (const gchar*) "Username validation crashed!");
        gtk_widget_set_opacity(inform_label_sign_up, 1.0);
    } else if (!passwordsMatch) {
        gtk_label_set_text(GTK_LABEL(inform_label_sign_up), (const gchar*) "Password unavailable!");
        gtk_widget_set_opacity(inform_label_sign_up, 1.0);
    } else {
        send_registration_request(client_info->ssl, user_data);
        gtk_widget_set_opacity(entry_message, 0.0);
        gtk_widget_set_opacity(send_message_button, 0.0);
    }
}

gboolean onProfilePageClose(GtkWidget *widget) {
    gtk_widget_hide(widget);
    return TRUE;
}

void onProfileButtonClicked(void) {
    if (!builder_chat) {
        builder_chat = gtk_builder_new();
        gtk_builder_add_from_file(builder_chat, CHAT_GLADE, NULL);
    }

    gtk_label_set_text(GTK_LABEL(profile_username), client_info->username);
    gtk_window_set_position(GTK_WINDOW(profile_page), GTK_WIN_POS_CENTER);
    g_signal_connect(profile_page, "delete-event", G_CALLBACK(onProfilePageClose), NULL);
    gtk_widget_show_all(profile_page);
}

gboolean onChangePasswordWindowClose(GtkWidget *change_password_window) {
    gtk_widget_hide(change_password_window);
    GtkWidget *change_button_on_passw_change_window = GTK_WIDGET(gtk_builder_get_object(builder_chat, "change_button_on_passw_change_window"));
    GtkWidget *change_current_password = GTK_WIDGET(gtk_builder_get_object(builder_chat, "change_current_password"));
    GtkWidget *change_new_password = GTK_WIDGET(gtk_builder_get_object(builder_chat, "change_new_password"));
    GtkWidget *change_confirm_new_password = GTK_WIDGET(gtk_builder_get_object(builder_chat, "change_confirm_new_password"));

    gtk_entry_set_text(GTK_ENTRY(change_current_password), "");
    gtk_entry_set_text(GTK_ENTRY(change_new_password), "");
    gtk_entry_set_text(GTK_ENTRY(change_confirm_new_password), "");

    gtk_widget_set_opacity(inform_label, 0.0);
    gtk_widget_set_sensitive(change_button_on_passw_change_window, TRUE);
    return TRUE;
}

void onChangePasswButtonClicked(void) {
    gtk_widget_hide(profile_page);

    if (!builder_chat) {
        builder_chat = gtk_builder_new();
        gtk_builder_add_from_file(builder_chat, CHAT_GLADE, NULL);
    }

    GtkWidget *change_password_window = GTK_WIDGET(gtk_builder_get_object(builder_chat, "change_password_window"));
    g_signal_connect(change_password_window, "delete-event", G_CALLBACK(onChangePasswordWindowClose), NULL);
    gtk_widget_show_all(change_password_window);
}

gboolean getChangeDataPasswords(void) {
    GtkWidget *change_new_password = GTK_WIDGET(gtk_builder_get_object(builder_chat, "change_new_password"));
    GtkWidget *change_confirm_new_password = GTK_WIDGET(gtk_builder_get_object(builder_chat, "change_confirm_new_password"));
    const gchar *password_new = gtk_entry_get_text(GTK_ENTRY(change_new_password));
    const gchar *password_new_confirm = gtk_entry_get_text(GTK_ENTRY(change_confirm_new_password));

    if (strcmp(password_new, password_new_confirm) == 0
        && strlen(password_new) > 5
        && (strcmp(password_new, "") != 0
        || strcmp(password_new_confirm, "") != 0)) {
        password_to_change = mx_strdup((const char *) password_new);
        return TRUE;
    } else {
        return FALSE;
    }
}

void onChangeButtonOnPasswButtonClicked(void) {
    if (!builder_chat) {
        builder_chat = gtk_builder_new();
        gtk_builder_add_from_file(builder_chat, CHAT_GLADE, NULL);
    }

    gtk_widget_show(inform_label);
    gboolean passwordMatch = getChangeDataPasswords();

    if (!passwordMatch) {
        gtk_label_set_text(GTK_LABEL(inform_label), (const gchar*) "Failed to change the password!");
        gtk_widget_set_opacity(inform_label, 1.0);
    } else {
        t_user_data *user_data = (t_user_data *) malloc(sizeof(t_user_data));
        user_data->username = mx_strdup((const char *) client_info->username);
        user_data->password = mx_strdup((const char *) client_info->password);
        send_update_user_request(client_info->ssl, user_data, (char *)password_to_change);
        gtk_widget_set_opacity(inform_label, 1.0);
    }
}

void onProfileGoBackButtonClicked(void) {
    gtk_widget_hide(profile_page);
}

void onChangePasswGoBackButtonClicked(GtkButton *go_back_button_chng_window) {
    GtkWidget *change_password_window = gtk_widget_get_toplevel(GTK_WIDGET(go_back_button_chng_window));
    gtk_widget_hide(change_password_window);

    if (!builder_chat) {
        builder_chat = gtk_builder_new();
        gtk_builder_add_from_file(builder_chat, CHAT_GLADE, NULL);
    }
    onChangePasswordWindowClose(change_password_window);
    gtk_label_set_text(GTK_LABEL(inform_label), (const gchar*) "Successfully changed!");
    g_signal_connect(profile_page, "destroy", G_CALLBACK(on_window_destroy), NULL);
    gtk_widget_show_all(profile_page);
}

gboolean onAddChatWindowClose(GtkWidget *widget) {
    gtk_widget_hide(widget);
    return TRUE;
}

gboolean filter_func(GtkTreeModel *model, GtkTreeIter *iter, gpointer data) {
    const gchar *search = (const gchar *)data;
    gchar *username = NULL;
    gtk_tree_model_get(model, iter, USER_USERNAME, &username, -1);

    if (username
        && *username != '\0') {
        gboolean match = g_str_has_prefix(username, search);
        g_free(username);
        return match;
    }

    g_free(username);
    return FALSE;
}

void users_entry_search(GtkSearchEntry *search_entry_username) {
    const gchar *search;
    search = gtk_entry_get_text(GTK_ENTRY(search_entry_username));
    GtkTreeModelFilter *filter = GTK_TREE_MODEL_FILTER(gtk_tree_model_filter_new(GTK_TREE_MODEL(users_store), NULL));
    gtk_tree_model_filter_set_visible_func(filter, filter_func, (gpointer) search, NULL);
    gtk_tree_view_set_model(users, GTK_TREE_MODEL(filter));
}

gboolean send_get_users_for_search_request_idle(gpointer data) {
    t_user_data *user_data = (t_user_data *)data;
    send_get_users_for_search_request(client_info->ssl, user_data);
    g_free(user_data->username);
    g_free(user_data->password);
    g_free(user_data);
    return G_SOURCE_REMOVE;
}

void onAddChatButtonClicked(void) {
    if (!builder_chat) {
        builder_chat = gtk_builder_new();
        gtk_builder_add_from_file(builder_chat, CHAT_GLADE, NULL);
    }

    if (!users_store) {
        users_store = gtk_list_store_new(N_COLUMNS, G_TYPE_INT, G_TYPE_STRING);
        users = GTK_TREE_VIEW(gtk_builder_get_object(builder_chat, "users"));
        gtk_tree_view_set_model(users, GTK_TREE_MODEL(users_store));

        GtkTreeViewColumn *user_column = gtk_tree_view_column_new_with_attributes("ID", gtk_cell_renderer_text_new(),
                                                                                  "text", USER_ID, NULL);
        gtk_tree_view_append_column(users, user_column);
        gtk_tree_view_column_set_visible(user_column, FALSE);

        GtkTreeViewColumn *user_column1 = gtk_tree_view_column_new_with_attributes("Users",
                                                                                   gtk_cell_renderer_text_new(),
                                                                                   "text", USER_USERNAME, NULL);
        gtk_tree_view_append_column(users, user_column1);
        gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(users), FALSE);
    }

    t_user_data *user_data = (t_user_data *) malloc(sizeof(t_user_data));
    user_data->username = mx_strdup(client_info->username);
    user_data->password = mx_strdup(client_info->password);
    g_idle_add_full(G_PRIORITY_HIGH_IDLE, send_get_users_for_search_request_idle, user_data, NULL);
    g_signal_connect(add_chat_window, "delete-event", G_CALLBACK(onAddChatWindowClose), NULL);
    selected_username = 0;
    gtk_widget_show_all(add_chat_window);
}

void onAddChatGoBackButtonClicked(void) {
    gtk_widget_hide(add_chat_window);
    gtk_entry_set_text(GTK_ENTRY(search_entry_username), "");
}

void messages_entry_changed(GtkEntry *entry_message) {
    sprintf(message_buffer, "%s", gtk_entry_get_text(entry_message));
}

GdkPixbuf *load_pixbuf_from_file(const gchar *file_path) {
    GError *error = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(file_path, &error);

    if (error != NULL) {
        g_error_free(error);
        return NULL;
    }

    return pixbuf;
}

void getMessage(void) {
    if (message_buffer[0] != '\0') {
        if (being_edited) {
            if (strcmp(selected_message_text, message_buffer) != 0) {
                t_user_data *user_data = (t_user_data *) malloc(sizeof(t_user_data));
                t_msg_data *msg_data = (t_msg_data *) malloc(sizeof(t_msg_data));
                user_data->username = mx_strdup(client_info->username);
                user_data->password = mx_strdup(client_info->password);
                msg_data->message_id = selected_message_id;
                msg_data->body = mx_strdup(message_buffer);
                send_update_message_request(client_info->ssl, user_data, msg_data);
            }

            gtk_entry_set_text(GTK_ENTRY(entry_message), message_old);
            being_edited = false;
        }
        else {
            if (message_buffer[0] != '\0') {
                t_msg_data *message_data = (t_msg_data *) malloc(sizeof(t_msg_data));
                t_user_data *user_data = (t_user_data *) malloc(sizeof(t_user_data));
                user_data->username = mx_strdup(client_info->username);
                user_data->password = mx_strdup(client_info->password);
                message_data->body = message_buffer;
                message_data->chat_id = selected_chat;
                send_add_message_request(client_info->ssl, user_data, message_data);
                message_buffer[0] = '\0';
            }

            gtk_editable_delete_text(GTK_EDITABLE(entry_message), 0, -1);
        }
    }
}

void onShowPasswButtonClicked(GtkToggleButton *show_passw_button) {
    GtkWidget *password_entry_signin = GTK_WIDGET(gtk_builder_get_object(builder, "password_entry_signin"));
    gtk_entry_set_visibility(GTK_ENTRY(password_entry_signin), gtk_toggle_button_get_active(show_passw_button));
}

gboolean on_enter_press(GdkEventKey *event, gpointer user_data) {
    if (event->keyval == GDK_KEY_Return
        || event->keyval == GDK_KEY_KP_Enter) {
        gtk_button_clicked(GTK_BUTTON(user_data));
        return TRUE;
    }

    return FALSE;
}

void day_theme(GtkWidget *button, gboolean state) {
    GtkCssProvider *provider = gtk_css_provider_new();
    (void)button;

    if (state) {
        gtk_css_provider_load_from_path(provider, STYLES_1_CSS, NULL);
    } else {
        gtk_css_provider_load_from_path(provider, STYLES_CSS, NULL);
    }

    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                              GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void on_users_selection_changed(GtkWidget *s) {
    GtkTreeIter iter;
    GtkTreeModel *model;

    if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(s), &model, &iter) == FALSE)
        return;

    gtk_tree_model_get(model, &iter, USER_ID, &selected_username, -1);
}

gboolean send_add_chat_request_idle(gpointer data) {
    t_user_data *user_data = (t_user_data *)data;
    send_add_chat_request(client_info->ssl, user_data, selected_username);
    gtk_widget_hide(add_chat_window);
    g_free(user_data->username);
    g_free(user_data->password);
    g_free(user_data);
    return G_SOURCE_REMOVE;
}

void onCreateChatButtonClicked(void) {
    gtk_entry_set_text(GTK_ENTRY(search_entry_username), "");
    t_user_data *user_data = (t_user_data *) malloc(sizeof(t_user_data));
    user_data->username = mx_strdup(client_info->username);
    user_data->password = mx_strdup(client_info->password);
    g_idle_add_full(G_PRIORITY_HIGH_IDLE, send_add_chat_request_idle, user_data, NULL);
}

gboolean massage_store_update(gpointer data) {
    CallbackData *callback_data = (CallbackData *)data;
    t_user_data *user_data = callback_data->user_data;
    int selected_chat = callback_data->selected_chat;
    send_get_messages_request(client_info->ssl, user_data, selected_chat);
    return TRUE;
}

void show_user_icon(void) {
    GdkPixbuf *pixbuf;
    pixbuf = gdk_pixbuf_new_from_file(ICON_MESSAGE_PNG, NULL);
    gtk_image_set_from_pixbuf(GTK_IMAGE(user_chat_icon), pixbuf);
}

void on_chats_selection_changed(GtkTreeSelection *s) {
    if (timer) {
        g_source_remove(timer);
    }

    first_load = true;
    is_clear = true;
    gchar *selected_chat_username;
    GtkTreeIter iter;
    GtkTreeModel *model;
    gtk_tree_selection_set_mode(s, GTK_SELECTION_SINGLE);

    if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(s), &model, &iter) == FALSE)
        return;

    gtk_list_store_clear(messages_store);
    gtk_tree_model_get(model, &iter, CHAT_ID_COLUMN, &selected_chat, -1);

    t_user_data *user_data = (t_user_data *) malloc(sizeof(t_user_data));
    user_data->username = mx_strdup(client_info->username);
    user_data->password = mx_strdup(client_info->password);

    CallbackData *callback_data = malloc(sizeof(CallbackData));
    callback_data->user_data = user_data;
    callback_data->selected_chat = selected_chat;

    timer = g_timeout_add(1000, (GSourceFunc)massage_store_update, callback_data);
    gtk_tree_model_get(model, &iter, CHAT_CHATNAME_COLUMN, &selected_chat_username, -1);
    gtk_label_set_text(GTK_LABEL(chat_user_username), selected_chat_username);
    gtk_widget_set_opacity(entry_message, 1.0);
    gtk_widget_set_opacity(send_message_button, 1.0);
    show_user_icon();
    gtk_widget_hide(home_page);
}

void show_reconnect(void) {
    reconnection_gif = GTK_WIDGET(gtk_builder_get_object(builder_chat, "reconnection_gif"));
    GdkPixbufAnimation *animation;
    animation = gdk_pixbuf_animation_new_from_file(RECONNECTION_GIF, NULL);
    gtk_image_set_from_animation(GTK_IMAGE(reconnection_gif), animation);
    gtk_widget_set_opacity(reconnection_gif, 1.0);
}

void recon_closed(void) {
    gtk_widget_set_opacity(reconnection_gif, 0.0);
}

void onLogOutButtonClicked(void) {
    gtk_widget_hide(our_chat);
    gtk_widget_hide(profile_page);
    gtk_entry_set_text(GTK_ENTRY(username_entry_signin), "");
    gtk_entry_set_text(GTK_ENTRY(password_entry_signin), "");
    gtk_entry_set_text(GTK_ENTRY(username_entry_signup), "");
    gtk_entry_set_text(GTK_ENTRY(password_entry_signup), "");
    gtk_entry_set_text(GTK_ENTRY(password_entry_signup_confirm), "");

    if (timer_id) {
        g_source_remove(timer_id);
    }

    if (users_store != NULL) {
        gtk_list_store_clear(users_store);
    }

    if (chats_store != NULL) {
        gtk_list_store_clear(chats_store);
    }

    if (messages_store != NULL) {
        gtk_list_store_clear(messages_store);
    }

    gtk_label_set_text(GTK_LABEL(chat_user_username), "");
    mx_clear_list(&chat_list);
    mx_clear_list(&message_list);
    GdkPixbuf *pixbuf;
    pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, 1, 1);
    gtk_image_set_from_pixbuf(GTK_IMAGE(user_chat_icon), pixbuf);
    gtk_widget_show_all(sign_in_window);
    gtk_label_set_text(GTK_LABEL(error_label), "");
}

void gtk_initialisation(void) {
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, REG_LOGIN_GLADE, NULL);
    sign_in_window = GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_window"));
    g_signal_connect(sign_in_window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    sign_up_mini_button = GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_mini_button"));
    sign_in_mini_button = GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_mini_button"));
    sign_in_button = GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_button"));
    sign_up_button = GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_button"));
    show_passw_button = GTK_WIDGET(gtk_builder_get_object(builder, "show_passw_button"));
    error_label = GTK_WIDGET(gtk_builder_get_object(builder, "error_label"));
    inform_label_sign_up = GTK_WIDGET(gtk_builder_get_object(builder, "inform_label_sign_up"));
    username_entry_signin = GTK_WIDGET(gtk_builder_get_object(builder, "username_entry_signin"));
    password_entry_signin = GTK_WIDGET(gtk_builder_get_object(builder, "password_entry_signin"));

    g_signal_connect(sign_up_mini_button, "clicked", G_CALLBACK(onBtnSignUpMiniClicked), NULL);
    g_signal_connect(sign_in_mini_button, "clicked", G_CALLBACK(onBtnSignInMiniClicked), NULL);
    g_signal_connect(sign_in_button, "clicked", G_CALLBACK(onSignInClicked), NULL);
    g_signal_connect(sign_up_button, "clicked", G_CALLBACK(onSignUpClicked), NULL);
    g_signal_connect(show_passw_button, "toggled", G_CALLBACK(onShowPasswButtonClicked), NULL);

    builder_chat = gtk_builder_new();
    gtk_builder_add_from_file(builder_chat, CHAT_GLADE, NULL);
    profile_button = GTK_WIDGET(gtk_builder_get_object(builder_chat, "profile_button"));
    log_out_button = GTK_WIDGET(gtk_builder_get_object(builder_chat, "log_out_button"));
    change_passw_button = GTK_WIDGET(gtk_builder_get_object(builder_chat, "change_passw_button"));
    change_button_on_passw_change_window = GTK_WIDGET(gtk_builder_get_object(builder_chat, "change_button_on_passw_change_window"));
    inform_label = GTK_WIDGET(gtk_builder_get_object(builder_chat, "inform_label"));
    reconection_label = GTK_WIDGET(gtk_builder_get_object(builder_chat, "reconection_label"));
    profile_go_back = GTK_WIDGET(gtk_builder_get_object(builder_chat, "profile_go_back"));
    go_back_button_chng_window = GTK_WIDGET(gtk_builder_get_object(builder_chat, "go_back_button_chng_window"));
    add_chat_button = GTK_WIDGET(gtk_builder_get_object(builder_chat, "add_chat_button"));
    go_back_button_addchat_window = GTK_WIDGET(gtk_builder_get_object(builder_chat, "go_back_button_addchat_window"));
    send_message_button = GTK_WIDGET(gtk_builder_get_object(builder_chat, "send_message_button"));
    our_chat = GTK_WIDGET(gtk_builder_get_object(builder_chat, "our_chat"));
    search_entry_username = GTK_SEARCH_ENTRY(gtk_builder_get_object(builder_chat, "search_entry_username"));
    entry_message = GTK_WIDGET(gtk_builder_get_object(builder_chat, "entry_message"));
    messages_scroller = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder_chat, "messages_scroller"));
    messages_selection = GTK_TREE_SELECTION(gtk_builder_get_object(builder_chat, "messages_selection"));
    change_theme = GTK_SWITCH(gtk_builder_get_object(builder_chat, "change_theme"));
    users_selection = GTK_TREE_SELECTION(gtk_builder_get_object(builder_chat, "users_selection"));
    chats_selection = GTK_TREE_SELECTION(gtk_builder_get_object(builder_chat, "chat_selection"));
    create_button = GTK_WIDGET(gtk_builder_get_object(builder_chat, "create_button"));
    add_chat_window = GTK_WIDGET(gtk_builder_get_object(builder_chat, "add_chat_window"));
    chat_username = GTK_WIDGET(gtk_builder_get_object(builder_chat, "chat_username"));
    chat_user_username = GTK_WIDGET(gtk_builder_get_object(builder_chat, "chat_user_username"));
    profile_username = GTK_WIDGET(gtk_builder_get_object(builder_chat, "profile_username"));
    home_page = GTK_WIDGET(gtk_builder_get_object(builder_chat, "home_page"));
    profile_page = GTK_WIDGET(gtk_builder_get_object(builder_chat, "profile_page"));
    username_entry_signup = GTK_WIDGET(gtk_builder_get_object(builder, "username_entry_signup"));
    password_entry_signup = GTK_WIDGET(gtk_builder_get_object(builder, "password_entry_signup"));
    password_entry_signup_confirm = GTK_WIDGET(gtk_builder_get_object(builder, "password_entry_signup_confirm"));
    user_chat_icon = GTK_WIDGET(gtk_builder_get_object(builder_chat, "user_chat_icon"));
    g_signal_connect(our_chat, "destroy", G_CALLBACK(on_window_destroy), NULL);

    g_signal_connect(profile_button, "clicked", G_CALLBACK(onProfileButtonClicked), NULL);
    g_signal_connect(change_passw_button, "clicked", G_CALLBACK(onChangePasswButtonClicked), NULL);
    g_signal_connect(change_button_on_passw_change_window, "clicked", G_CALLBACK(onChangeButtonOnPasswButtonClicked), NULL);
    g_signal_connect(profile_go_back, "clicked", G_CALLBACK(onProfileGoBackButtonClicked), NULL);
    g_signal_connect(go_back_button_chng_window, "clicked", G_CALLBACK(onChangePasswGoBackButtonClicked), NULL);
    g_signal_connect(add_chat_button, "clicked", G_CALLBACK(onAddChatButtonClicked), NULL);
    g_signal_connect(go_back_button_addchat_window, "clicked", G_CALLBACK(onAddChatGoBackButtonClicked), NULL);
    g_signal_connect(send_message_button, "clicked", G_CALLBACK(getMessage), NULL);
    g_signal_connect(log_out_button, "clicked", G_CALLBACK(onLogOutButtonClicked), NULL);
    g_signal_connect(create_button, "clicked", G_CALLBACK(onCreateChatButtonClicked), NULL);
    g_signal_connect(our_chat, "key-press-event", G_CALLBACK(on_enter_press), send_message_button);
    g_signal_connect(search_entry_username, "search-changed", G_CALLBACK(users_entry_search), NULL);
    g_signal_connect(entry_message, "changed", G_CALLBACK(messages_entry_changed), NULL);
    g_signal_connect(messages_selection, "changed", G_CALLBACK(messages_selection_changed), NULL);
    g_signal_connect(users_selection, "changed", G_CALLBACK(on_users_selection_changed), NULL);
    g_signal_connect(chats_selection, "changed", G_CALLBACK(on_chats_selection_changed), NULL);
    g_signal_connect(change_theme, "state-set", G_CALLBACK(day_theme), NULL);

    gtk_widget_show_all(sign_in_window);

    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_css_provider_load_from_file(provider, g_file_new_for_path(STYLES_CSS), NULL);

    create_message_side();

    gtk_main();
    g_object_unref(builder);
    g_object_unref(builder_chat);
    return;
}

