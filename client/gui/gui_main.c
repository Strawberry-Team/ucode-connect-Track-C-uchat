#include "client.h"
//#include "gui.h"
#include "api.h"

const gchar *username = "Anton";
// Глобальный флаг для отслеживания применения фильтрации
bool filter_applied = false;
// Инициализация значений по умолчанию
t_server *server;
t_client *client;
/*char message_buffer[1024] = "";
char selected_message_text[1024] = "";
char selected_message_sender[40] = "";
char message_old[1024] = "";
char selected_chat[40] = "";*/
//int selected_message_id;
bool if_deleted = false;


GtkBuilder *builder;
GtkBuilder *builder_chat;

GtkWidget *username_entry_signin;
GtkWidget *password_entry_signin;
GtkWidget *sign_up_mini_button;
GtkWidget *sign_in_mini_button;
GtkWidget *sign_in_button;
GtkWidget *sign_up_button;
GtkWidget *failed_passw_label_sign_up;
GtkWidget *failed_username_label_sign_up;
GtkWidget *failed_username_passw_label_sign_up;
GtkWidget *show_passw_button;
GtkWidget *sign_in_window;
GtkWidget *inform_label_sign_up;


GtkWidget *profile_button;
GtkWidget *change_passw_button;
GtkWidget *change_button_on_passw_change_window;
GtkWidget *inform_label;
GtkWidget *chat_username;
GtkWidget *error_label;
GtkWidget *profile_go_back;
GtkWidget *go_back_button_chng_window;
GtkWidget *add_chat_button;
GtkWidget *add_chat_window;
GtkWidget *go_back_button_addchat_window;
GtkWidget *create_button;
GtkWidget *send_message_button;
GtkWidget *our_chat;
GtkSearchEntry *search_entry_username;
GtkWidget *entry_message;

GtkTreeSelection *users_selection;
GtkListStore *users_store;
GtkTreeView *users;


GtkTreeView *tree_view_messegabar;
GtkListStore *messages_store;
GtkScrolledWindow *messages_scroller;
GtkTreeSelection *messages_selection;


GtkWidget *context_menu;
GtkSwitch *change_theme;
bool being_edited;


GtkTreeView *tree_view_chatbar;
GtkListStore *chats_store;
GtkTreeSelection *chats_selection;
char message_buffer[1024];
char message_old[1024];
char selected_message_text[1024];
char selected_message_sender[40];
int selected_message_id;
char selected_username[40];
int selected_chat_id;
char selected_chat[40];




void scroll_to_bottom(void) {
    GtkAdjustment *vadjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(messages_scroller));
    gtk_adjustment_set_value(vadjustment, gtk_adjustment_get_upper(vadjustment));
    gtk_widget_show_all(GTK_WIDGET(messages_scroller));
}
/*GtkWidget *widget*/
gboolean scroll_delayed(void) {
    scroll_to_bottom();
    return G_SOURCE_REMOVE;
}

t_user_data *user_data = NULL;

gboolean check_username(const char *username) {
    while (*username) {

        if ((*username < '0' || *username > '9') && (*username < 'a' || *username > 'z') && *username != '-' && *username != '_') {
            return FALSE;
        }
        username++;
    }

    return TRUE;
}

// Функция для получения данных из полей ввода на окне sign_in_window
gboolean getSignInData(void) {


    const char *username = gtk_entry_get_text(GTK_ENTRY(username_entry_signin));
    const char *password = gtk_entry_get_text(GTK_ENTRY(password_entry_signin));

    if(!check_username(username) || strlen(username) < 5){
        return FALSE;
    } else {
        user_data = (t_user_data *) malloc(sizeof(t_user_data));
        //user_data->request_type = LOGIN;
        user_data->username = mx_strdup(username);
        user_data->password = mx_strdup(password);
        return TRUE;
    }


}


gboolean getSignUpDataUsername(void) {
    GtkWidget *username_entry_signup = GTK_WIDGET(gtk_builder_get_object(builder, "username_entry_signup"));
    GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(username_entry_signup));
    gchar *username = mx_strdup(gtk_entry_buffer_get_text(buffer));

    if(!check_username(username) || strlen(username) < 6){
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


// Сравнение пароля и его подтверждения
    if (strlen(password) == 0 || strlen(password_confirm) == 0 ||
        strlen(password) < 6 || strcmp(password, password_confirm) != 0 ||
        strchr(password, ' ') != NULL) {
        return FALSE; // Если хоть одно условие не выполнено, возвращаем FALSE
    }

    user_data->password = g_strdup(password_confirm);
    return TRUE;
}

void on_window_destroy(void) { /*GtkWidget *widget, gpointer data*/
    gtk_main_quit(); // Вийти з циклу обробки подій GTK
}

