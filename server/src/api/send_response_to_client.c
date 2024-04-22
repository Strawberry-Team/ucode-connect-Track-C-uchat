#include "api.h"

void send_status_response(SSL *ssl, t_request_type request_type, t_status_type status) {
    char *json_string = NULL;
    cJSON *json = cJSON_CreateObject();

    if (!json) {
        log_to_file("Could not create the cJSON object to send status response to the client", JSON_ERROR);
        return;
    }

    if (!cJSON_AddNumberToObject(json, "request_type", request_type)
        || !cJSON_AddNumberToObject(json, "status", status)) {
        log_to_file("Could not add the data in the cJSON object to send status response to the client", JSON_ERROR);
        cJSON_Delete(json);
        return;
    }

    json_string = cJSON_Print(json);
    int bytes_written = SSL_write(ssl, json_string, strlen(json_string));

    if (bytes_written <= 0) {
        log_to_file("Could not write JSON string over the TLS/SSL connection to send status response to the client", JSON_ERROR);
    }

    free(json_string);
    cJSON_Delete(json);
    return;
}

void send_auth_response(SSL *ssl, t_request_type request_type, t_status_type status, t_user_data *user_data) {
    char *json_string = NULL;
    cJSON *json = cJSON_CreateObject();
    cJSON *json_data_obj = cJSON_CreateObject();

    if (!json
        || !json_data_obj) {
        log_to_file("Could not create the cJSON object to send authentication response to the client", JSON_ERROR);
        send_status_response(ssl, request_type, ERROR_JSON);
        return;
    }

    if (!cJSON_AddNumberToObject(json, "request_type", request_type)
        || !cJSON_AddNumberToObject(json, "status", status)
        || !cJSON_AddItemToObject(json, "data", json_data_obj)
        || !cJSON_AddNumberToObject(json_data_obj, "id", user_data->id)
        || !cJSON_AddStringToObject(json_data_obj, "username", user_data->username)) {
        log_to_file("Could not add the data in the cJSON object to send authentication response to the client", JSON_ERROR);
        send_status_response(ssl, request_type, ERROR_JSON);
        cJSON_Delete(json);
        return;
    }

    json_string = cJSON_Print(json);
    int bytes_written = SSL_write(ssl, json_string, strlen(json_string));

    if (bytes_written <= 0) {
        log_to_file("Could not write JSON string over the TLS/SSL connection to send authentication response to the client", JSON_ERROR);
    }

    free(json_string);
    cJSON_Delete(json);
    return;
}

void send_get_users_for_search_response(SSL *ssl, t_status_type status, t_list *user_list) {
    t_request_type request_type = GET_USERS_FOR_SEARCH;
    char *json_string = NULL;
    cJSON *json = cJSON_CreateObject();
    cJSON *json_data_arr = NULL;

    if (!json) {
        log_to_file("Could not create the cJSON object to send GET_USERS_FOR_SEARCH response to the client", JSON_ERROR);
        send_status_response(ssl, request_type, ERROR_JSON);
        return;
    }

    if (!cJSON_AddNumberToObject(json, "request_type", request_type)
        || !cJSON_AddNumberToObject(json, "status", status)
        || !(json_data_arr = cJSON_AddArrayToObject(json, "data"))) {
        log_to_file("Could not add the data in the cJSON object to send GET_USERS_FOR_SEARCH response to the client", JSON_ERROR);
        send_status_response(ssl, request_type, ERROR_JSON);
        cJSON_Delete(json);
        return;
    }

    int list_size = mx_list_size(user_list);

    for (int i = 0; i < list_size; i++) {
        t_user_data *user_data = (t_user_data *)user_list->data;
        cJSON *data_item = cJSON_CreateObject();

        if (!data_item
            || !cJSON_AddItemToArray(json_data_arr, data_item)
            || !cJSON_AddNumberToObject(data_item, "id", user_data->id)
            || !cJSON_AddStringToObject(data_item, "username", user_data->username)) {
            log_to_file("Could not create the cJSON object to send GET_USERS_FOR_SEARCH response to the client", JSON_ERROR);
            send_status_response(ssl, request_type, ERROR_JSON);
            cJSON_Delete(json);
            return;
        }

        user_list = user_list->next;
    }

    json_string = cJSON_Print(json);
    int bytes_written = SSL_write(ssl, json_string, strlen(json_string));

    if (bytes_written <= 0) {
        log_to_file("Could not write JSON string over the TLS/SSL connection to send GET_USERS_FOR_SEARCH response to the client", JSON_ERROR);
    }

    free(json_string);
    cJSON_Delete(json);
    return;
}

