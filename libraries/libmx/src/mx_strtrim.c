#include "libmx.h"

char *mx_strtrim(const char *str) {
    if (str == NULL)
        return NULL;
    while (mx_isspace(*str))
        str++;
    int length = mx_strlen(str);
    if (length != 0)
        while (mx_isspace(str[length - 1]))
            length--;
    char *result = mx_strnew(length);
    return mx_strncpy(result, str, length);
}