// Кнопка Sign up міні
void onBtnSignUpMiniClicked(GtkButton *sign_up_mini_button) {
    GtkWidget *sign_in_window = gtk_widget_get_toplevel(GTK_WIDGET(sign_up_mini_button)); // Отримання вікна(login), до якого відноситься кнопка
    gboolean is_registration_window_open = FALSE; // Прапорець, що вказує на те, чи відкрите вже вікно c registration
    gtk_widget_hide(sign_in_window); // Сховати поточне вікно

    // Якщо вікно registration вже відкрите, вийти з функції
    if (is_registration_window_open)
        return;

    // Якщо конструктор не існує, створити його і завантажити вікно з файлу
    if (!builder) {
        builder = gtk_builder_new();
        gtk_builder_add_from_file(builder, REG_LOGIN_GLADE, NULL);
    }

    GtkWidget *sign_up_window = GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_window")); // Отримати вікно для реєстрації(registration)
    g_signal_connect(sign_up_window , "destroy", G_CALLBACK(on_window_destroy), NULL);
    gtk_widget_show_all(sign_up_window); // Показати вікно для реєстрації(registration)

    gtk_widget_set_opacity(error_label, 0.0);
    gtk_entry_set_text(GTK_ENTRY(username_entry_signin), "");
    gtk_entry_set_text(GTK_ENTRY(password_entry_signin), "");

    is_registration_window_open = TRUE; // Позначити, що вікно registration відкрите
}
void signUpClose(void) {
    GtkWidget *sign_in_window = GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_window"));
    gtk_widget_hide(sign_in_window); // Скрываем окно вместо уничтожения
    GtkWidget *username_entry_signup = GTK_WIDGET(gtk_builder_get_object(builder, "username_entry_signup"));
    GtkWidget *password_entry_signup = GTK_WIDGET(gtk_builder_get_object(builder, "password_entry_signup"));
    GtkWidget *password_entry_signup_confirm = GTK_WIDGET(gtk_builder_get_object(builder, "password_entry_signup_confirm"));

    gtk_entry_set_text(GTK_ENTRY(username_entry_signup), "");
    gtk_entry_set_text(GTK_ENTRY(password_entry_signup), "");
    gtk_entry_set_text(GTK_ENTRY(password_entry_signup_confirm), "");
}
// Кнопка Sign in міні
void onBtnSignInMiniClicked(GtkButton *sign_in_mini_button) {
    GtkWidget *sign_up_window = gtk_widget_get_toplevel(GTK_WIDGET(sign_in_mini_button)); // Отримання вікна(registration), до якого відноситься кнопка
    gboolean is_login_window_open = FALSE; // Прапорець, що вказує на те, чи відкрите вже вікно c login
    gtk_widget_hide(sign_up_window); // Сховати поточне вікно

    // Якщо вікно login вже відкрите, вийти з функції
    if (is_login_window_open)
        return;

    // Якщо конструктор не існує, створити його і завантажити вікно з файлу
    if (!builder) {
        builder = gtk_builder_new();
        gtk_builder_add_from_file(builder, REG_LOGIN_GLADE, NULL);
    }

    signUpClose();

    GtkWidget *sign_in_window = GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_window")); // Отримати вікно для входу(login)
    g_signal_connect(sign_in_window , "destroy", G_CALLBACK(on_window_destroy), NULL);
    gtk_widget_show_all(sign_in_window); // Показати вікно для входу(login)
    gtk_widget_set_opacity(inform_label_sign_up, 0.0);
    is_login_window_open = TRUE; // Позначити, що вікно login відкрите
}

void update_in_model(int selected_message_id, const char *message_buffer) {
    GtkTreeIter iter;
    gboolean found = FALSE;

    found = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(messages_store), &iter);
    while (found) {
        int str_message_id;
        gtk_tree_model_get(GTK_TREE_MODEL(messages_store), &iter, MESSAGE_ID_COLUMN, &str_message_id, -1); // Получаем ID как строку
        //int message_id = atoi(str_message_id); // Преобразуем строку в целое число

        //g_print("Хуеня как ты работаешь%d\n", message_id); // Добавляем отладочный вывод

        if (str_message_id == selected_message_id) {
            // Найдена строка с выбранным message_id, обновляем данные
            if(if_deleted) {
                //gtk_list_store_remove(messages_store, &iter);
                gtk_list_store_set(messages_store, &iter, 3, "{deleted}", -1);
                if_deleted = false;
            } else {
                gtk_list_store_set(messages_store, &iter, 3, message_buffer, -1);
            }
            // Вызываем функцию обновления представления для обновления отображения на экране
            gtk_widget_queue_draw(GTK_WIDGET(tree_view_messegabar));
            g_print("Сообщение с ID %d найдено и обновлено.\n", selected_message_id); // Добавляем отладочный вывод
            //g_free(str_message_id); // Освобождаем память, выделенную для строки
            return;
        }
        //g_free(str_message_id); // Освобождаем память, выделенную для строки
        found = gtk_tree_model_iter_next(GTK_TREE_MODEL(messages_store), &iter);
    }

    // Если сообщение с выбранным ID не найдено, можно вывести сообщение об ошибке
    g_print("Сообщение с ID %d не найдено в модели данных.\n", selected_message_id);
}


gboolean tree_view_messegabar_button_press_event(GtkWidget *treeview, GdkEventButton *event, gpointer data)
{
    GtkTreePath *path;
    GtkTreeViewColumn *column;
    GtkTreeModel *model;

    if (event->button == GDK_BUTTON_SECONDARY)
    {

        if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), (gint)event->x, (gint)event->y, &path, &column, NULL, NULL) && gtk_tree_selection_get_selected(GTK_TREE_SELECTION(messages_selection), &model, NULL))
        {
            GtkWidget *menu = GTK_WIDGET(data);
            GList *children = gtk_container_get_children(GTK_CONTAINER(menu));
            GtkWidget *edit_item = GTK_WIDGET(g_list_nth_data(children, 0));
            GtkWidget *delete_item = GTK_WIDGET(g_list_nth_data(children, 1));
            g_list_free(children);
            // Show/hide "Edit" and "Delete" options based on whether the selected message was sent by the user
            if (strcmp(selected_message_sender, username) != 0)
            {
                gtk_widget_hide(edit_item);
                gtk_widget_hide(delete_item);
                if (GTK_IS_MENU(menu))
                {
                    gtk_menu_popup_at_pointer(GTK_MENU(menu), NULL);
                }
            }
            else
            {
                gtk_widget_show_all(menu);
                if (GTK_IS_MENU(menu))
                {
                    gtk_menu_popup_at_pointer(GTK_MENU(menu), NULL);
                }
            }
        }
    }
    if (event->button == GDK_BUTTON_PRIMARY)
    {
        if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), (gint)event->x, (gint)event->y, &path, &column, NULL, NULL) && gtk_tree_selection_get_selected(GTK_TREE_SELECTION(messages_selection), &model, NULL))
        {
            // nothin
        }
        else
        {
            if (!being_edited)
            {
                gtk_tree_selection_unselect_all(messages_selection);
            }
        }
    }

    return FALSE;
}

