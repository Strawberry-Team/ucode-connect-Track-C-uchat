#pragma once

#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <sqlite/include/sqlite3.h> // -I /usr/bin/      -I /opt/homebrew/opt/        -L /opt/homebrew/opt/sqlite/lib -lsqlite3

#define DATABASE_FILE "server/database/uchat.db"
#define CREATE_DB_TABLES_FILE "server/database/create_db_tables.db"
#define USERS_TABLE_COLUMNS_COUNT 4 // id, login(username), password, icon_id

typedef struct s_user_data {
    int id;
    char *username;
    char *password; // якщо "unsigned char *password", то для SHA-256 хеша розмір поля буде 32 байти.
    int icon_id;
} t_user_data;

// enumeration of error types
typedef enum e_status_type{
    SUCCESS,
    SUCCESS_VALID_CREDENTIALS,
    SUCCESS_CHAT_EXIST,
    SUCCESS_USER_IN_CHAT,
    ERROR_JSON_FAILED,
    ERROR_INVALID_CREDENTIALS,
    ERROR_CHAT_NONEXIST,
    ERROR_USER_NOT_IN_CHAT,
    ERROR_USER_DONT_HAVE_PERMISSION
} t_status_type;

/*
 * Який тип даних вибрати для поля "password":
 * char: Цей тип даних може зберігати значення від -128 до 127. Якщо ваш алгоритм хешування генерує хеш-суми в цьому діапазоні, то char є кращим вибором.
 * unsigned char: Цей тип даних може зберігати значення від 0 до 255. Якщо ваш алгоритм хешування генерує хеш-суми в цьому діапазоні, то unsigned char є кращим вибором.
 */

// functions from "send_response_to_client.c" file
sqlite3 *db_file_open(void);
bool db_create(void);
t_user_data *db_get_user_data(sqlite3 *db, char *username);

