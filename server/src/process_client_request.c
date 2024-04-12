#include "api.h"

bool handle_login(t_client *client_info, char *json_string) {
    cJSON *json = cJSON_Parse(json_string);

    if (!json) {
        const char *error_ptr = cJSON_GetErrorPtr();

        if (error_ptr != NULL) {
            char msg[200];
            sprintf(msg, "Could not parse the cJSON object from client. A pointer to the error location: [%s]", error_ptr);
            log_to_file(msg, ERROR);
        }

        cJSON_Delete(json);
        return false;
    }

    const cJSON *json_credentials = cJSON_GetObjectItemCaseSensitive(json, "credentials");

    if (cJSON_IsObject(json_credentials)) {
        const cJSON *json_username = cJSON_GetObjectItemCaseSensitive(json_credentials, "username");
        const cJSON *json_password = cJSON_GetObjectItemCaseSensitive(json_credentials, "password");

        if (cJSON_IsString(json_username)
            && cJSON_IsString(json_password)) {
            client_info->username = strdup((const char *)json_username->valuestring);
            client_info->password = strdup((const char *)json_password->valuestring);
        }
    } else {
        log_to_file("Could not parse the \"credentials\" from a cJSON object", ERROR);
        cJSON_Delete(json);
        return false;
    }

//    if (!db_create()) {
//        cJSON_Delete(json);
//        return false;
//    }

    sqlite3 *db = db_file_open();

    if (!db) {
        cJSON_Delete(json);
        return false;
    }

    t_user_data *user_data;
    user_data = db_get_user_data(db, client_info->username);

    if (!user_data) {
        log_to_file("The passed user's credentials is invalid", ERROR);
        send_status_response(client_info->ssl, LOGIN, ERROR_INVALID_CREDENTIALS);
        sqlite3_close(db);
        return false;
    }

    log_to_file("The passed user's credentials exists in the database", INFO);

    if (mx_strcmp(client_info->password, user_data->password) != 0) {
//    if (memcmp(user_data->password, client_info->password, SHA256_DIGEST_LENGTH) != 0) {
        log_to_file("The passed user's credentials is invalid", ERROR);
        send_status_response(client_info->ssl, LOGIN, ERROR_INVALID_CREDENTIALS);
        sqlite3_close(db);
        return false;
    }

    char msg[200];
    sprintf(msg, "User \"%s\" successfully logged in", client_info->username);
    log_to_file(msg, INFO);
    send_login_response(client_info->ssl, LOGIN, SUCCESS_VALID_CREDENTIALS, user_data);
//    sqlite3_close(db); // todo Do we need to close database in this point?
    return true;
}