void edit_messages(void) { /*GtkMenuItem *menuitem, gpointer data*/
    // code to edit the selected message
    g_print("Button 'Edit message'\n");
    being_edited = true;
    strcpy(message_old, message_buffer);
    gtk_entry_set_text(GTK_ENTRY(entry_message), selected_message_text);
    gtk_tree_selection_unselect_all(messages_selection);

}
/*void dell_masage_update_model(int selected_message_id) {
    GtkTreeIter iter;
    gboolean found = FALSE;

    found = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(messages_store), &iter);
    while (found) {
        gchar *str_message_id;
        gtk_tree_model_get(GTK_TREE_MODEL(messages_store), &iter, 1, &str_message_id, -1); // Получаем ID как строку
        int message_id = atoi(str_message_id); // Преобразуем строку в целое число

        if (message_id == selected_message_id) {
            // Найдена строка с выбранным message_id, удаляем данные
            gtk_list_store_set(messages_store, &iter, 4, "{deleted}", -1);
            // Вызываем функцию обновления представления для обновления отображения на экране
            // Вызываем функцию обновления представления для обновления отображения на экране
            gtk_widget_queue_draw(GTK_WIDGET(tree_view_messegabar));
            g_print("Сообщение с ID %d удалено.\n", selected_message_id); // Добавляем отладочный вывод
            g_free(str_message_id); // Освобождаем память, выделенную для строки
            return;
        }
        g_free(str_message_id); // Освобождаем память, выделенную для строки
        found = gtk_tree_model_iter_next(GTK_TREE_MODEL(messages_store), &iter);
    }

    // Если сообщение с выбранным ID не найдено, можно вывести сообщение об ошибке
    g_print("Сообщение с ID %d не найдено в модели данных.\n", selected_message_id);
}*/

void delete_messages(void) { /*GtkMenuItem *menuitem, gpointer data*/
    // code to delete the selected message
    g_print("Button 'Delete message'\n");
    /*gint result;*/
    if_deleted = true;
    update_in_model(selected_message_id, message_buffer); /*selected_chat,*/
    gtk_tree_selection_unselect_all(messages_selection);

}


void messages_selection_changed(GtkWidget *messages_selection)
{
    gchar *value;
    int value1;
    GtkTreeIter iter;
    GtkTreeModel *model;

    if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(messages_selection), &model, &iter) == FALSE)
        return;

    gtk_tree_model_get(model, &iter, MESSAGE_TEXT_COLUMN, &value, -1);
    strcpy(selected_message_text, value);
    gtk_tree_model_get(model, &iter, MESSAGE_SENDER_COLUMN, &value, -1);
    strcpy(selected_message_sender, value);
    gtk_tree_model_get(model, &iter, MESSAGE_ID_COLUMN, &value1, -1);

    if (selected_message_id != value1)
    {
        selected_message_id =value1;
        g_print("Selected id%d", selected_message_id);
        if (being_edited)
        {
            gtk_entry_set_text(GTK_ENTRY(entry_message), message_old);
            being_edited = false;
        }
        else
        {
            gtk_entry_set_text(GTK_ENTRY(entry_message), message_buffer);
        }
    }
}

//fill_messages_list(selected_chat, messages_store);
//функия должна отправлять с=запрос на сервер/БД и получать сообщения все в зависимости от выбранного чата и загружать их в меседж стор


