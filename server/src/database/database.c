#include "database.h"

void log_db_error_to_file(char *message, sqlite3 *db) {
    char *main_message = (char *) malloc(300 * sizeof(char));
    char *err_message = (char *) malloc(150 * sizeof(char));
    sprintf(err_message, ": [%d | %s]", sqlite3_errcode(db), sqlite3_errmsg(db));
    strcat(main_message, message);
    strcat(main_message, err_message);
    log_to_file(main_message, DB_ERROR);
    free(err_message);
    free(main_message);
}

sqlite3 *db_open(void) {
    sqlite3 *db;

    if (sqlite3_open(DATABASE_FILE, &db) != SQLITE_OK) {
        log_db_error_to_file("Could not open the database", db);
        sqlite3_close(db);
        return NULL;
    }

    return db;
}

void db_close(sqlite3 *db) {
    if (sqlite3_close(db) != SQLITE_OK) {
        log_db_error_to_file("Could not close the database", db);
        return;
    }

    return;
}

t_user_data *db_get_user_data(char *username) {
    sqlite3 *db = db_open();

    if (!db) {
        return NULL;
    }

    sqlite3_stmt *statement;
    const char *sql_get_user = "SELECT id, username, password "
                               "FROM users "
                               "WHERE username = ? ; ";

    if (sqlite3_prepare_v2(db, sql_get_user, -1, &statement, NULL) != SQLITE_OK) {
        log_db_error_to_file("Could not create the prepared statement object", db);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    if (sqlite3_bind_text(statement, 1, username, -1, NULL) != SQLITE_OK) {
        log_db_error_to_file("Could not bind value to the prepared statement object", db);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    t_user_data *user_data = NULL;

    if (sqlite3_step(statement) == SQLITE_ROW) {
        if (sqlite3_column_count(statement) != 3) {
            log_db_error_to_file("The prepared operator returns an invalid number of columns in the resulting set", db);
            sqlite3_finalize(statement);
            db_close(db);
            return NULL;
        }

        user_data = (t_user_data *) malloc(sizeof(t_user_data));
        user_data->id = sqlite3_column_int(statement, 0);
        user_data->username = mx_strdup((const char *) sqlite3_column_text(statement, 1));
        user_data->password = mx_strdup((const char *) sqlite3_column_text(statement, 2));
    } else {
        log_db_error_to_file("Could not execute the SQL statement", db);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    sqlite3_finalize(statement);
    db_close(db);
    return user_data;
}

bool db_add_user(t_user_data *user_data) {
    sqlite3 *db = db_open();

    if (!db) {
        return false;
    }

    char *sql_add_user = sqlite3_mprintf("INSERT INTO users (username, password, created_at) "
                                      "VALUES ('%q', '%q', '%ld'); ",
                                      user_data->username, user_data->password, user_data->created_at);

    if (sqlite3_exec(db, sql_add_user, NULL, NULL, NULL) != SQLITE_OK) {
        log_db_error_to_file("Failed to execute SQL query to write user data", db);
        sqlite3_free(sql_add_user);
        db_close(db);
        return false;
    }

    sqlite3_free(sql_add_user);
    db_close(db);
    return true;
}

bool db_update_user(t_user_data *user_data) {
    sqlite3 *db = db_open();

    if (!db) {
        return false;
    }

    char *sql_upd_user = sqlite3_mprintf("UPDATE users "
                                         "SET password = '%q', updated_at = '%li' "
                                         "WHERE id = '%i' ; ",
                                         user_data->password, user_data->updated_at, user_data->id);

    if (sqlite3_exec(db, sql_upd_user, NULL, NULL, NULL) != SQLITE_OK) {
        log_db_error_to_file("Failed to execute SQL query to update user data", db);
        sqlite3_free(sql_upd_user);
        db_close(db);
        return false;
    }

    sqlite3_free(sql_upd_user);
    db_close(db);
    return true;
}

t_list *db_get_user_list(int current_user_id) {
    sqlite3 *db = db_open();

    if (!db) {
        return NULL;
    }

    sqlite3_stmt *statement;
    const char *sql_get_user_list = "WITH current_user_chats AS ( "
                      "SELECT DISTINCT chat_id "
                      "FROM users_chats "
                      "WHERE user_id = ? ), "
                      "chats_with_users AS ( "
                      "SELECT user_id "
                      "FROM users_chats AS uc "
                      "INNER JOIN current_user_chats AS cuc ON cuc.chat_id = uc.chat_id ) "
                      "SELECT u.id, u.username "
                      "FROM users AS u "
                      "LEFT JOIN chats_with_users AS cwu ON cwu.user_id = u.id "
                      "WHERE cwu.user_id IS NULL AND u.id != ? ; ";

    if (sqlite3_prepare_v2(db, sql_get_user_list, -1, &statement, NULL) != SQLITE_OK) {
        log_db_error_to_file("Could not create the prepared statement object", db);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    if (sqlite3_bind_int(statement, 1, current_user_id) != SQLITE_OK
        || sqlite3_bind_int(statement, 2, current_user_id) != SQLITE_OK) {
        log_db_error_to_file("Could not bind value to the prepared statement object", db);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    t_list *user_list = NULL;

    while (sqlite3_step(statement) == SQLITE_ROW) {
        if (sqlite3_column_count(statement) != 2) {
            log_db_error_to_file("The prepared operator returns an invalid number of columns in the resulting set", db);
            sqlite3_finalize(statement);
            db_close(db);
            return NULL;
        }

        t_user_data *user_data = (t_user_data *) malloc(sizeof(t_user_data));
        user_data->id = sqlite3_column_int(statement, 0);
        user_data->username = mx_strdup((const char *) sqlite3_column_text(statement, 1));
        mx_push_back(&user_list, user_data);
    }

    sqlite3_finalize(statement);
    db_close(db);
    return user_list;
}

char *db_get_username(int user_id) {
    sqlite3 *db = db_open();

    if (!db) {
        return NULL;
    }

    sqlite3_stmt *statement;
    const char *sql_get_username = "SELECT username "
                              "FROM users "
                              "WHERE id = ? ; ";

    if (sqlite3_prepare_v2(db, sql_get_username, -1, &statement, NULL) != SQLITE_OK) {
        log_db_error_to_file("Could not create the prepared statement object", db);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    if (sqlite3_bind_int(statement, 1, user_id) != SQLITE_OK) {
        log_db_error_to_file("Could not bind value to the prepared statement object", db);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    char *username;

    if (sqlite3_step(statement) == SQLITE_ROW) {
        if (sqlite3_column_count(statement) != 1) {
            log_db_error_to_file("The prepared operator returns an invalid number of columns in the resulting set", db);
            sqlite3_finalize(statement);
            db_close(db);
            return NULL;
        }

        username = mx_strdup((const char *) sqlite3_column_text(statement, 0));
    } else {
        log_db_error_to_file("Could not execute the SQL statement", db);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    sqlite3_finalize(statement);
    db_close(db);
    return username;
}

t_chat_data *db_add_chat(t_chat_data *chat_data) {
    sqlite3 *db = db_open();

    if (!db) {
        return NULL;
    }

    sqlite3_stmt *stmt;
    const char *sql_check_chat = "SELECT id "
                                 "FROM users_chats "
                                 "WHERE chat_id IN ( "
                                 "SELECT DISTINCT chat_id "
                                 "FROM users_chats "
                                 "WHERE user_id = ? ) "
                                 "AND user_id = ? ; ";

    if (sqlite3_prepare_v2(db, sql_check_chat, -1, &stmt, NULL) != SQLITE_OK) {
        log_db_error_to_file("Could not create the prepared statement object", db);
        sqlite3_finalize(stmt);
        db_close(db);
        return NULL;
    }

    if (sqlite3_bind_int(stmt, 1, chat_data->current_user.id) != SQLITE_OK
        || sqlite3_bind_int(stmt, 2, chat_data->invitee_user.id) != SQLITE_OK) {
        log_db_error_to_file("Could not bind value to the prepared statement object", db);
        sqlite3_finalize(stmt);
        db_close(db);
        return NULL;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        log_db_error_to_file("Chat with the selected user already exists", db);
        sqlite3_finalize(stmt);
        db_close(db);
        return NULL;
    }

    sqlite3_finalize(stmt);

    char *sql_add_chat = sqlite3_mprintf("INSERT INTO chats (title, created_at) "
                                      "VALUES ('%Q', '%li'); ",
                                      chat_data->title, chat_data->created_at);

    if (sqlite3_exec(db, sql_add_chat, NULL, NULL, NULL) != SQLITE_OK) {
        log_db_error_to_file("Failed to execute SQL query to create chat", db);
        sqlite3_free(sql_add_chat);
        db_close(db);
        return NULL;
    }

    chat_data->id = sqlite3_last_insert_rowid(db);
    char *sql_add_users_to_chat = sqlite3_mprintf("INSERT INTO users_chats (chat_id, user_id, created_at) "
                                          "VALUES ('%i', '%i', '%li'); "
                                          "INSERT INTO users_chats (chat_id, user_id, created_at) "
                                          "VALUES ('%i', '%i', '%li'); ",
                                          chat_data->id, chat_data->current_user.id, chat_data->current_user.created_at,
                                          chat_data->id, chat_data->invitee_user.id, chat_data->invitee_user.created_at);

    if (sqlite3_exec(db, sql_add_users_to_chat, NULL, NULL, NULL) != SQLITE_OK) {
        log_db_error_to_file("Failed to execute SQL query to add users to chat", db);
        sqlite3_free(sql_add_chat);
        sqlite3_free(sql_add_users_to_chat);
        db_close(db);
        return NULL;
    }

    sqlite3_stmt *statement;
    const char *sql_get_chat = "WITH user_chats AS ( "
                              "SELECT DISTINCT chat_id "
                              "FROM users_chats "
                              "WHERE user_id = ? ), "
                              "chat_titles AS ( "
                              "SELECT DISTINCT c.chat_id, u.username AS title "
                              "FROM users_chats AS uc "
                              "INNER JOIN user_chats AS c ON c.chat_id = uc.chat_id "
                              "LEFT JOIN users AS u ON u.id = uc.user_id "
                              "WHERE uc.user_id != ? "
                              "AND u.username IS NOT NULL ) "
                              "SELECT c.id, ct.title "
                              "FROM user_chats AS uc "
                              "INNER JOIN chats AS c ON c.id = uc.chat_id "
                              "LEFT JOIN chat_titles AS ct ON ct.chat_id = uc.chat_id "
                              "WHERE c.id = ? ; ";

    if (sqlite3_prepare_v2(db, sql_get_chat, -1, &statement, NULL) != SQLITE_OK) {
        log_db_error_to_file("Could not create the prepared statement object", db);
        sqlite3_free(sql_add_chat);
        sqlite3_free(sql_add_users_to_chat);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    if (sqlite3_bind_int(statement, 1, chat_data->current_user.id) != SQLITE_OK
        || sqlite3_bind_int(statement, 2, chat_data->current_user.id) != SQLITE_OK
        || sqlite3_bind_int(statement, 3, chat_data->id) != SQLITE_OK) {
        log_db_error_to_file("Could not bind value to the prepared statement object", db);
        sqlite3_free(sql_add_chat);
        sqlite3_free(sql_add_users_to_chat);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    t_chat_data *new_chat_data = NULL;

    if (sqlite3_step(statement) == SQLITE_ROW) {
        if (sqlite3_column_count(statement) != 2) {
            log_db_error_to_file("The prepared operator returns an invalid number of columns in the resulting set", db);
            sqlite3_free(sql_add_chat);
            sqlite3_free(sql_add_users_to_chat);
            sqlite3_finalize(statement);
            db_close(db);
            return NULL;
        }

        new_chat_data = (t_chat_data *) malloc(sizeof(t_chat_data));
        new_chat_data->id = sqlite3_column_int(statement, 0);
        new_chat_data->title = mx_strdup((const char *) sqlite3_column_text(statement, 1));
    } else {
        log_db_error_to_file("Could not execute the SQL statement", db);
        sqlite3_free(sql_add_chat);
        sqlite3_free(sql_add_users_to_chat);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    sqlite3_free(sql_add_chat);
    sqlite3_free(sql_add_users_to_chat);
    sqlite3_finalize(statement);
    db_close(db);
    return new_chat_data;
}

t_list *db_get_user_chats(int current_user_id) {
    sqlite3 *db = db_open();

    if (!db) {
        return NULL;
    }

    sqlite3_stmt *statement;
    const char *sql_get_chats = "WITH user_chats AS ( "
                                "SELECT DISTINCT chat_id "
                                "FROM users_chats "
                                "WHERE user_id = ? ), "
                                "chat_titles AS ( "
                                "SELECT DISTINCT c.chat_id, u.username AS title "
                                "FROM users_chats AS uc "
                                "INNER JOIN user_chats AS c ON c.chat_id = uc.chat_id "
                                "LEFT JOIN users AS u ON u.id = uc.user_id "
                                "WHERE uc.user_id != ? "
                                "AND u.username IS NOT NULL) "
                                "SELECT c.id, ct.title "
                                "FROM user_chats AS uc "
                                "INNER JOIN chats AS c ON c.id = uc.chat_id "
                                "LEFT JOIN chat_titles AS ct ON ct.chat_id = uc.chat_id "
                                "ORDER BY c.created_at; ";

    if (sqlite3_prepare_v2(db, sql_get_chats, -1, &statement, NULL) != SQLITE_OK) {
        log_db_error_to_file("Could not create the prepared statement object", db);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    if (sqlite3_bind_int(statement, 1, current_user_id) != SQLITE_OK
        || sqlite3_bind_int(statement, 2, current_user_id) != SQLITE_OK) {
        log_db_error_to_file("Could not bind value to the prepared statement object", db);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    t_list *chat_list = NULL;

    while (sqlite3_step(statement) == SQLITE_ROW) {
        if (sqlite3_column_count(statement) != 2) {
            log_db_error_to_file("The prepared operator returns an invalid number of columns in the resulting set", db);
            sqlite3_finalize(statement);
            db_close(db);
            return NULL;
        }

        t_chat_data *chat_data = (t_chat_data *) malloc(sizeof(t_chat_data));
        chat_data->id = sqlite3_column_int(statement, 0);
        chat_data->title = mx_strdup((const char *) sqlite3_column_text(statement, 1));
        mx_push_back(&chat_list, chat_data);
    }

    sqlite3_finalize(statement);
    db_close(db);
    return chat_list;
}

t_list *db_get_chat_messages(int chat_id) {
    sqlite3 *db = db_open();

    if (!db) {
        return NULL;
    }

    sqlite3_stmt *statement;
    const char *sql_get_msgs = "SELECT m.id, m.user_id, u.username AS user_username, m.chat_id, m.body, m.created_at, m.updated_at, m.deleted_at "
                               "FROM messages AS m LEFT JOIN users u ON m.user_id = u.id "
                               "WHERE m.chat_id = ? ORDER BY m.created_at; ";

    if (sqlite3_prepare_v2(db, sql_get_msgs, -1, &statement, NULL) != SQLITE_OK) {
        log_db_error_to_file("Could not create the prepared statement object", db);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    if (sqlite3_bind_int(statement, 1, chat_id) != SQLITE_OK) {
        log_db_error_to_file("Could not bind value to the prepared statement object", db);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    t_list *message_list = NULL;

    while (sqlite3_step(statement) == SQLITE_ROW) {
        if (sqlite3_column_count(statement) != 8) {
            log_db_error_to_file("The prepared operator returns an invalid number of columns in the resulting set", db);
            sqlite3_finalize(statement);
            db_close(db);
            return NULL;
        }

        t_msg_data *msg_data = (t_msg_data *) malloc(sizeof(t_msg_data));
        msg_data->message_id = sqlite3_column_int(statement, 0);
        msg_data->user_id = sqlite3_column_int(statement, 1);
        msg_data->username = mx_strdup((const char *) sqlite3_column_text(statement, 2));
        msg_data->chat_id = sqlite3_column_int(statement, 3);
        msg_data->body = mx_strdup((const char *) sqlite3_column_text(statement, 4));
        msg_data->created_at = sqlite3_column_int64(statement, 5);
        msg_data->updated_at = sqlite3_column_int64(statement, 6);
        msg_data->deleted_at = sqlite3_column_int64(statement, 7);
        mx_push_back(&message_list, msg_data);
    }

    sqlite3_finalize(statement);
    db_close(db);
    return message_list;
}

t_msg_data *db_add_message(t_msg_data *msg_data) {
    sqlite3 *db = db_open();

    if (!db) {
        return NULL;
    }

    char *sql_add_msg = sqlite3_mprintf("INSERT INTO messages (user_id, chat_id, body, created_at, updated_at, deleted_at) "
                                        "VALUES ('%i', '%i', '%q', '%li', '%Q', '%Q') ; ",
                                        msg_data->user_id, msg_data->chat_id, msg_data->body, msg_data->created_at, NULL, NULL);

    if (sqlite3_exec(db, sql_add_msg, NULL, NULL, NULL) != SQLITE_OK) {
        log_db_error_to_file("Failed to execute SQL query to write message", db);
        sqlite3_free(sql_add_msg);
        db_close(db);
        return NULL;
    }

    msg_data->message_id = sqlite3_last_insert_rowid(db);
    sqlite3_stmt *statement;
    const char *sql_get_msg = "SELECT m.id, m.user_id, u.username AS user_username, m.chat_id, m.body, "
                              "m.created_at, m.updated_at, m.deleted_at "
                              "FROM messages AS m "
                              "LEFT JOIN users u "
                              "ON m.user_id = u.id "
                              "WHERE m.id = ? ; ";

    if (sqlite3_prepare_v2(db, sql_get_msg, -1, &statement, NULL) != SQLITE_OK) {
        log_db_error_to_file("Could not create the prepared statement object", db);
        sqlite3_free(sql_add_msg);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    if (sqlite3_bind_int(statement, 1, msg_data->message_id) != SQLITE_OK) {
        log_db_error_to_file("Could not bind value to the prepared statement object", db);
        sqlite3_free(sql_add_msg);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    t_msg_data *new_msg_data = NULL;

    if (sqlite3_step(statement) == SQLITE_ROW) {
        if (sqlite3_column_count(statement) != 8) {
            log_db_error_to_file("The prepared operator returns an invalid number of columns in the resulting set", db);
            sqlite3_free(sql_add_msg);
            sqlite3_finalize(statement);
            db_close(db);
            return NULL;
        }

        new_msg_data = (t_msg_data *) malloc(sizeof(t_msg_data));
        new_msg_data->message_id = sqlite3_column_int(statement, 0);
        new_msg_data->user_id = sqlite3_column_int(statement, 1);
        new_msg_data->username = mx_strdup((const char *) sqlite3_column_text(statement, 2));
        new_msg_data->chat_id = sqlite3_column_int(statement, 3);
        new_msg_data->body = mx_strdup((const char *) sqlite3_column_text(statement, 4));
        new_msg_data->created_at = sqlite3_column_int64(statement, 5);
        new_msg_data->updated_at = sqlite3_column_int64(statement, 6);
        new_msg_data->deleted_at = sqlite3_column_int64(statement, 7);
    } else {
        log_db_error_to_file("Could not execute the SQL statement", db);
        sqlite3_free(sql_add_msg);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    sqlite3_free(sql_add_msg);
    sqlite3_finalize(statement);
    db_close(db);
    return new_msg_data;
}

t_msg_data *db_update_message(t_msg_data *msg_data) {
    sqlite3 *db = db_open();

    if (!db) {
        return NULL;
    }

    char *sql_upd_msg = sqlite3_mprintf("UPDATE messages "
                                         "SET body = '%q' , updated_at = '%li' "
                                         "WHERE id = '%i' ; ",
                                         msg_data->body, msg_data->updated_at, msg_data->message_id);

    if (sqlite3_exec(db, sql_upd_msg, NULL, NULL, NULL) != SQLITE_OK) {
        log_db_error_to_file("Failed to execute SQL query to update message", db);
        sqlite3_free(sql_upd_msg);
        db_close(db);
        return NULL;
    }

    sqlite3_stmt *statement;
    const char *sql_get_msg = "SELECT m.id, m.user_id, u.username AS user_username, m.chat_id, m.body, m.created_at, m.updated_at, m.deleted_at "
                              "FROM messages AS m "
                              "LEFT JOIN users u ON m.user_id = u.id "
                              "WHERE m.id = ? ; ";

    if (sqlite3_prepare_v2(db, sql_get_msg, -1, &statement, NULL) != SQLITE_OK) {
        log_db_error_to_file("Could not create the prepared statement object", db);
        sqlite3_free(sql_upd_msg);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    if (sqlite3_bind_int(statement, 1, msg_data->message_id) != SQLITE_OK) {
        log_db_error_to_file("Could not bind value to the prepared statement object", db);
        sqlite3_free(sql_upd_msg);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    t_msg_data *new_msg_data = NULL;

    if (sqlite3_step(statement) == SQLITE_ROW) {
        if (sqlite3_column_count(statement) != 8) {
            log_db_error_to_file("The prepared operator returns an invalid number of columns in the resulting set", db);
            sqlite3_free(sql_upd_msg);
            sqlite3_finalize(statement);
            db_close(db);
            return NULL;
        }

        new_msg_data = (t_msg_data *) malloc(sizeof(t_msg_data));
        new_msg_data->message_id = sqlite3_column_int(statement, 0);
        new_msg_data->user_id = sqlite3_column_int(statement, 1);
        new_msg_data->username = mx_strdup((const char *) sqlite3_column_text(statement, 2));
        new_msg_data->chat_id = sqlite3_column_int(statement, 3);
        new_msg_data->body = mx_strdup((const char *) sqlite3_column_text(statement, 4));
        new_msg_data->created_at = sqlite3_column_int64(statement, 5);
        new_msg_data->updated_at = sqlite3_column_int64(statement, 6);
        new_msg_data->deleted_at = sqlite3_column_int64(statement, 7);
    } else {
        log_db_error_to_file("Could not execute the SQL statement", db);
        sqlite3_free(sql_upd_msg);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    sqlite3_free(sql_upd_msg);
    sqlite3_finalize(statement);
    db_close(db);
    return new_msg_data;
}

t_msg_data *db_delete_message(t_msg_data *msg_data) {
    sqlite3 *db = db_open();

    if (!db) {
        return NULL;
    }

    char *sql_upd_msg = sqlite3_mprintf("UPDATE messages "
                                        "SET deleted_at = %li "
                                        "WHERE id = %i ; ",
                                        msg_data->deleted_at, msg_data->message_id);

    if (sqlite3_exec(db, sql_upd_msg, NULL, NULL, NULL) != SQLITE_OK) {
        log_db_error_to_file("Failed to execute SQL query to delete message", db);
        sqlite3_free(sql_upd_msg);
        db_close(db);
        return NULL;
    }

    sqlite3_stmt *statement;
    const char *sql_get_msg = "SELECT m.id, m.user_id, u.username AS user_username, m.chat_id, m.body, m.created_at, m.updated_at, m.deleted_at "
                              "FROM messages AS m "
                              "LEFT JOIN users u ON m.user_id = u.id "
                              "WHERE m.id = ? ; ";

    if (sqlite3_prepare_v2(db, sql_get_msg, -1, &statement, NULL) != SQLITE_OK) {
        log_db_error_to_file("Could not create the prepared statement object", db);
        sqlite3_free(sql_upd_msg);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    if (sqlite3_bind_int(statement, 1, msg_data->message_id) != SQLITE_OK) {
        log_db_error_to_file("Could not bind value to the prepared statement object", db);
        sqlite3_free(sql_upd_msg);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    t_msg_data *new_msg_data = NULL;

    if (sqlite3_step(statement) == SQLITE_ROW) {
        if (sqlite3_column_count(statement) != 8) {
            log_db_error_to_file("The prepared operator returns an invalid number of columns in the resulting set", db);
            sqlite3_free(sql_upd_msg);
            sqlite3_finalize(statement);
            db_close(db);
            return NULL;
        }

        new_msg_data = (t_msg_data *) malloc(sizeof(t_msg_data));
        new_msg_data->message_id = sqlite3_column_int(statement, 0);
        new_msg_data->user_id = sqlite3_column_int(statement, 1);
        new_msg_data->username = mx_strdup((const char *) sqlite3_column_text(statement, 2));
        new_msg_data->chat_id = sqlite3_column_int(statement, 3);
        new_msg_data->body = mx_strdup((const char *) sqlite3_column_text(statement, 4));
        new_msg_data->created_at = sqlite3_column_int64(statement, 5);
        new_msg_data->updated_at = sqlite3_column_int64(statement, 6);
        new_msg_data->deleted_at = sqlite3_column_int64(statement, 7);
    } else {
        log_db_error_to_file("Could not execute the SQL statement", db);
        sqlite3_free(sql_upd_msg);
        sqlite3_finalize(statement);
        db_close(db);
        return NULL;
    }

    sqlite3_free(sql_upd_msg);
    sqlite3_finalize(statement);
    db_close(db);
    return new_msg_data;
}