void send_add_chat_response(SSL *ssl, t_status_type status, t_chat_data *chat_data) {
    t_request_type request_type = ADD_CHAT;
    char *json_string = NULL;
    cJSON *json = cJSON_CreateObject();
    cJSON *json_data_obj = cJSON_CreateObject();

    if (!json
        || !json_data_obj) {
        log_to_file("Could not create the cJSON object to send ADD_CHAT response to the client", JSON_ERROR);
        send_status_response(ssl, request_type, ERROR_JSON);
        return;
    }

    if (!cJSON_AddNumberToObject(json, "request_type", request_type)
        || !cJSON_AddNumberToObject(json, "status", status)
        || !cJSON_AddItemToObject(json, "data", json_data_obj)
        || !cJSON_AddNumberToObject(json_data_obj, "chat_id", chat_data->id)
        || !cJSON_AddStringToObject(json_data_obj, "title", chat_data->title)) {
        log_to_file("Could not add the data in the cJSON object to send ADD_CHAT response to the client", JSON_ERROR);
        send_status_response(ssl, request_type, ERROR_JSON);
        cJSON_Delete(json);
        return;
    }

    json_string = cJSON_Print(json);
    int bytes_written = SSL_write(ssl, json_string, strlen(json_string));

    if (bytes_written <= 0) {
        log_to_file("Could not write JSON string over the TLS/SSL connection to send ADD_CHAT response to the client", JSON_ERROR);
    }

    free(json_string);
    cJSON_Delete(json);
    return;
}

void send_get_chats_response(SSL *ssl, t_status_type status, t_list *chat_list) {
    t_request_type request_type = GET_CHATS;
    char *json_string = NULL;
    cJSON *json = cJSON_CreateObject();
    cJSON *json_data_arr = NULL;

    if (!json) {
        log_to_file("Could not create the cJSON object to send GET_CHATS response to the client", JSON_ERROR);
        send_status_response(ssl, request_type, ERROR_JSON);
        return;
    }

    if (!cJSON_AddNumberToObject(json, "request_type", request_type)
        || !cJSON_AddNumberToObject(json, "status", status)
        || !(json_data_arr = cJSON_AddArrayToObject(json, "data"))) {
        log_to_file("Could not add the data in the cJSON object to send GET_CHATS response to the client", JSON_ERROR);
        send_status_response(ssl, request_type, ERROR_JSON);
        cJSON_Delete(json);
        return;
    }

    int list_size = mx_list_size(chat_list);

    for (int i = 0; i < list_size; i++) {
        t_chat_data *chat_data = (t_chat_data *)chat_list->data;
        cJSON *data_item = cJSON_CreateObject();

        if (!data_item
            || !cJSON_AddItemToArray(json_data_arr, data_item)
            || !cJSON_AddNumberToObject(data_item, "chat_id", chat_data->id)
            || !cJSON_AddStringToObject(data_item, "title", chat_data->title)) {
            log_to_file("Could not create the cJSON object to send GET_CHATS response to the client", JSON_ERROR);
            send_status_response(ssl, request_type, ERROR_JSON);
            cJSON_Delete(json);
            return;
        }

        chat_list = chat_list->next;
    }

    json_string = cJSON_Print(json);
    int bytes_written = SSL_write(ssl, json_string, strlen(json_string));

    if (bytes_written <= 0) {
        log_to_file("Could not write JSON string over the TLS/SSL connection to send GET_CHATS response to the client", JSON_ERROR);
    }

    free(json_string);
    cJSON_Delete(json);
    return;
}

