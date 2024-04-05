#include "database.h"

sqlite3 *db_file_open(void) {
    sqlite3 *db;

    // Якщо файл бази даних не існує, sqlite3_open() створює новий файл з вказаним ім’ям
    if (sqlite3_open(DATABASE_FILE, &db) != SQLITE_OK) {
        log_to_file("DB: Could not open the database: [%d | %s]", sqlite3_errcode(db), sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    log_to_file("DB: Database successfully opened");
    return db;
}

bool db_create(void) {
    sqlite3 *db = db_open();

    if (!db) {
        return false;
    }

    FILE *sql_file = fopen(CREATE_DB_TABLES_FILE, "r");

    if (!sql_file) {
        log_to_file("DB: Could not create the database tables: [%d | %s]", sqlite3_errcode(db), sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char buffer[BUF_SIZE] = NULL;

    if (fgets(buffer, BUF_SIZE, sql_file) == NULL) {
        log_to_file("DB: Could not read the file with SQL for creating database tables: [%d | %s]", sqlite3_errcode(db), sqlite3_errmsg(db));
        free(buffer);
        sqlite3_close(db);
        return false;
    }

    char *err_message = NULL;

    if (sqlite3_exec(db, buffer, NULL, 0, &err_message) != SQLITE_OK) {
        log_to_file("DB: Could not execute the SQL for creating database tables: [%d | %s | %s]", sqlite3_errcode(db), sqlite3_errmsg(db), err_message); //todo check err_message
        sqlite3_free(err_message);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
}

t_user_data db_get_user_data(sqlite3 *db, char *username) {
    t_user_data *user_data = NULL;
    sqlite3_stmt *sql_statement; // An instance of this object represents a single SQL statement that has been compiled into binary form and is ready to be evaluated.

    // або "SELECT users.id, users.username, users.password, users.icon_id "
    const char *sql = "SELECT * "
                      "FROM users "
                      "WHERE users.username = ?; ";
    // Підготовка SQL-запиту
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        log_to_file("DB: Could not create the prepared statement object: [%d | %s]", sqlite3_errcode(db), sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return NULL;
    }

    // Прив'язка параметрів - прив'язка значення аргументу username до параметра ? в підготовленому SQL-запиті
    if (sqlite3_bind_text(stmt, 1, username, -1, NULL) != SQLITE_OK) { // Прив'язує рядок тексту до параметра "?" у запиті "WHERE users.username = ?"
        log_to_file("DB: Could not bind value to the prepared statement object: [%d | %s]", sqlite3_errcode(db), sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return NULL;
    }

    // виконання підготовлених запитів
    if (sqlite3_step(stmt) == SQLITE_ROW) { // Якщо запит успішно виконався і повернув рядок даних
        // перевіряємо кількість стовпців у результуючому наборі, який повертає підготовлений оператор
        if (sqlite3_column_count(sql_statement) != USERS_TABLE_COLUMNS_COUNT) {
            log_to_file("DB: The prepared operator returns an invalid number of columns in the resulting set: [%d | %s]", sqlite3_errcode(db), sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return NULL;
        }

        // записуємо значення з цього рядка за допомогою функцій доступу до стовпців
        user_data = (t_user_data *) malloc(sizeof(t_user_data));
        user_data->id = sqlite3_column_int(stmt, 0);
        user_data->username = strdup((const char *) sqlite3_column_text(stmt, 1)); // or mx_strdup()
        user_data->password = (char *) strdup((const char *)sqlite3_column_text(stmt, 2)); // or mx_strdup()
        user_data->icon_id = sqlite3_column_int(stmt, 3);
    } else {
        log_to_file("DB: Could not execute the SQL statement: [%d | %s]", sqlite3_errcode(db), sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return NULL;
    }

    // звільнити ресурси, пов’язані з підготовленим запитом
    sqlite3_finalize(stmt);
    return user_data;
}

