#pragma once

#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef __MACH__
#include <sqlite/include/sqlite3.h>
#else
#include <sqlite3.h>
#endif

#define DATABASE_FILE "server/src/database/uchat.db"


bool db_add_user(t_user_data *user_data);
bool db_update_user(t_user_data *user_data);
char *db_get_username(int user_id);
sqlite3 *db_open(void);
t_chat_data *db_add_chat(t_chat_data *chat_data);
t_list *db_get_chat_messages(int chat_id);
t_list *db_get_user_chats(int current_user_id);
t_list *db_get_user_list(int current_user_id);
t_msg_data *db_add_message(t_msg_data *msg_data);
t_msg_data *db_delete_message(t_msg_data *msg_data);
t_msg_data *db_update_message(t_msg_data *msg_data);
t_user_data *db_get_user_data(char *username);
void db_close(sqlite3 *db);
void log_db_error_to_file(char *message, sqlite3 *db);

