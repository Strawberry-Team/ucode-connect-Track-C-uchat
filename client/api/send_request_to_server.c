#include "api.h"

void send_registration_request(SSL *ssl, t_request_type request_type, t_user_data *user_data) {
    char *json_string = NULL;
    cJSON *json = cJSON_CreateObject();
    cJSON *json_data_obj = cJSON_CreateObject();

    if (!json
        || !json_data_obj) {
        log_to_file("Could not create the cJSON object to send a registration request to the server", CJSON_ERROR);
        cJSON_Delete(json_data_obj);
        cJSON_Delete(json);
        return;
    }

    if (!cJSON_AddNumberToObject(json, "request_type", request_type)
        || !cJSON_AddItemToObject(json, "data", json_data_obj)
        || !cJSON_AddStringToObject(json_data_obj, "username", user_data->username)
        || !cJSON_AddStringToObject(json_data_obj, "password", user_data->password)) {
        log_to_file("Could not add the data in the cJSON object to send a registration request to the server", CJSON_ERROR);
        cJSON_Delete(json);
        return;
    }

    json_string = cJSON_Print(json);

    int bytes_written = SSL_write(ssl, json_string, strlen(json_string));

    if (bytes_written <= 0) {
        log_to_file("Could not write JSON string over the TLS/SSL connection to send a registration request to the server", CJSON_ERROR);
    }

    log_to_file("Registration request successfully sent", INFO);
    free(json_string);
    cJSON_Delete(json);
    return;
}

void send_login_request(SSL *ssl, t_request_type request_type, t_user_data *user_data) {
    char *json_string = NULL;
    cJSON *json = cJSON_CreateObject();
    cJSON *json_credentials_obj = cJSON_CreateObject();

    if (!json
        || !json_credentials_obj) {
        log_to_file("Could not create the cJSON object to send a login request to the server", CJSON_ERROR);
        cJSON_Delete(json_credentials_obj);
        cJSON_Delete(json);
        return;
    }

    if (!cJSON_AddNumberToObject(json, "request_type", request_type)
        || !cJSON_AddItemToObject(json, "credentials", json_credentials_obj)
        || !cJSON_AddStringToObject(json_credentials_obj, "username", user_data->username)
        || !cJSON_AddStringToObject(json_credentials_obj, "password", user_data->password)) {
        log_to_file("Could not add the data in the cJSON object to send a login request to the server", CJSON_ERROR);
        cJSON_Delete(json);
        return;
    }

    json_string = cJSON_Print(json);

    int bytes_written = SSL_write(ssl, json_string, strlen(json_string));

    if (bytes_written <= 0) {
        log_to_file("Could not write JSON string over the TLS/SSL connection to send a login request to the server", CJSON_ERROR);
    }

    log_to_file("Login request successfully sent", INFO);
    free(json_string);
    cJSON_Delete(json);
    return;
}

