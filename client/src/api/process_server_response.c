#include "api.h"

t_list *chat_list = NULL;
t_list *message_list = NULL;
bool first_load = true;

void process_server_response(t_request_type request_type, char *json_string) {
    switch (request_type) {
        case REGISTER:
        case LOGIN:
        case UPDATE_USER:
            handle_auth_response(json_string);
            break;
        case GET_USERS_FOR_SEARCH:
            handle_get_users_for_search_response(json_string);
            break;
        case ADD_CHAT:
            handle_add_chat_response(json_string);
            break;
        case GET_CHATS:
            handle_get_chats_response(json_string);
            break;
        case GET_MESSAGES:
            handle_get_messages_response(json_string);
            break;
        case ADD_MESSAGE:
        case UPDATE_MESSAGE:
        case DELETE_MESSAGE:
            break;
        case UNKNOWN_REQUEST:
            log_to_file("Could not process a client request. A non-existent request type was received", ERROR);
            break;
        default:
            break;
    }

    return;
}

void log_status_to_file(char *message, char *value) {
    char *main_message = (char *) malloc(200 * sizeof(char));
    char *data = (char *) malloc(100 * sizeof(char));
    sprintf(data, ": %s", value);
    strcat(main_message, message);
    strcat(main_message, data);
    log_to_file(main_message, INFO);
    free(data);
    free(main_message);
}

bool handle_auth_response(char *json_string) {
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        log_to_file("Could not parse the cJSON object from server", JSON_ERROR);
        return false;
    }

    const cJSON *json_status = cJSON_GetObjectItemCaseSensitive(json, "status");
    int status;

    if (!cJSON_IsNumber(json_status)
        || json_status->valueint < 0
        || json_status->valueint >= STATUS_TYPE_COUNT) {
        status = UNKNOWN_STATUS;
        log_to_file("Could not parse the \"status\" field from a cJSON object", JSON_ERROR);
        cJSON_Delete(json);
        return false;
    }

    status = json_status->valueint;
    const cJSON *json_req_type = cJSON_GetObjectItemCaseSensitive(json, "request_type");
    int request_type;

    if (!cJSON_IsNumber(json_req_type)
        || json_req_type->valueint < 0
        || json_req_type->valueint >= REQUEST_TYPE_COUNT) {
        request_type = UNKNOWN_REQUEST;
        log_to_file("Could not parse the \"request_type\" field from a cJSON object", JSON_ERROR);
        cJSON_Delete(json);
        return false;
    }

    request_type = json_req_type->valueint;

    if (status != SUCCESS) {
        if (request_type == LOGIN) {
            gtk_widget_show(error_label);
            gtk_label_set_text(GTK_LABEL(error_label), (const gchar*) "Invalid username or password!");
            gtk_widget_set_opacity(error_label, 1.0);
            log_to_file("Error status received for LOGIN request", JSON_ERROR);
        }
        else if (request_type == REGISTER) {
            gtk_widget_show(inform_label_sign_up);
            gtk_label_set_text(GTK_LABEL(inform_label_sign_up), (const gchar*) "Invalid registration!");
            gtk_widget_set_opacity(inform_label_sign_up, 1.0);
            log_to_file("Error status received for REGISTER request", JSON_ERROR);
        } else if (request_type == UPDATE_USER) {
            gtk_label_set_text(GTK_LABEL(inform_label), (const gchar*) "Failed to change the password");
        }

        log_to_file("Error status received for auth request", JSON_ERROR);
        cJSON_Delete(json);
        return false;
    }

    const cJSON *json_data_obj = cJSON_GetObjectItemCaseSensitive(json, "data");

    if (cJSON_IsObject(json_data_obj)) {
        const cJSON *json_id = cJSON_GetObjectItemCaseSensitive(json_data_obj, "id");
        const cJSON *json_username = cJSON_GetObjectItemCaseSensitive(json_data_obj, "username");

        if (!cJSON_IsNumber(json_id)
            || !cJSON_IsString(json_username)) {
            log_to_file("Could not parse the \"data\" fields from a cJSON object", JSON_ERROR);
            cJSON_Delete(json);
            return false;
        }

        client_info->id = json_id->valueint;
        client_info->username = mx_strdup((const char *)json_username->valuestring);
    } else {
        log_to_file("Could not parse the \"data\" object from a cJSON object", JSON_ERROR);
        cJSON_Delete(json);
        return false;
    }

    t_user_data *user_data = (t_user_data *) malloc(sizeof(t_user_data));
    user_data->username = mx_strdup(client_info->username);
    user_data->password = mx_strdup(client_info->password);

    if (request_type == LOGIN) {
        timer_id = g_timeout_add(1000, (GSourceFunc)chats_store_update, user_data);
        gtk_widget_hide(sign_in_window);
        GtkWidget *chat = GTK_WIDGET(gtk_builder_get_object(builder_chat, "our_chat"));
        g_signal_connect(chat, "destroy", G_CALLBACK(on_window_destroy), NULL);
        gtk_widget_show_all(chat);
        gtk_widget_show(chat_username);
        gtk_label_set_text(GTK_LABEL(chat_username), client_info->username);
    } else if (request_type == REGISTER) {
        timer_id = g_timeout_add(1000, (GSourceFunc)chats_store_update, user_data);
        GtkWidget *sign_up_window = gtk_widget_get_toplevel(GTK_WIDGET(sign_up_button));
        gtk_widget_hide(sign_up_window);
        GtkWidget *chat = GTK_WIDGET(gtk_builder_get_object(builder_chat, "our_chat"));
        g_signal_connect(chat, "destroy", G_CALLBACK(on_window_destroy), NULL);
        gtk_widget_show_all(chat);
        gtk_widget_show(chat_username);
        gtk_label_set_text(GTK_LABEL(chat_username), client_info->username);
    } else if (request_type == UPDATE_USER) {
        gtk_label_set_text(GTK_LABEL(inform_label), (const gchar*) "Successfully changed");
    }

    if (request_type == LOGIN
        || request_type == REGISTER) {
        log_status_to_file("Successfully logged in user", client_info->username);
    } else if (request_type == UPDATE_USER) {
        log_status_to_file("Successfully updated password for user", client_info->username);
    }

    cJSON_Delete(json);
    return true;
}

