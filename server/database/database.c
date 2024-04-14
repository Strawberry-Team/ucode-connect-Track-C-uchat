#include "database.h"

void log_db_error_to_file(char *message, sqlite3 *db) {
    char main_message[300];
    char err_message[150];
    sprintf(err_message, ": [%d | %s]", sqlite3_errcode(db), sqlite3_errmsg(db));
    strcat(main_message, message);
    strcat(main_message, err_message);
    log_to_file(main_message, DB_ERROR);
}

sqlite3 *db_open(void) {
    sqlite3 *db;

    /* Якщо файл бази даних не існує, sqlite3_open() створює новий файл з вказаним ім’ям */
    if (sqlite3_open(DATABASE_FILE, &db) != SQLITE_OK) {
        log_db_error_to_file("Could not open the database", db);
        sqlite3_close(db);
        return NULL;
    }

    log_to_file("Database successfully opened", INFO);
    return db;
}

void db_close(sqlite3 *db) {
    if (sqlite3_close(db) != SQLITE_OK) {
        log_db_error_to_file("Could not close the database", db);
        return;
    }

    log_to_file("Database successfully closed", INFO);
    return;
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
//        log_db_error_to_file("Could not open create the database tables file", db);
//        db_close(db);
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
//            log_db_error_to_file("Could not read the file with SQL for creating database tables", db);
////        mx_strdel(&buffer); // todo change to standart foo?
//            db_close(db);
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
///         log_db_error_to_file("Could not read the file with SQL for creating database tables", db);
//////        mx_strdel(&buffer); // todo change to standart foo?
////        db_close(db);
////        return false;
////    }
//
//    char *err_message = NULL;
//
//    int result = sqlite3_exec(db, buffer, NULL, 0, &err_message);
//
//    if (result != SQLITE_OK) {
//        log_db_error_to_file("Could not execute the SQL for creating database tables", db);
//        sqlite3_free(err_message);
//        db_close(db);
//        return false;
//    }
//
//    db_close(db);
//    log_to_file("Database successfully created", INFO);
//    return true;
//}

t_user_data *db_get_user_data(char *username) {
    sqlite3 *db = db_open();

    if (!db) {
        return NULL;
    }

    t_user_data *user_data = NULL;
    sqlite3_stmt *sql_statement; // An instance of this object represents a single SQL statement that has been compiled into binary form and is ready to be evaluated.

    // або "SELECT users.id, users.username, users.password, users.icon_id "
    const char *sql = "SELECT users.id, users.username, users.password, users.icon_id "
                      "FROM users "
                      "WHERE users.username = ?; ";

    // Підготовка SQL-запиту
    if (sqlite3_prepare_v2(db, sql, -1, &sql_statement, NULL) != SQLITE_OK) {
        log_db_error_to_file("Could not create the prepared statement object", db);
        sqlite3_finalize(sql_statement);
        return NULL;
    }

    // Прив'язка параметрів - прив'язка значення аргументу username до параметра ? в підготовленому SQL-запиті
    if (sqlite3_bind_text(sql_statement, 1, username, -1, NULL) != SQLITE_OK) { // Прив'язує рядок тексту до параметра "?" у запиті "WHERE users.username = ?"
        log_db_error_to_file("Could not bind value to the prepared statement object", db);
        sqlite3_finalize(sql_statement);
        return NULL;
    }

    // виконання підготовлених запитів
    if (sqlite3_step(sql_statement) == SQLITE_ROW) { // Якщо запит успішно виконався і повернув рядок даних
        // перевіряємо кількість стовпців у результуючому наборі, який повертає підготовлений оператор
        if (sqlite3_column_count(sql_statement) != USERS_TABLE_COLUMNS_COUNT) {
            log_db_error_to_file("The prepared operator returns an invalid number of columns in the resulting set", db);
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
        log_db_error_to_file("Could not execute the SQL statement", db);
        sqlite3_finalize(sql_statement);
        return NULL;
    }

    // звільнити ресурси, пов’язані з підготовленим запитом
    sqlite3_finalize(sql_statement);
    db_close(db);
    return user_data;
}

void db_put_user_data(char *username, char *password) {
    sqlite3 *db = db_open();

    if (!db) {
        return;
    }

    int icon_id = 1;
    int created_at = 1;
    int updated_at = 1;
    char *sql_query = sqlite3_mprintf("INSERT INTO users (username, password, icon_id, created_at, updated_at) VALUES('%q', '%q', '%i', '%i', '%i'); ",
                                      username, password, icon_id, created_at, updated_at); // todo нужно ли передавать icon_id
    char *error_msg = NULL;

    if (sqlite3_exec(db, sql_query, NULL, NULL, &error_msg) != SQLITE_OK) {
        log_db_error_to_file("Failed to execute SQL query to put user data", db);
    }

    log_to_file("User data successfully written to the database", INFO);
    sqlite3_free(sql_query);
    sqlite3_free(error_msg);
    db_close(db);
    return;
}

