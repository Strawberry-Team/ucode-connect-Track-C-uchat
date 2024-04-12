#include "database.h"

sqlite3 *db_file_open(void) {
    sqlite3 *db;

    /* Якщо файл бази даних не існує, sqlite3_open() створює новий файл з вказаним ім’ям */
    if (sqlite3_open(DATABASE_FILE, &db) != SQLITE_OK) {
        char msg[200];
        sprintf(msg, "Could not open the database: [%s]", sqlite3_errmsg(db));
        log_to_file(msg, ERROR);
        sqlite3_close(db);
        return NULL;
    }

    log_to_file("Database successfully opened", INFO);
//    log_db_error_to_file("Database successfully opened", db);
    return db;
}

//bool db_create(void) {
//    sqlite3 *db = db_file_open();
//
//    if (!db) {
//        return false;
//    }
//
//    FILE *sql_file = fopen(CREATE_DB_TABLES_FILE, "r");
//
//    if (!sql_file) {
//        char msg[200];
//        sprintf(msg, "Could not open create the database tables file: [%d | %s]", sqlite3_errcode(db), sqlite3_errmsg(db));
//        log_to_file(msg, ERROR);
//        sqlite3_close(db);
//        return false;
//    }
//
//    char buffer[BUF_SIZE];
//    memset(buffer, 0, sizeof(buffer));
////    int i;
////    char character;
////
////    for (i = 0; (character = fgetc(sql_file)) != EOF; i++) {
////        buffer[i] = character;
////    }
////
////    buffer[i + 1] = '\0';
//
//    char *string;
//    // Read each line and accumulate into statement until a semicolon (;) is found
//    while ((string = fgets(buffer, BUF_SIZE, sql_file)) != NULL) {
////        if (character == '\n') {
////            continue; // Skip empty lines
////        }
//
//        if (!string) {
//            char msg[200];
//            sprintf(msg, "Could not read the file with SQL for creating database tables: [%d | %s]", sqlite3_errcode(db), sqlite3_errmsg(db));
//            log_to_file(msg, ERROR);
////        mx_strdel(&buffer); // todo change to standart foo?
//            sqlite3_close(db);
//            return false;
//        }
//
//        strcat(buffer, string); // Append character to statement
//        memset(string, 0, strlen(string)); // Clear statement for next line
//    }
//
//
////    if (fgets(buffer, BUF_SIZE, sql_file) == NULL) {
////    if (strlen(buffer) == 0) {
////        char msg[200];
////        sprintf(msg, "Could not read the file with SQL for creating database tables: [%d | %s]", sqlite3_errcode(db), sqlite3_errmsg(db));
////        log_to_file(msg, ERROR);
//////        mx_strdel(&buffer); // todo change to standart foo?
////        sqlite3_close(db);
////        return false;
////    }
//
//    char *err_message = NULL;
//
//    int result = sqlite3_exec(db, buffer, NULL, 0, &err_message);
//    char msg[200];
//    sprintf(msg, "sqlite3_exec == %d", result);
//    log_to_file(msg, ERROR);
//
//    if (result != SQLITE_OK) {
//        char msg[500];
//        sprintf(msg, "res = %d | Could not execute the SQL for creating database tables: [%d | %s | %s]", result, sqlite3_errcode(db), sqlite3_errmsg(db), err_message);
//        log_to_file(msg, ERROR);
////        log_to_file("Could not execute the SQL for creating database tables: [%d | %s | %s]", sqlite3_errcode(db), sqlite3_errmsg(db), err_message); //todo check err_message
//        sqlite3_free(err_message);
//        sqlite3_close(db);
//        return false;
//    }
//
//    sqlite3_close(db);
//    log_to_file("Database successfully created", INFO);
//    return true;
//}

t_user_data *db_get_user_data(sqlite3 *db, char *username) {
    t_user_data *user_data = NULL;
    sqlite3_stmt *sql_statement; // An instance of this object represents a single SQL statement that has been compiled into binary form and is ready to be evaluated.

    // або "SELECT users.id, users.username, users.password, users.icon_id "
    const char *sql = "SELECT users.id, users.username, users.password, users.icon_id "
                      "FROM users "
                      "WHERE users.username = ?; ";

    // Підготовка SQL-запиту
    if (sqlite3_prepare_v2(db, sql, -1, &sql_statement, NULL) != SQLITE_OK) {
        char msg[200];
        sprintf(msg, "Could not create the prepared statement object: [%d | %s]", sqlite3_errcode(db), sqlite3_errmsg(db));
        log_to_file(msg, ERROR);
        sqlite3_finalize(sql_statement);
        return NULL;
    }

    // Прив'язка параметрів - прив'язка значення аргументу username до параметра ? в підготовленому SQL-запиті
    if (sqlite3_bind_text(sql_statement, 1, username, -1, NULL) != SQLITE_OK) { // Прив'язує рядок тексту до параметра "?" у запиті "WHERE users.username = ?"
        char msg[200];
        sprintf(msg, "Could not bind value to the prepared statement object: [%d | %s]", sqlite3_errcode(db), sqlite3_errmsg(db));
        log_to_file(msg, ERROR);
        sqlite3_finalize(sql_statement);
        return NULL;
    }

    // виконання підготовлених запитів
    if (sqlite3_step(sql_statement) == SQLITE_ROW) { // Якщо запит успішно виконався і повернув рядок даних
        // перевіряємо кількість стовпців у результуючому наборі, який повертає підготовлений оператор
        if (sqlite3_column_count(sql_statement) != USERS_TABLE_COLUMNS_COUNT) {
            char msg[200];
            sprintf(msg, "The prepared operator returns an invalid number of columns in the resulting set: [%d | %s]", sqlite3_errcode(db), sqlite3_errmsg(db));
            log_to_file(msg, ERROR);
            sqlite3_finalize(sql_statement);
            return NULL;
        }

        // записуємо значення з цього рядка за допомогою функцій доступу до стовпців
        user_data = (t_user_data *) malloc(sizeof(t_user_data));
        user_data->id = sqlite3_column_int(sql_statement, 0);
        user_data->username = mx_strdup((const char *) sqlite3_column_text(sql_statement, 1)); // or mx_strdup()
        user_data->password = (char *) mx_strdup((const char *)sqlite3_column_text(sql_statement, 2)); // or mx_strdup()
        user_data->icon_id = sqlite3_column_int(sql_statement, 3);
    } else {
        char msg[200];
        sprintf(msg, "Could not execute the SQL statement: [%d | %s]", sqlite3_errcode(db), sqlite3_errmsg(db));
        log_to_file(msg, ERROR);
        sqlite3_finalize(sql_statement);
        return NULL;
    }

    // звільнити ресурси, пов’язані з підготовленим запитом
    sqlite3_finalize(sql_statement);
    return user_data;
}