void handle_get_users_for_search_response(char *json_string) {
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        log_to_file("Could not parse the cJSON object from server", JSON_ERROR);
        return;
    }

    const cJSON *json_status = cJSON_GetObjectItemCaseSensitive(json, "status");
    int status;

    if (!cJSON_IsNumber(json_status)
        || json_status->valueint < 0
        || json_status->valueint >= STATUS_TYPE_COUNT) {
        status = UNKNOWN_STATUS;
        log_to_file("Could not parse the \"status\" field from a cJSON object", JSON_ERROR);
        cJSON_Delete(json);
        return;
    }

    status = json_status->valueint;

    if (status != SUCCESS) {
        gtk_widget_show(error_label);
        gtk_label_set_text(GTK_LABEL(error_label), (const gchar*) "Invalid username or password!");
        gtk_widget_set_opacity(error_label, 1.0);
        log_to_file("Error status received for GET_USERS_FOR_SEARCH request", JSON_ERROR);
        cJSON_Delete(json);
        return;
    }

    const cJSON *json_data_array = cJSON_GetObjectItemCaseSensitive(json, "data");

    if (!json_data_array
        || !cJSON_IsArray(json_data_array)) {
        log_to_file("Could not parse the \"data\" array from a cJSON object", JSON_ERROR);
        cJSON_Delete(json);
        return;
    }

    gtk_list_store_clear(users_store);
    GtkTreeIter iter;
    int array_size = cJSON_GetArraySize(json_data_array);

    for (int i = 0; i < array_size; i++) {
        const cJSON *json_obj = cJSON_GetArrayItem(json_data_array, i);
        const cJSON *json_id = cJSON_GetObjectItemCaseSensitive(json_obj, "id");
        const cJSON *json_username = cJSON_GetObjectItemCaseSensitive(json_obj, "username");

        if (!cJSON_IsObject(json_obj)
            || !cJSON_IsNumber(json_id)
            || !cJSON_IsString(json_username)) {
            log_to_file("Could not parse the \"data\" object or fields from a cJSON object", JSON_ERROR);
            cJSON_Delete(json);
            return;
        }

        gtk_list_store_append(users_store, &iter);
        gtk_list_store_set(users_store, &iter,
                           USER_ID, json_id->valueint,
                           USER_USERNAME, json_username->valuestring,
                           -1);
    }

    cJSON_Delete(json);
    return;
}

