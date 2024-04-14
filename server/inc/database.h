#pragma once

#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <sqlite/include/sqlite3.h> // -I /usr/bin/      -I /opt/homebrew/opt/        -L /opt/homebrew/opt/sqlite/lib -lsqlite3

#define DATABASE_FILE "server/database/uchat.db"
#define CREATE_DB_TABLES_FILE "server/database/create_db_tables.sql"
#define USERS_TABLE_COLUMNS_COUNT 4 // id, login(username), password, icon_id

/* Functions from "send_response_to_client.c" file */
void log_db_error_to_file(char *message, sqlite3 *db);
sqlite3 *db_open(void);
void db_close(sqlite3 *db);
//bool db_create(void); // todo deprecated
t_user_data *db_get_user_data(char *username);
void db_put_user_data(char *username, char *password);

