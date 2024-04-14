#include "api.h"

void process_server_response(t_request_type request_type, char *json_string) {
    if (request_type != REGISTER
        && request_type != LOGIN
        && request_type != UNKNOWN_REQUEST) {
        if (!handle_auth_response(json_string)) {
            log_to_file("Could not authenticate the user before processing the client request", ERROR);
            free_and_exit();
        }
    }

    switch (request_type) {
        case REGISTER:
            handle_auth_response(json_string);
            break;
        case LOGIN:
            handle_auth_response(json_string);
            break;
//        case CREATE_CHAT:
//            handle_chat_creation(json_string);
//            break;
//        case ADD_MEMBER_TO_CHAT:
//            handle_adding_new_member_to_chat(json_string);
//            break;
//        case GET_USER_CHATS:
//            handle_getting_chats(json_string);
//            break;
//        case SEND_MESSAGE:
//            handle_text_message_sending(json_string);
//            break;
//        case GET_MESSAGES_IN_CHAT:
//            handle_messages_in_chat_getting(json_string);
//            break;
//        case SEND_MESSAGE_AND_GET_MESSAGE_UPDATES:
//            handle_message_sending_and_messages_updates_getting(json_string);
//            break;
//        case DELETE_MESSAGE_AND_GET_MESSAGE_UPDATES:
//            handle_message_deleting_and_messages_updates_getting(json_string);
//            break;
//        case CHANGE_MESSAGE_AND_GET_MESSAGE_UPDATES:
//            handle_message_changing_and_message_updates_getting(json_string);
//            break;
//        case REPLY_TO_MESSAGE_AND_GET_MESSAGE_UPDATES:
//            handle_message_replying_and_message_updates_getting(json_string);
//            break;
//        case GET_MESSAGE_UPDATES:
//            handle_message_updates_getting(json_string);
//            break;
//        case GET_CHAT_MEMBERS:
//            handle_getting_chat_members(json_string);
//            break;
        case UNKNOWN_REQUEST:
            log_to_file("Could not process a client request. A non-existent request type was received", ERROR);
            break;
        default:
            break;
    }

    return;
}

bool handle_auth_response(char *json_string) {
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        log_to_file("Could not parse the cJSON object from server", CJSON_ERROR);
        cJSON_Delete(json);
        return false;
    }

    const cJSON *json_status = cJSON_GetObjectItemCaseSensitive(json, "status");
    int request_status;

    if (!cJSON_IsNumber(json_status)
        || json_status->valueint < 0
        || json_status->valueint >= STATUS_TYPE_COUNT) {
        request_status = UNKNOWN_STATUS;
        log_to_file("Could not parse the \"status\" field from a cJSON object", CJSON_ERROR);
        cJSON_Delete(json);
        return false;
    }

    request_status = json_status->valueint;

    if (request_status != SUCCESS_VALID_CREDENTIALS
        && request_status != SUCCESS) {
        gtk_widget_show(error_label);
        gtk_label_set_text(GTK_LABEL(error_label), (const gchar*) "Invalid user name or password!");
        gtk_widget_set_opacity(error_label, 1.0);
        // todo создать вывод сообщения об ошибке на фронте
        log_to_file("Wrong status for register or login request", CJSON_ERROR);
        cJSON_Delete(json);
        return false;
    }

    const cJSON *json_data_obj = cJSON_GetObjectItemCaseSensitive(json, "data");

    if (cJSON_IsObject(json_data_obj)) {
        const cJSON *json_id = cJSON_GetObjectItemCaseSensitive(json_data_obj, "id");
        const cJSON *json_username = cJSON_GetObjectItemCaseSensitive(json_data_obj, "username");
        const cJSON *json_password = cJSON_GetObjectItemCaseSensitive(json_data_obj, "password");
        const cJSON *json_icon_id = cJSON_GetObjectItemCaseSensitive(json_data_obj, "icon_id");

        if (cJSON_IsNumber(json_id)
            && cJSON_IsString(json_username)
            && cJSON_IsString(json_password)
            && cJSON_IsNumber(json_icon_id)) {
            client_info->id = json_id->valueint;
            client_info->username = mx_strdup((const char *)json_username->valuestring);
            client_info->password = mx_strdup((const char *)json_password->valuestring);
            client_info->icon_id = json_icon_id->valueint;
        }
    } else {
        log_to_file("Could not parse the \"data\" field from a cJSON object", CJSON_ERROR);
        cJSON_Delete(json);
        return false;
    }

    gtk_widget_hide(sign_in_window);
    GtkWidget *chat = GTK_WIDGET(gtk_builder_get_object(builder_chat, "our_chat")); // Отримати вікно чату
    g_signal_connect(chat, "destroy", G_CALLBACK(on_window_destroy), NULL);
    gtk_widget_show_all(chat); // Показати вікно чату
    gtk_widget_show(chat_username);
    gtk_label_set_text(GTK_LABEL(chat_username), client_info->username);

    // todo printf for testing
    printf("RESPONSE:\nUser data:\nid: %d\nusername: %s\npassword: %s\nicon_id: %d\n", client_info->id, client_info->username, client_info->password, client_info->icon_id);
    char msg[200];
    sprintf(msg, "RESPONSE:\nUser data:\nid: %d\nusername: %s\npassword: %s\nicon_id: %d\n", client_info->id, client_info->username, client_info->password, client_info->icon_id);
    log_to_file(msg, INFO);
    cJSON_Delete(json);
    return true;
}

