#pragma once

#include "server.h"
#include <stdio.h>
#include <stdlib.h>
// #include <sqlite3.h> // FOR LINUX
#include <sqlite/include/sqlite3.h> // FOR MAC OS -I /usr/bin/      -I /opt/homebrew/opt/        -L /opt/homebrew/opt/sqlite/lib -lsqlite3


#define DATABASE_FILE "server/database/uchat.db"
#define CREATE_DB_TABLES_FILE "server/database/create_db_tables.sql"
#define USERS_TABLE_COLUMNS_COUNT 4 // id, login(username), password, icon_id

typedef struct s_user_data {
    int id;
    char *username;
    char *password; // якщо "unsigned char *password", то для SHA-256 хеша розмір поля буде 32 байти.
    int icon_id;
} t_user_data;

/*
 * Який тип даних вибрати для поля "password":
 * char: Цей тип даних може зберігати значення від -128 до 127. Якщо ваш алгоритм хешування генерує хеш-суми в цьому діапазоні, то char є кращим вибором.
 * unsigned char: Цей тип даних може зберігати значення від 0 до 255. Якщо ваш алгоритм хешування генерує хеш-суми в цьому діапазоні, то unsigned char є кращим вибором.
 */

// functions from "send_response_to_client.c" file
sqlite3 *db_file_open(void);
bool db_create(void);
t_user_data *db_get_user_data(sqlite3 *db, char *username);