void handle_add_chat_response(char *json_string) {
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        log_to_file("Could not parse the cJSON object from server", JSON_ERROR);
        return;
    }

    const cJSON *json_status = cJSON_GetObjectItemCaseSensitive(json, "status");
    int status;

    if (!cJSON_IsNumber(json_status)
        || json_status->valueint < 0
        || json_status->valueint >= STATUS_TYPE_COUNT) {
        status = UNKNOWN_STATUS;
        log_to_file("Could not parse the \"status\" field from a cJSON object", JSON_ERROR);
        cJSON_Delete(json);
        return;
    }

    status = json_status->valueint;

    if (status != SUCCESS) {
        log_to_file("Error status received for ADD_CHAT request", ERROR);
        cJSON_Delete(json);
        return;
    }

    const cJSON *json_data_obj = cJSON_GetObjectItemCaseSensitive(json, "data");

    if (cJSON_IsObject(json_data_obj)) {
        const cJSON *json_chat_id = cJSON_GetObjectItemCaseSensitive(json_data_obj, "chat_id");
        const cJSON *json_title = cJSON_GetObjectItemCaseSensitive(json_data_obj, "title");

        if (!cJSON_IsNumber(json_chat_id)
            || !cJSON_IsString(json_title)) {
            log_to_file("Could not parse the \"data\" fields from a cJSON object", JSON_ERROR);
            cJSON_Delete(json);
            return;
        }
    } else {
        log_to_file("Could not parse the \"data\" object from a cJSON object", JSON_ERROR);
        cJSON_Delete(json);
        return;
    }

    log_status_to_file("Successfully added chat for the user", client_info->username);
    cJSON_Delete(json);
    return;
}

void handle_get_chats_response(char *json_string) {
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        log_to_file("Could not parse the cJSON object from server", JSON_ERROR);
        return;
    }

    const cJSON *json_status = cJSON_GetObjectItemCaseSensitive(json, "status");
    int status;

    if (!cJSON_IsNumber(json_status)
        || json_status->valueint < 0
        || json_status->valueint >= STATUS_TYPE_COUNT) {
        status = UNKNOWN_STATUS;
        log_to_file("Could not parse the \"status\" field from a cJSON object", JSON_ERROR);
        cJSON_Delete(json);
        return;
    }

    status = json_status->valueint;

    if (status != SUCCESS) {
        log_to_file("Error status received for GET_CHATS request", ERROR);
        cJSON_Delete(json);
        return;
    }

    const cJSON *json_data_array = cJSON_GetObjectItemCaseSensitive(json, "data");

    if (!json_data_array
        || !cJSON_IsArray(json_data_array)) {
        log_to_file("Could not parse the \"data\" array from a cJSON object", JSON_ERROR);
        cJSON_Delete(json);
        return;
    }

    GtkTreeIter iter;

    for (int i = 0; i < cJSON_GetArraySize(json_data_array); i++) {
        const cJSON *json_chat = cJSON_GetArrayItem(json_data_array, i);
        const cJSON *json_id = cJSON_GetObjectItemCaseSensitive(json_chat, "chat_id");
        const cJSON *json_title = cJSON_GetObjectItemCaseSensitive(json_chat, "title");

        if (!cJSON_IsObject(json_chat)
            || !cJSON_IsNumber(json_id)
            || !cJSON_IsString(json_title)) {
            log_to_file("Could not parse the chat object from the JSON data array", JSON_ERROR);
            cJSON_Delete(json);
            return;
        }

        t_chat_data *existing_chat = find_chat_in_list(json_id->valueint);

        if (!existing_chat) {
            t_chat_data *chat_data = (t_chat_data *) malloc(sizeof(t_chat_data));
            chat_data->id = json_id->valueint;
            chat_data->title = mx_strdup((const char *)json_title->valuestring);
            mx_push_back(&chat_list, chat_data);
            gtk_list_store_append(chats_store, &iter);
            gtk_list_store_set(chats_store, &iter,
                               CHAT_ID_COLUMN, chat_data->id,
                               CHAT_CHATNAME_COLUMN, chat_data->title,
                               -1);
        }
    }

    cJSON_Delete(json);
    return;
}