void create_message_side(void) {
// messages_store
    tree_view_messegabar = GTK_TREE_VIEW(gtk_builder_get_object(builder_chat, "tree_view_messegabar"));
    messages_store = gtk_list_store_new(N_MESSAGES_COLUMNS, G_TYPE_STRING, GDK_TYPE_PIXBUF,  G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);
    GtkCellRenderer *renderer_time = gtk_cell_renderer_text_new();
    g_object_set(renderer_time, "xalign", 0.0, NULL);
    g_object_set(renderer_time, "yalign", 0.5, NULL); // Устанавливаем выравнивание текста по верхнему краю
    g_object_set(renderer_time, "font", "Arial 15", NULL);

    GtkTreeViewColumn *msg_col1 = gtk_tree_view_column_new_with_attributes("Time:", renderer_time, "text", MESSAGE_TIME_COLUMN, NULL);
    gtk_tree_view_append_column(tree_view_messegabar, msg_col1);
    GtkCellRenderer *renderer = gtk_cell_renderer_pixbuf_new();
    //gtk_tree_view_column_set_fixed_width(msg_col1, 100);
    // Set the GtkListStore as the model for the TreeView
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
    //gtk_tree_view_column_set_fixed_width(msg_col_icon, 35);

    // Set the visibility of the icon column as needed
    gtk_tree_view_column_set_visible(msg_col_icon, TRUE);


    GtkTreeViewColumn *msg_col2 = gtk_tree_view_column_new_with_attributes("Sender:", render_sender, "text", MESSAGE_SENDER_COLUMN, NULL);
    gtk_tree_view_append_column(tree_view_messegabar, msg_col2);

    GtkTreeViewColumn *msg_col3 = gtk_tree_view_column_new_with_attributes("Message:", render_message, "text", MESSAGE_TEXT_COLUMN, NULL);
    gtk_tree_view_append_column(tree_view_messegabar, msg_col3);


    //set_column_height(GTK_TREE_VIEW(tree_view_messegabar), MESSAGE_TIME_COLUMN, 10); // Set the height of the first column to 50 pixels
    //set_column_height(GTK_TREE_VIEW(treeview), COLUMN_AGE, 30);

    gtk_tree_view_column_set_expand(msg_col3, TRUE);
    gtk_tree_view_column_set_max_width(msg_col3, G_MAXINT);

    GtkTreeViewColumn *msg_col0 = gtk_tree_view_column_new_with_attributes("ID:", gtk_cell_renderer_text_new(), "text", MESSAGE_ID_COLUMN, NULL);
    gtk_tree_view_append_column(tree_view_messegabar, msg_col0);
    gtk_tree_view_column_set_visible(msg_col0, FALSE);

    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree_view_messegabar), FALSE);



    // context menu for messages
    context_menu = gtk_menu_new();

    GtkWidget *edit_massage = gtk_menu_item_new_with_label("Edit message");
    g_signal_connect(edit_massage, "activate", G_CALLBACK(edit_messages), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(context_menu), edit_massage);

    GtkWidget *delete_massage = gtk_menu_item_new_with_label("Delete message");
    g_signal_connect(delete_massage, "activate", G_CALLBACK(delete_messages), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(context_menu), delete_massage);

    // Set up the TreeView to show the context menu
    gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(tree_view_messegabar), FALSE);
    gtk_tree_view_set_enable_tree_lines(GTK_TREE_VIEW(tree_view_messegabar), FALSE);
    gtk_tree_view_set_enable_search(GTK_TREE_VIEW(tree_view_messegabar), FALSE);
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(tree_view_messegabar), GTK_TREE_VIEW_GRID_LINES_NONE);
    gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(tree_view_messegabar), FALSE);
    g_signal_connect(tree_view_messegabar, "button-press-event", G_CALLBACK(tree_view_messegabar_button_press_event), context_menu);



    // chats_store
    tree_view_chatbar = GTK_TREE_VIEW(gtk_builder_get_object(builder_chat, "tree_view_chatbar"));
    chats_store = gtk_list_store_new(N_CHATS_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    //fill_chats_list(username, chats_store);

    // Set the GtkListStore as the model for the TreeView
    gtk_tree_view_set_model(tree_view_chatbar, GTK_TREE_MODEL(chats_store));

    // Create the GtkTreeViewColumns for each column of data
    GtkTreeViewColumn *chat_col0 = gtk_tree_view_column_new_with_attributes("ID", gtk_cell_renderer_text_new(), "text", CHAT_ID_COLUMN, NULL);
    gtk_tree_view_append_column(tree_view_chatbar, chat_col0);
    gtk_tree_view_column_set_visible(chat_col0, FALSE);

    GtkTreeViewColumn *chat_col1 = gtk_tree_view_column_new_with_attributes("Chatname", gtk_cell_renderer_text_new(), "text", CHAT_CHATNAME_COLUMN, NULL);
    gtk_tree_view_append_column(tree_view_chatbar, chat_col1);
    gtk_tree_view_column_set_visible(chat_col1, FALSE);

    GtkTreeViewColumn *chat_col2 = gtk_tree_view_column_new_with_attributes("Sender", gtk_cell_renderer_text_new(), "text", CHAT_SENDER_COLUMN, NULL);
    gtk_tree_view_append_column(tree_view_chatbar, chat_col2);
    gtk_tree_view_column_set_visible(chat_col2, FALSE);

    GtkTreeViewColumn *chat_col3 = gtk_tree_view_column_new_with_attributes("Receiver", gtk_cell_renderer_text_new(), "text", CHAT_RECEIVER_COLUMN, NULL);
    gtk_tree_view_append_column(tree_view_chatbar, chat_col3);
    gtk_tree_view_column_set_visible(chat_col3, FALSE);

    GtkTreeViewColumn *chat_col4 = gtk_tree_view_column_new_with_attributes("Chats:", gtk_cell_renderer_text_new(), "text", CHAT_CHATS_COLUMN, NULL);
    gtk_tree_view_append_column(tree_view_chatbar, chat_col4);

    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree_view_chatbar), FALSE);

    //timer_id = g_timeout_add(1000, (GSourceFunc)chats_store_update, chats_store);
}






void onSignInClicked(void) {
    if (!builder_chat) { // Перевірка, чи конструктор вже існує
        builder_chat = gtk_builder_new(); // Створити конструктор
        gtk_builder_add_from_file(builder_chat, CHAT_GLADE, NULL); // Завантажити файл client/resources/chat.glade
    }

    if(getSignInData()) {
        send_login_request(client->ssl, LOGIN, user_data);
    } else {
        g_print("hyeta");
        gtk_widget_show(error_label);
        gtk_label_set_text(GTK_LABEL(error_label), (const gchar*) "Не верный логин или пароль!");
        gtk_widget_set_opacity(error_label, 1.0);

    }
}

void onSignUpClicked(GtkButton *sign_up_button) {
    if (!builder_chat) { // Перевірка, чи конструктор вже існує
        builder_chat = gtk_builder_new(); // Створити конструктор
        gtk_builder_add_from_file(builder_chat, CHAT_GLADE, NULL); // Завантажити файл client/resources/chat.glade
    }

    gtk_widget_show(inform_label_sign_up);
    user_data = (t_user_data *) malloc(sizeof(t_user_data));
    gboolean usernameValidation = getSignUpDataUsername();
    gboolean passwordsMatch = getSignUpDataPasswords();


    // Проверка совпадения паролей
    if (!passwordsMatch && !usernameValidation) {
        g_print("Passwords and username unavailable!\n");
        gtk_label_set_text(GTK_LABEL(inform_label_sign_up), (const gchar*) "Passwords and username unavailable!");
        gtk_widget_set_opacity(inform_label_sign_up, 1.0);
    } else if (!usernameValidation) {
        gtk_label_set_text(GTK_LABEL(inform_label_sign_up), (const gchar*) "Username validation crashed!");
        gtk_widget_set_opacity(inform_label_sign_up, 1.0);
        g_print("Username validation crashed!\n");
    } else if (!passwordsMatch) {
        gtk_label_set_text(GTK_LABEL(inform_label_sign_up), (const gchar*) "Password unavailable!");
        gtk_widget_set_opacity(inform_label_sign_up, 1.0);
        g_print("Password unavailable!\n");
    } else {
        g_print("%s\n",user_data->password);
        g_print("%s\n",user_data->username);
//        send_registration_req_to_server(client->ssl, REGISTER, user_data);
        send_registration_request(client->ssl, REGISTER, user_data);
        GtkWidget *sign_up_window = gtk_widget_get_toplevel(GTK_WIDGET(sign_up_button)); // Отримання вікна(chat), до якого відноситься кнопка
        gtk_widget_hide(sign_up_window);
        GtkWidget *chat = GTK_WIDGET(gtk_builder_get_object(builder_chat, "our_chat")); // Отримати вікно чату
        g_signal_connect(chat, "destroy", G_CALLBACK(on_window_destroy), NULL);
        gtk_widget_show_all(chat); // Показати вікно чату
    }

}

