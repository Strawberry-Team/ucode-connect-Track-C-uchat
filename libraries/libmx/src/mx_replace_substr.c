#include "libmx.h"

char *mx_replace_substr(const char *str, const char *sub, const char *replace) {
    if (str == NULL || sub == NULL || replace == NULL)
        return NULL;
    if (*sub == '\0') {
        char *result = mx_strnew(mx_strlen(str));
        result = mx_strcpy(result, str);
        return result;
    }
    int substr_number = mx_count_substr(str, sub);
    char *result = mx_strnew(mx_strlen(str) - mx_strlen(sub) * substr_number + mx_strlen(replace) * substr_number);
    char *current_ptr = result;

    char *sub_ptr = mx_strstr(str, sub);
    while (sub_ptr != NULL) {
        mx_strncpy(current_ptr, str, sub_ptr - str);
        current_ptr += sub_ptr - str;
        str = sub_ptr + mx_strlen(sub);
        mx_strcat(current_ptr, replace);
        current_ptr += mx_strlen(replace);
        sub_ptr = mx_strstr(str, sub);
    }
    mx_strcat(current_ptr, str);

    return result;
}