void handle_get_messages_response(char *json_string) {
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        log_to_file("Could not parse the cJSON object from server", JSON_ERROR);
        return;
    }

    const cJSON *json_status = cJSON_GetObjectItemCaseSensitive(json, "status");
    int status;

    if (!cJSON_IsNumber(json_status)
        || json_status->valueint < 0
        || json_status->valueint >= STATUS_TYPE_COUNT) {
        status = UNKNOWN_STATUS;
        log_to_file("Could not parse the \"status\" field from a cJSON object", JSON_ERROR);
        cJSON_Delete(json);
        return;
    }

    status = json_status->valueint;

    if (status != SUCCESS) {
        log_to_file("Error status received for GET_MESSAGES request", ERROR);
        cJSON_Delete(json);
        return;
    }

    const cJSON *json_data_array = cJSON_GetObjectItemCaseSensitive(json, "data");

    if (!json_data_array
        || !cJSON_IsArray(json_data_array)) {
        log_to_file("Could not parse the \"data\" array from a cJSON object", JSON_ERROR);
        cJSON_Delete(json);
        return;
    }

    GtkTreeIter iter;
    int array_size = cJSON_GetArraySize(json_data_array);

    for (int i = 0; i < array_size; i++) {
        const cJSON *json_obj = cJSON_GetArrayItem(json_data_array, i);
        const cJSON *json_message_id = cJSON_GetObjectItemCaseSensitive(json_obj, "message_id");
        const cJSON *json_user_id = cJSON_GetObjectItemCaseSensitive(json_obj, "user_id");
        const cJSON *json_username = cJSON_GetObjectItemCaseSensitive(json_obj, "username");
        const cJSON *json_chat_id = cJSON_GetObjectItemCaseSensitive(json_obj, "chat_id");
        const cJSON *json_body = cJSON_GetObjectItemCaseSensitive(json_obj, "body");
        const cJSON *json_created_at = cJSON_GetObjectItemCaseSensitive(json_obj, "created_at");
        const cJSON *json_updated_at = cJSON_GetObjectItemCaseSensitive(json_obj, "updated_at");
        const cJSON *json_deleted_at = cJSON_GetObjectItemCaseSensitive(json_obj, "deleted_at");

        if (!cJSON_IsObject(json_obj)
            || !cJSON_IsNumber(json_message_id)
            || !cJSON_IsNumber(json_user_id)
            || !cJSON_IsString(json_username)
            || !cJSON_IsNumber(json_chat_id)
            || !cJSON_IsString(json_body)
            || !cJSON_IsNumber(json_created_at)
            || !cJSON_IsNumber(json_updated_at)
            || !cJSON_IsNumber(json_deleted_at)) {
            log_to_file("Could not parse the \"data\" object or fields from a cJSON object", JSON_ERROR);
            cJSON_Delete(json);
            return;
        }

        if (is_clear) {
            gtk_list_store_clear(messages_store);
            mx_clear_list(&message_list);
            is_clear = false;
        }

        char *updated_at_text = NULL;
        t_msg_data *existing_message = find_message_in_list(json_message_id->valueint);

        if (json_deleted_at->valueint != 0
            && !first_load) {
            update_model(json_message_id->valueint, (const char *)json_body->valuestring, true);
            continue;
        }

        if (json_updated_at->valueint != 0
            && !first_load) {
            update_model(json_message_id->valueint, (const char *)json_body->valuestring, false);
            continue;
        }

        if (existing_message != NULL) {
            continue;
        }

        t_msg_data *msg_data = (t_msg_data *) malloc(sizeof(t_msg_data));
        msg_data->message_id = json_message_id->valueint;
        msg_data->user_id = json_user_id->valueint;
        msg_data->username = mx_strdup((const char *) json_username->valuestring);
        msg_data->chat_id = json_chat_id->valueint;
        msg_data->body = mx_strdup((const char *) json_body->valuestring);
        msg_data->created_at = (time_t) json_created_at->valueint;
        msg_data->updated_at = (time_t) json_updated_at->valueint;
        msg_data->deleted_at = (time_t) json_deleted_at->valueint;

        if (msg_data->deleted_at != 0) {
            msg_data->body = "{Deleted message}";
        } else if (msg_data->updated_at != 0) {
            updated_at_text = "edited";
        }
        mx_push_back(&message_list, msg_data);
        char *created_at = NULL;
        convert_timestamp_to_string(&created_at, msg_data->created_at);
        gtk_list_store_append(messages_store, &iter);
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(ICON_MESSAGE_PNG, NULL);
        gtk_list_store_set(messages_store, &iter,
                           MESSAGE_TIME_COLUMN, created_at,
                           ICON_COLUMN, pixbuf,
                           MESSAGE_SENDER_COLUMN, msg_data->username,
                           MESSAGE_TEXT_COLUMN, msg_data->body,
                           MESSAGE_EDITED_COLUMN, updated_at_text,
                           MESSAGE_ID_COLUMN, msg_data->message_id,
                           -1);
        free(created_at);
        gtk_widget_queue_draw(GTK_WIDGET(tree_view_messegabar));
    }

    first_load = false;
    cJSON_Delete(json);
    return;
}