gboolean onProfilePageClose(GtkWidget *widget) {
    gtk_widget_hide(widget); // Скрываем окно вместо уничтожения
    return TRUE; // Возвращаем TRUE, чтобы предотвратить уничтожение окна
}

void onProfileButtonClicked(void) { /*GtkButton *profile_button*/
    if (!builder_chat) { // Перевірка, чи конструктор вже існує
        builder_chat = gtk_builder_new(); // Створити конструктор
        gtk_builder_add_from_file(builder_chat, CHAT_GLADE, NULL); // Завантажити файл client/resources/chat.glade
    }

    GtkWidget *profile_page = GTK_WIDGET(gtk_builder_get_object(builder_chat, "profile_page"));// Отримати вікно профілю
    gtk_window_set_position(GTK_WINDOW(profile_page), GTK_WIN_POS_CENTER);
    // Прикрепляем обработчик закрытия окна, который скрывает окно вместо уничтожения
    g_signal_connect(profile_page, "delete-event", G_CALLBACK(onProfilePageClose), NULL);
    gtk_widget_show_all(profile_page); // Показати вікно профілю
}

gboolean onChangePasswordWindowClose(GtkWidget *change_password_window) {
    gtk_widget_hide(change_password_window); // Скрываем окно вместо уничтожения
    GtkWidget *change_button_on_passw_change_window = GTK_WIDGET(gtk_builder_get_object(builder_chat, "change_button_on_passw_change_window"));
    GtkWidget *change_current_password = GTK_WIDGET(gtk_builder_get_object(builder_chat, "change_current_password"));
    GtkWidget *change_new_password = GTK_WIDGET(gtk_builder_get_object(builder_chat, "change_new_password"));
    GtkWidget *change_confirm_new_password = GTK_WIDGET(gtk_builder_get_object(builder_chat, "change_confirm_new_password"));

    gtk_entry_set_text(GTK_ENTRY(change_current_password), "");
    gtk_entry_set_text(GTK_ENTRY(change_new_password), "");
    gtk_entry_set_text(GTK_ENTRY(change_confirm_new_password), "");

    gtk_widget_set_opacity(inform_label, 0.0);
    gtk_widget_set_sensitive(change_button_on_passw_change_window, TRUE);
    return TRUE; // Возвращаем TRUE, чтобы предотвратить уничтожение окна
}


void onChangePasswButtonClicked(GtkButton *change_passw_button) {
    GtkWidget *profile_page = gtk_widget_get_toplevel(GTK_WIDGET(change_passw_button)); // Отримання вікна(chat), до якого відноситься кнопка
    gtk_widget_hide(profile_page); // Сховати поточне вікно
    if (!builder_chat) { // Перевірка, чи конструктор вже існує
        builder_chat = gtk_builder_new(); // Створити конструктор
        gtk_builder_add_from_file(builder_chat, CHAT_GLADE, NULL); // Завантажити файл client/resources/chat.glade
    }

    GtkWidget *change_password_window = GTK_WIDGET(gtk_builder_get_object(builder_chat, "change_password_window")); // Отримати вікно профілю
    // Прикрепляем обработчик закрытия окна, который скрывает окно вместо уничтожения
    g_signal_connect(change_password_window, "delete-event", G_CALLBACK(onChangePasswordWindowClose), NULL);
    gtk_widget_show_all(change_password_window); // Показати вікно профілю

}

gboolean getChangeDataPasswords(void) {

    GtkWidget *change_new_password = GTK_WIDGET(gtk_builder_get_object(builder_chat, "change_new_password"));
    GtkWidget *change_confirm_new_password = GTK_WIDGET(gtk_builder_get_object(builder_chat, "change_confirm_new_password"));
    const gchar *password_new = gtk_entry_get_text(GTK_ENTRY(change_new_password));
    const gchar *password_new_confirm = gtk_entry_get_text(GTK_ENTRY(change_confirm_new_password));

// Сравнение пароля и его подтверждения
    if (strcmp(password_new, password_new_confirm) == 0 && (strcmp(password_new, "") != 0 || strcmp(password_new_confirm, "") != 0)) { //ебаная залупа непонятная с != 0
        g_print("Password New: %s\n", password_new);
        g_print("Password new confirm: %s\n", password_new_confirm);
        return TRUE;
    } else {
        return FALSE;
    }
}


void onChangeButtonOnPasswButtonClicked(void) { /*GtkWidget *change_button_on_passw_change_window, gpointer data*/
    if (!builder_chat) { // Перевірка, чи конструктор вже існує
        builder_chat = gtk_builder_new(); // Створити конструктор
        gtk_builder_add_from_file(builder_chat, CHAT_GLADE, NULL); // Завантажити файл client/resources/chat.glade
    }

    gtk_widget_show(inform_label);
    gboolean passwordMatch = getChangeDataPasswords();

    if(!passwordMatch) {
        gtk_label_set_text(GTK_LABEL(inform_label), (const gchar*) "Ошибка смены пароля!");
        gtk_widget_set_opacity(inform_label, 1.0);
    } else {
        gtk_label_set_text(GTK_LABEL(inform_label), (const gchar*) "Успешно изменено!");
        gtk_widget_set_opacity(inform_label, 1.0);
    }

}

void onProfileGoBackButtonClicked(GtkButton *profile_go_back) {
    GtkWidget *profile_page = gtk_widget_get_toplevel(GTK_WIDGET(profile_go_back)); // Отримання вікна(chat), до якого відноситься кнопка
    gtk_widget_hide(profile_page); // Сховати поточне вікно
}

