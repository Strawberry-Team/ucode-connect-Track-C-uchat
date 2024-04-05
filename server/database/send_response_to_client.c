#include "database.h"

void send_status_response(SSL *ssl, t_request_type request_type, t_status_type status) {
    char *json_string = NULL;
    cJSON *json = cJSON_CreateObject();

    if (!json) {
        log_to_file("Could not create the cJSON object to send a status response to the client");
        cJSON_Delete(json);
        return;
    }

    if (!cJSON_AddNumberToObject(json, "request_type", request_type)
        || !cJSON_AddNumberToObject(json, "status", status)) {
        log_to_file("Could not add the data in the cJSON object to send a status response to the client");
        CJSON_Delete(json_data_obj);
        cJSON_Delete(json);
        return;
    }

    json_string = cJSON_Print(json);

    int bytes_written = SSL_write(ssl, json_string, strlen(json_string)); // або mx_strlen()

    if (bytes_written <= 0) {
        log_to_file("Could not write JSON string over the TLS/SSL connection to send a login response to the client");
        free(json_string); // або mx_strdel(&json_string);
    }

    free(json_string); // або mx_strdel(&json_string);
    CJSON_Delete(json_data_obj);
    cJSON_Delete(json);
    return;
}

void send_login_response(SSL *ssl, t_request_type request_type, t_status_type status, t_user_data *user_data) {
    char *json_string = NULL;
    cJSON *json = cJSON_CreateObject();
    cJSON *json_data_obj = cJSON_CreateObject();

    if (!json
        || !json_data_obj) {
        log_to_file("Could not create the cJSON object to send a login response to the client");
        send_status_response(ssl, request_type, ERROR_JSON_FAILED);
        CJSON_Delete(json_data_obj);
        cJSON_Delete(json);
        return;
    }

    if (!cJSON_AddItemToObject(json, "data", json_data_obj)
        || !cJSON_AddNumberToObject(json, "request_type", request_type)
        || !cJSON_AddNumberToObject(json, "status", status)
        || !cJSON_AddNumberToObject(json_data_obj, "id", user_data->id)
        || !cJSON_AddStringToObject(json_data_obj, "username", user_data->username)
        || !cJSON_AddStringToObject(json_data_obj, "password", user_data->password)
        || !cJSON_AddNumberToObject(json_data_obj, "icon_id", user_data->icon_id)) {
        log_to_file("Could not add the data in the cJSON object to send a login response to the client");
        send_status_response(ssl, request_type, ERROR_JSON_FAILED);
        CJSON_Delete(json_data_obj);
        cJSON_Delete(json);
        return;
    }

    json_string = cJSON_Print(json); // Друкує JSON-об'єкт або масив у форматі, що включає відступи та нові рядки. альтернатива cJSON_PrintUnformatted() - без відступів та нових рядків.

    int bytes_written = SSL_write(ssl, json_string, strlen(json_string)); // або mx_strlen()

    if (bytes_written <= 0) {
        log_to_file("Could not write JSON string over the TLS/SSL connection to send a login response to the client");
        free(json_string); // або mx_strdel(&json_string);
    }

    free(json_string); // або mx_strdel(&json_string);
    CJSON_Delete(json_data_obj);
    cJSON_Delete(json);
    return;
}