void handle_message_response(char *json_string) {
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        log_to_file("Could not parse the cJSON object from server", JSON_ERROR);
        return;
    }

    const cJSON *json_status = cJSON_GetObjectItemCaseSensitive(json, "status");
    int status;

    if (!cJSON_IsNumber(json_status)
        || json_status->valueint < 0
        || json_status->valueint >= STATUS_TYPE_COUNT) {
        status = UNKNOWN_STATUS;
        log_to_file("Could not parse the \"status\" field from a cJSON object", JSON_ERROR);
        cJSON_Delete(json);
        return;
    }

    status = json_status->valueint;

    if (status != SUCCESS) {
        log_to_file("Error status received for message request", ERROR);
        cJSON_Delete(json);
        return;
    }

    t_msg_data *message_data = (t_msg_data *) malloc(sizeof(t_msg_data));
    const cJSON *json_data_obj = cJSON_GetObjectItemCaseSensitive(json, "data");

    if (cJSON_IsObject(json_data_obj)) {
        const cJSON *json_message_id = cJSON_GetObjectItemCaseSensitive(json_data_obj, "message_id");
        const cJSON *json_user_id = cJSON_GetObjectItemCaseSensitive(json_data_obj, "user_id");
        const cJSON *json_username = cJSON_GetObjectItemCaseSensitive(json_data_obj, "username");
        const cJSON *json_chat_id = cJSON_GetObjectItemCaseSensitive(json_data_obj, "chat_id");
        const cJSON *json_body = cJSON_GetObjectItemCaseSensitive(json_data_obj, "body");
        const cJSON *json_created_at = cJSON_GetObjectItemCaseSensitive(json_data_obj, "created_at");
        const cJSON *json_updated_at = cJSON_GetObjectItemCaseSensitive(json_data_obj, "updated_at");
        const cJSON *json_deleted_at = cJSON_GetObjectItemCaseSensitive(json_data_obj, "deleted_at");

        if (!cJSON_IsNumber(json_message_id)
            || !cJSON_IsNumber(json_user_id)
            || !cJSON_IsString(json_username)
            || !cJSON_IsNumber(json_chat_id)
            || !cJSON_IsString(json_body)
            || !cJSON_IsNumber(json_created_at)
            || !cJSON_IsNumber(json_updated_at)
            || !cJSON_IsNumber(json_deleted_at)) {
            log_to_file("Could not parse the \"data\" fields from a cJSON object", JSON_ERROR);
            free(message_data);
            cJSON_Delete(json);
            return;
        }

        message_data->message_id = json_message_id->valueint;
        message_data->user_id = json_user_id->valueint;
        message_data->username = mx_strdup((const char *) json_username->valuestring);
        message_data->chat_id = json_chat_id->valueint;
        message_data->body = mx_strdup((const char *) json_body->valuestring);
        message_data->created_at = json_created_at->valueint;
        message_data->updated_at = json_updated_at->valueint;
        message_data->deleted_at = json_deleted_at->valueint;
        char *created_at = NULL;
        convert_timestamp_to_string(&created_at, message_data->created_at);
        GtkTreeIter iter;
        gtk_list_store_append(messages_store, &iter);
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(ICON_MESSAGE_PNG, NULL);
        gtk_list_store_set(messages_store, &iter,
                           MESSAGE_TIME_COLUMN, created_at,
                           ICON_COLUMN, pixbuf,
                           MESSAGE_SENDER_COLUMN, message_data->username,
                           MESSAGE_TEXT_COLUMN, message_data->body,
                           MESSAGE_ID_COLUMN, message_data->message_id,
                           -1);
        free(created_at);
    } else {
        log_to_file("Could not parse the \"data\" object from a cJSON object", JSON_ERROR);
        free(message_data);
        cJSON_Delete(json);
        return;
    }

    log_status_to_file("Successfully received message for the user", client_info->username);
    free(message_data);
    cJSON_Delete(json);
    return;
}

t_chat_data *find_chat_in_list(int chat_id) {
    t_list *temp = chat_list;

    while (temp) {
        t_chat_data *chat_data = (t_chat_data *)temp->data;

        if (chat_data->id == chat_id) {
            return chat_data;
        }

        temp = temp->next;
    }

    return NULL;
}

t_msg_data *find_message_in_list(int message_id) {
    t_list *temp = message_list;

    while (temp) {
        t_msg_data *msg_data = (t_msg_data *)temp->data;

        if (msg_data && msg_data->message_id == message_id) {
            return msg_data;
        }

        temp = temp->next;
    }

    return NULL;
}

void convert_timestamp_to_string(char **string, time_t timestamp) {
    if (timestamp != 0) {
        struct tm *time_info;
        time_info = localtime(&timestamp);
        char time_string[50];
        strftime(time_string, sizeof(time_string), "%d-%m-%Y %H:%M:%S", time_info);
        *string = mx_strdup(time_string);
    } else {
        *string = NULL;
    }

    return;
}