void onChangePasswGoBackButtonClicked(GtkButton *go_back_button_chng_window) {
    GtkWidget *change_password_window = gtk_widget_get_toplevel(GTK_WIDGET(go_back_button_chng_window)); // Отримання вікна(chat), до якого відноситься кнопка
    gtk_widget_hide(change_password_window); // Сховати поточне вікно

    if (!builder_chat) { // Перевірка, чи конструктор вже існує
        builder_chat = gtk_builder_new(); // Створити конструктор
        gtk_builder_add_from_file(builder_chat, CHAT_GLADE, NULL); // Завантажити файл client/resources/chat.glade
    }
    onChangePasswordWindowClose(change_password_window);
    gtk_label_set_text(GTK_LABEL(inform_label), (const gchar*) "Успешно изменено!");

    GtkWidget *profile_page = GTK_WIDGET(gtk_builder_get_object(builder_chat, "profile_page")); // Отримати вікно чату
    g_signal_connect(profile_page, "destroy", G_CALLBACK(on_window_destroy), NULL);
    gtk_widget_show_all(profile_page); // Показати вікно чату
}

gboolean onAddChatWindowClose(GtkWidget *widget) {
    gtk_widget_hide(widget); // Скрываем окно вместо уничтожения
    return TRUE; // Возвращаем TRUE, чтобы предотвратить уничтожение окна

}

// Обработчик события изменения текста в GtkEntry

void fill_users_list(GtkListStore *users_store) {

    GtkTreeIter iter;
    // Вставляем первую строку с ID 1 и именем "Антон"
    gtk_list_store_append(users_store, &iter); // Добавляем новую строку
    gtk_list_store_set(users_store, &iter,
                       USER_ID, 1, // Значение ID
                       USER_USERNAME, "Антон", // Имя пользователя
                       -1);

    // Добавляем вторую строку с ID 2 и именем "Вадим"
    gtk_list_store_append(users_store, &iter); // Добавляем новую строку
    gtk_list_store_set(users_store, &iter,
                       USER_ID, 2, // Значение ID
                       USER_USERNAME, "Вадим", // Имя пользователя
                       -1);
}


gboolean filter_func(GtkTreeModel *model, GtkTreeIter *iter, gpointer data)
{
    const gchar *search = (const gchar *)data;
    gchar *username;

    // Получаем имя пользователя из модели
    gtk_tree_model_get(model, iter, USER_USERNAME, &username, -1);

    // Сравниваем имя пользователя с введенным текстом
    gboolean match = strcmp(username, search) == 0;

    g_free(username);

    return match;
}

void users_entry_search(GtkSearchEntry *search_entry_username) {
    const gchar *search;

    // Получаем текст из строки поиска
    search = gtk_entry_get_text(GTK_ENTRY(search_entry_username));


    // Создаем фильтр для модели
    GtkTreeModelFilter *filter = GTK_TREE_MODEL_FILTER(
            gtk_tree_model_filter_new(GTK_TREE_MODEL(users_store), NULL));

    // Устанавливаем функцию фильтрации
    gtk_tree_model_filter_set_visible_func(filter, filter_func, (gpointer) search, NULL);

    // Устанавливаем отфильтрованную модель в представление списка пользователей
    gtk_tree_view_set_model(users, GTK_TREE_MODEL(filter));
}



void onAddChatButtonClicked(void) { /*GtkButton *add_chat_button*/
    if (!builder_chat) { // Перевірка, чи конструктор вже існує
        builder_chat = gtk_builder_new(); // Створити конструктор
        gtk_builder_add_from_file(builder_chat, CHAT_GLADE, NULL); // Завантажити файл client/resources/chat.glade
    }

    if (!users_store) { // Если модель еще не создана, создаем ее
        users_store = gtk_list_store_new(N_COLUMNS, G_TYPE_INT, G_TYPE_STRING);
        users = GTK_TREE_VIEW(gtk_builder_get_object(builder_chat, "users"));
        gtk_tree_view_set_model(users, GTK_TREE_MODEL(users_store));

        // Создаем колонки дерева представлений
        GtkTreeViewColumn *user_column = gtk_tree_view_column_new_with_attributes("ID", gtk_cell_renderer_text_new(), "text", USER_ID, NULL);
        gtk_tree_view_append_column(users, user_column);
        gtk_tree_view_column_set_visible(user_column, FALSE);

        GtkTreeViewColumn *user_column1 = gtk_tree_view_column_new_with_attributes("Users", gtk_cell_renderer_text_new(), "text", USER_USERNAME, NULL);
        gtk_tree_view_append_column(users, user_column1);

        gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(users), FALSE);

        fill_users_list(users_store);
    }



    // Отображаем окно чата
    GtkWidget *add_chat_window = GTK_WIDGET(gtk_builder_get_object(builder_chat, "add_chat_window"));
    g_signal_connect(add_chat_window, "delete-event", G_CALLBACK(onAddChatWindowClose), NULL);
    selected_username[0] = '\0';

    gtk_widget_show_all(add_chat_window);
}



void onAddChatGoBackButtonClicked(GtkButton *go_back_button_addchat_window) {
    GtkWidget *add_chat_window = gtk_widget_get_toplevel(GTK_WIDGET(go_back_button_addchat_window)); // Отримання вікна(chat), до якого відноситься кнопка
    gtk_widget_hide(add_chat_window); // Сховати поточне вікно

    GtkWidget *search_entry_username = GTK_WIDGET(gtk_builder_get_object(builder_chat, "search_entry_username"));
    gtk_entry_set_text(GTK_ENTRY(search_entry_username), "");
    // gtk_list_store_clear(users_store);
}

void messages_entry_changed(GtkEntry *entry_message) {
    sprintf(message_buffer, "%s", gtk_entry_get_text(entry_message));
}

// Глобальная переменная для хранения следующего доступного индекса сообщения
static int next_message_id = 1;

GdkPixbuf *load_pixbuf_from_file(const gchar *file_path) {
    GError *error = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(file_path, &error);
    if (error != NULL) {
        g_error_free(error);
        return NULL;
    }
    return pixbuf;
}



