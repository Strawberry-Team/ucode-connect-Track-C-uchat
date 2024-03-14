#include "libmx.h"

char *mx_del_extra_spaces(const char *str) {
    char *current = mx_strtrim(str);
    if (str == NULL || current == NULL)
        return NULL;
    int counter = 0;
    for (int i = 0; i < mx_strlen(current) - 1; i++)
        if (mx_isspace(current[i]) && mx_isspace(current[i + 1]))
            counter++;
    char *result = mx_strnew(mx_strlen(current) - counter);
    int j = 0;
    for (int i = 0; i < mx_strlen(current); i++) {
        if (mx_isspace(current[i]) && !mx_isspace(current[i + 1])) {
            result[j] = ' ';
            j++;
        } else if (!mx_isspace(current[i])) {
            result[j] = current[i];
            j++;
        }
    }
    mx_strdel(&current);
    return result;
}
