#include "api.h"

bool handle_login(cJSON *json) {
    cJSON *json_credentials = cJSON_GetObjectItemCaseSensitive(json, "credentials");

    if (!cJSON_IsNull(json_credentials) && cJSON_IsObject(json_credentials)) { // todo можна замінити на json_credentials != NULL
        cJSON *json_username = cJSON_GetObjectItemCaseSensitive(json_credentials, "username");

        if (!cJSON_IsNull(json_username) && cJSON_IsString(json_username)) {
            client_info->username = json_username->valuestring;
        }

        cJSON *json_password = cJSON_GetObjectItemCaseSensitive(json_credentials, "password");

        if (!cJSON_IsNull(json_password) && cJSON_IsString(json_password)) {
            client_info->password = json_password->valuestring;
        }
    } else {
        log_to_file("Could not parse the \"credentials\" from a cJSON object");
        cJSON_Delete(json);
        return false;
    }

    if (!db_create()) {
        cJSON_Delete(json);
        return false;
    }

    sqlite3 *db = db_file_open();

    if (!db) {
        cJSON_Delete(json);
        return false;
    }

    t_user_data *user_data;
    user_data = db_get_user_data(db, client_info->username);

    if (!user_data) {
        log_to_file("The passed user's credentials is invalid");
        send_status_response(client_info->ssl, LOGIN, ERROR_INVALID_CREDENTIALS);
        sqlite3_close(db);
        return false;
    }

    log_to_file("The passed user's credentials exists in the database");

    if (mx_strcmp(client_info->password, user_data->password) != 0) {
//    if (memcmp(user_data->password, client_info->password, SHA256_DIGEST_LENGTH) != 0) {
        log_to_file("The passed user's credentials is invalid");
        send_status_response(client_info->ssl, LOGIN, ERROR_INVALID_CREDENTIALS);
        sqlite3_close(db);
        return false;
    }

    char msg[200];
    sprintf(msg, "User \"%s\" successfully logged in", client_info->username);
    log_to_file(msg);
    send_login_response(client_info->ssl, LOGIN, SUCCESS_VALID_CREDENTIALS, user_data);
//    sqlite3_close(db); // todo Do we need to clode database in this point?
    return true;
}

