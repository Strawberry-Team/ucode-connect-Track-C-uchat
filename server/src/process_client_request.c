#include "server.h"
#include "data_base.h"

t_user_data dbu_get_user_data(sqlite3 *db, char *username) {
    t_user_data *user_data = NULL;
    sqlite3_stmt *stmt;




}

void handle_login(cJSON *json) {
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
        return;
    }

    sqlite3 *db;
    db_init();
    db = db_open();

    t_user_data user_data;
    user_data = dbu_get_user_data(db, client_info->username);

}