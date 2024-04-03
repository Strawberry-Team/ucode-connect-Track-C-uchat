// brew install cjson
// COMPILE && RUN:
// cd server/src/ && clang -std=c11 -Wall -Wextra -Werror -Wpedantic cjson_example.c -o cjson_example.o -I /opt/homebrew/include -L /opt/homebrew/lib -lcjson && ./cjson_example.o && cd ../../
// open /opt/homebrew/include/cjson/cJSON.h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

int main(void) {
    // Створюємо об'єкт cJSON
    cJSON *json = cJSON_CreateObject();

    // Додаємо поле типу character
    cJSON_AddStringToObject(json, "name", "Ivan");

    // Додаємо поле типу integer
    cJSON_AddNumberToObject(json, "age", 30);

    // Серіалізуємо об'єкт cJSON у рядок
    char *json_str = cJSON_Print(json);
    printf("\nСеріалізований JSON:\n[%s]\n", json_str);

    // Розпарсовуємо серіалізований JSON назад у об'єкт cJSON
    cJSON *parsed_json = cJSON_Parse(json_str);

    printf("\nОтримуємо значення полів обʼєкта:\n");

    // Отримуємо поле типу character
    cJSON *name_field = cJSON_GetObjectItem(parsed_json, "name");
    if (name_field && name_field->type == cJSON_String) {
        printf("Ім'я: [%s]\n", name_field->valuestring);
    }

    // Отримуємо поле типу integer
    cJSON *age_field = cJSON_GetObjectItem(parsed_json, "age");
    if (age_field && age_field->type == cJSON_Number) {
        printf("Вік: [%d]\n\n", age_field->valueint);
    }

    // Звільняємо пам'ять
    cJSON_Delete(json);
    cJSON_Delete(parsed_json);
    free(json_str);

    return 0;
}