void send_get_messages_response(SSL *ssl, t_status_type status, t_list *msg_list) {
    t_request_type request_type = GET_MESSAGES;
    char *json_string = NULL;
    cJSON *json = cJSON_CreateObject();
    cJSON *json_data_arr = NULL;

    if (!json) {
        log_to_file("Could not create the cJSON object to send GET_MESSAGES response to the client", JSON_ERROR);
        send_status_response(ssl, request_type, ERROR_JSON);
        return;
    }

    if (!cJSON_AddNumberToObject(json, "request_type", request_type)
        || !cJSON_AddNumberToObject(json, "status", status)
        || !(json_data_arr = cJSON_AddArrayToObject(json, "data"))) {
        log_to_file("Could not add the data in the cJSON object to send GET_MESSAGES response to the client", JSON_ERROR);
        send_status_response(ssl, request_type, ERROR_JSON);
        cJSON_Delete(json);
        return;
    }

    int list_size = mx_list_size(msg_list);

    for (int i = 0; i < list_size; i++) {
        t_msg_data *msg_data = (t_msg_data *)msg_list->data;
        cJSON *data_item = cJSON_CreateObject();

        if (!data_item
            || !cJSON_AddItemToArray(json_data_arr, data_item)
            || !cJSON_AddNumberToObject(data_item, "message_id", msg_data->message_id)
            || !cJSON_AddNumberToObject(data_item, "user_id", msg_data->user_id)
            || !cJSON_AddStringToObject(data_item, "username", msg_data->username)
            || !cJSON_AddNumberToObject(data_item, "chat_id", msg_data->chat_id)
            || !cJSON_AddStringToObject(data_item, "body", msg_data->body)
            || !cJSON_AddNumberToObject(data_item, "created_at", msg_data->created_at)
            || !cJSON_AddNumberToObject(data_item, "updated_at", msg_data->updated_at)
            || !cJSON_AddNumberToObject(data_item, "deleted_at", msg_data->deleted_at)) {
            log_to_file("Could not create the cJSON object to send GET_MESSAGES response to the client", JSON_ERROR);
            send_status_response(ssl, request_type, ERROR_JSON);
            cJSON_Delete(json);
            return;
        }

        msg_list = msg_list->next;
    }

    json_string = cJSON_Print(json);
    int bytes_written = SSL_write(ssl, json_string, strlen(json_string));

    if (bytes_written <= 0) {
        log_to_file("Could not write JSON string over the TLS/SSL connection to send GET_MESSAGES response to the client", JSON_ERROR);
    }

    free(json_string);
    cJSON_Delete(json);
    return;
}

void send_message_response(SSL *ssl, t_request_type request_type, t_status_type status, t_msg_data *message_data) {
    char *json_string = NULL;
    cJSON *json = cJSON_CreateObject();
    cJSON *json_data_obj = cJSON_CreateObject();

    if (!json
        || !json_data_obj) {
        log_to_file("Could not create the cJSON object to send message response to the client", JSON_ERROR);
        send_status_response(ssl, request_type, ERROR_JSON);
        return;
    }

    if (!cJSON_AddNumberToObject(json, "request_type", request_type)
        || !cJSON_AddNumberToObject(json, "status", status)
        || !cJSON_AddItemToObject(json, "data", json_data_obj)
        || !cJSON_AddNumberToObject(json_data_obj, "message_id", message_data->message_id)
        || !cJSON_AddNumberToObject(json_data_obj, "user_id", message_data->user_id)
        || !cJSON_AddStringToObject(json_data_obj, "username", message_data->username)
        || !cJSON_AddNumberToObject(json_data_obj, "chat_id", message_data->chat_id)
        || !cJSON_AddStringToObject(json_data_obj, "body", message_data->body)
        || !cJSON_AddNumberToObject(json_data_obj, "created_at", message_data->created_at)
        || !cJSON_AddNumberToObject(json_data_obj, "updated_at", message_data->updated_at)
        || !cJSON_AddNumberToObject(json_data_obj, "deleted_at", message_data->deleted_at)) {
        log_to_file("Could not add the data in the cJSON object to send message response to the client", JSON_ERROR);
        send_status_response(ssl, request_type, ERROR_JSON);
        cJSON_Delete(json);
        return;
    }

    json_string = cJSON_Print(json);
    int bytes_written = SSL_write(ssl, json_string, strlen(json_string));

    if (bytes_written <= 0) {
        log_to_file("Could not write JSON string over the TLS/SSL connection to send message response to the client", JSON_ERROR);
    }

    free(json_string);
    cJSON_Delete(json);
    return;
}