// Функция для добавления нового сообщения
void print_massages(void) { /*GtkButton *send_message_button, gpointer data*/
    g_print("Сообщение: %s\n", message_buffer);

    // Проверяем, что сообщение не пустое
    if (message_buffer[0] != '\0') {
        // Получаем текущее время
        time_t rawtime;
        struct tm *timeinfo;
        char time_buffer[80];
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", timeinfo);

        // Добавляем новую строку в модель данных
        GtkTreeIter iter;
        gtk_list_store_append(messages_store, &iter);


        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(ICON_MESSAGE_PNG, NULL);


        /* char message_id[10]; // Буфер для хранения строкового представления индекса
         sprintf(message_id, "%d", next_message_id);*/

        // Заполняем столбцы данными о сообщении
        gtk_list_store_set(messages_store, &iter,
                           MESSAGE_TIME_COLUMN, time_buffer,
                           ICON_COLUMN, pixbuf,
                           MESSAGE_SENDER_COLUMN, username,  // Имя отправителя
                           MESSAGE_TEXT_COLUMN, message_buffer,
                           MESSAGE_ID_COLUMN, next_message_id,
                           -1);

        // Увеличиваем значение следующего доступного индекса для следующего сообщения
        next_message_id++;
        g_object_unref(pixbuf);
        // Очищаем буфер сообщения после отправки
        message_buffer[0] = '\0';

        // Обновляем treeview, чтобы отобразить новую строку
        gtk_widget_queue_draw(GTK_WIDGET(tree_view_messegabar));
    }
}


void getMessage(void) { /*GtkButton *send_message_button, gpointer data */
    if (message_buffer[0] != '\0' /*&& selected_chat[0] != '\0'*/) {
        if (being_edited) {
            if (strcmp(selected_message_text, message_buffer) != 0) {
                g_print("ID: %d\n", selected_message_id);
                g_print("Выбранное сообщение: %s\n", selected_message_text);
                g_print("Изменить на: %s\n", message_buffer);
                update_in_model(selected_message_id, message_buffer);
                // Здесь вызываем функцию для обновления сообщения в базе данных
            }
            gtk_entry_set_text(GTK_ENTRY(entry_message), message_old);
            being_edited = false;
        }
        else {
            print_massages();
            gtk_editable_delete_text(GTK_EDITABLE(entry_message), 0, -1);
            g_timeout_add(600, (GSourceFunc)scroll_delayed, messages_scroller);
        }
    }
}


void onShowPasswButtonClicked(GtkToggleButton *show_passw_button/*, gpointer user_data*/) {
    GtkWidget *password_entry_signin = GTK_WIDGET(gtk_builder_get_object(builder, "password_entry_signin"));

    gtk_entry_set_visibility(GTK_ENTRY(password_entry_signin), gtk_toggle_button_get_active(show_passw_button));
}


gboolean on_enter_press(GdkEventKey *event, gpointer user_data) { /*GtkWidget *widget, */
    if (event->keyval == GDK_KEY_Return || event->keyval == GDK_KEY_KP_Enter) {
        // Вызываем событие "clicked" для кнопки
        gtk_button_clicked(GTK_BUTTON(user_data));
        return TRUE; // Событие обработано
    }
    return FALSE; // Продолжаем обработку события
}

void day_theme(__attribute__((unused)) GtkWidget *button, gboolean state/*, void *data*/) {
    GtkCssProvider *provider = gtk_css_provider_new();

    if (state) {
        // Состояние включено: загрузить дневную тему
        gtk_css_provider_load_from_path(provider, STYLES_1_CSS, NULL);
    } else {
        // Состояние выключено: загрузить ночную тему
        gtk_css_provider_load_from_path(provider, STYLES_CSS, NULL);
    }
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                              GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void on_users_selection_changed(GtkWidget *s)
{
    gchar *value;
    GtkTreeIter iter;
    GtkTreeModel *model;

    if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(s), &model, &iter) == FALSE)
        return;

    gtk_tree_model_get(model, &iter, USER_USERNAME, &value, -1);
    strcpy(selected_username, value);
    g_print("Выбранный пользователь: %s\n", selected_username);

}



// Функция для проверки совпадений имён
bool check_username_exists(const gchar *selected_username) {
    GtkTreeIter iter;
    gboolean valid;
    valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(chats_store), &iter);

    // Проход по всем записям в модели чатов
    while (valid) {
        gchar *existing_username;
        gtk_tree_model_get(GTK_TREE_MODEL(chats_store), &iter, CHAT_RECEIVER_COLUMN, &existing_username, -1);

        // Сравнение имени текущего пользователя с именем из модели чатов
        if (g_strcmp0(existing_username, selected_username) == 0) {
            g_free(existing_username);
            return true; // Имя пользователя уже существует в модели чатов
        }

        g_free(existing_username);
        valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(chats_store), &iter);
    }
    return false; // Имя пользователя не найдено в модели чатов
}

void add_chat(const gchar *username, const gchar *selected_username) {

    GtkTreeIter iter;

    gtk_list_store_append(chats_store, &iter);

    // Добавляем данные чата в соответствующие столбцы
    gtk_list_store_set(chats_store, &iter,
                       CHAT_ID_COLUMN, "1", // Пустая строка, так как ID может быть заполнен вашей логикой
                       CHAT_CHATNAME_COLUMN, "хуй", // Пустая строка, если у вас нет имени чата
                       CHAT_SENDER_COLUMN, username,
                       CHAT_RECEIVER_COLUMN, selected_username,
                       CHAT_CHATS_COLUMN, selected_username, // Пример текста нового чата
                       -1);
}

void onCreateChatButtonClicked(void) { /*GtkButton *b, gpointer data*/
    gtk_entry_set_text(GTK_ENTRY(search_entry_username), "");
    if (check_username_exists(selected_username)) {
        g_print("Пользователь уже существует в модели чатов.\n");
        return;
    } else {
        add_chat(username, selected_username);
        gtk_widget_hide(add_chat_window);
    }
}

gpointer controller_thread(gpointer data) {
    controller();
    (void)data;  // This will suppress the unused parameter warning
    return NULL;
}


void gtk_inital_function(void) {

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, REG_LOGIN_GLADE, NULL);

    sign_in_window = GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_window")); // Отримати вікно для входу(login)
    g_signal_connect(sign_in_window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    sign_up_mini_button = GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_mini_button")); // Отримати кнопку "Sign Up Mini" с вікна login
    sign_in_mini_button = GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_mini_button")); // Отримати кнопку "Sign In Mini" с вікна registration
    sign_in_button = GTK_WIDGET(gtk_builder_get_object(builder, "sign_in_button")); // Отримати кнопку "Sign In" с вікна login
    sign_up_button = GTK_WIDGET(gtk_builder_get_object(builder, "sign_up_button"));
    show_passw_button = GTK_WIDGET(gtk_builder_get_object(builder, "show_passw_button"));
    error_label = GTK_WIDGET(gtk_builder_get_object(builder, "error_label"));
    inform_label_sign_up = GTK_WIDGET(gtk_builder_get_object(builder, "inform_label_sign_up"));
    username_entry_signin = GTK_WIDGET(gtk_builder_get_object(builder, "username_entry_signin"));
    password_entry_signin = GTK_WIDGET(gtk_builder_get_object(builder, "password_entry_signin"));

    g_signal_connect(sign_up_mini_button, "clicked", G_CALLBACK(onBtnSignUpMiniClicked), NULL); // Підключити функцію для обробки натискання на кнопку "Sign Up Mini" с вікна login
    g_signal_connect(sign_in_mini_button, "clicked", G_CALLBACK(onBtnSignInMiniClicked), NULL); // Підключити функцію для обробки натискання на кнопку "Sign In Mini" с вікна registration
    g_signal_connect(sign_in_button, "clicked", G_CALLBACK(onSignInClicked), NULL); // Підключити функцію для обробки натискання на кнопку "Sign In" с вікна login
    g_signal_connect(sign_up_button, "clicked", G_CALLBACK(onSignUpClicked), NULL);
    g_signal_connect(show_passw_button, "toggled", G_CALLBACK(onShowPasswButtonClicked), NULL);

    builder_chat = gtk_builder_new();
    gtk_builder_add_from_file(builder_chat, CHAT_GLADE, NULL);
    profile_button = GTK_WIDGET(gtk_builder_get_object(builder_chat, "profile_button")); // Отримати кнопку "profile_button" з вікна "our_chat"
    change_passw_button = GTK_WIDGET(gtk_builder_get_object(builder_chat, "change_passw_button"));
    change_button_on_passw_change_window = GTK_WIDGET(gtk_builder_get_object(builder_chat, "change_button_on_passw_change_window"));
    inform_label = GTK_WIDGET(gtk_builder_get_object(builder_chat, "inform_label"));
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
    chats_selection = GTK_TREE_SELECTION(gtk_builder_get_object(builder_chat, "chats_selection"));
    create_button = GTK_WIDGET(gtk_builder_get_object(builder_chat, "create_button"));
    add_chat_window = GTK_WIDGET(gtk_builder_get_object(builder_chat, "add_chat_window"));
    chat_username = GTK_WIDGET(gtk_builder_get_object(builder_chat, "chat_username"));


    g_signal_connect(profile_button, "clicked", G_CALLBACK(onProfileButtonClicked), NULL); // Підключити функцію для обробки натискання на кнопку "profile_button"
    g_signal_connect(change_passw_button, "clicked", G_CALLBACK(onChangePasswButtonClicked), NULL);
    g_signal_connect(change_button_on_passw_change_window, "clicked", G_CALLBACK(onChangeButtonOnPasswButtonClicked), NULL);
    g_signal_connect(profile_go_back, "clicked", G_CALLBACK(onProfileGoBackButtonClicked), NULL);
    g_signal_connect(go_back_button_chng_window, "clicked", G_CALLBACK(onChangePasswGoBackButtonClicked), NULL);
    g_signal_connect(add_chat_button, "clicked", G_CALLBACK(onAddChatButtonClicked), NULL);
    g_signal_connect(go_back_button_addchat_window, "clicked", G_CALLBACK(onAddChatGoBackButtonClicked), NULL);
    g_signal_connect(send_message_button, "clicked", G_CALLBACK(getMessage), NULL);
    g_signal_connect(create_button, "clicked", G_CALLBACK(onCreateChatButtonClicked), NULL);
    g_signal_connect(our_chat, "key-press-event", G_CALLBACK(on_enter_press), send_message_button);
    g_signal_connect(search_entry_username, "search-changed", G_CALLBACK(users_entry_search), NULL);
    g_signal_connect(entry_message, "changed", G_CALLBACK(messages_entry_changed), NULL);
    g_signal_connect(messages_selection, "changed", G_CALLBACK(messages_selection_changed), NULL);
    g_signal_connect(users_selection, "changed", G_CALLBACK(on_users_selection_changed), NULL);
    //g_signal_connect(chats_selection, "changed", G_CALLBACK(on_chats_selection_changed), NULL);
    g_signal_connect(change_theme, "state-set", G_CALLBACK(day_theme), NULL);

    gtk_widget_show_all(sign_in_window); // Показати вікно для входу(login)

    GtkCssProvider *provider = gtk_css_provider_new(); // Створити постачальника CSS
    GdkDisplay *display = gdk_display_get_default(); // Отримати відображення за замовчуванням
    GdkScreen *screen = gdk_display_get_default_screen(display); // Отримати екран за замовчуванням
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION); // Додати постачальника стилів для екрану
    gtk_css_provider_load_from_file(provider, g_file_new_for_path(STYLES_CSS), NULL); // Завантажити CSS з файлу
    create_message_side();

    GThread *thread;
    thread = g_thread_new(NULL, controller_thread, NULL);

    gtk_main(); // Почати цикл обробки подій GTK
    /*g_object_unref(builder);
    g_object_unref(builder_chat); */
    // Звільнити пам'ять, зайняту конструктором GtkBuilder
    g_thread_join(thread);

    return;
}
