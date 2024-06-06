#include "libmx.h"

char *mx_delim_strjoin(const char *str1, const char *str2, char delim) {
    if (str1 == NULL)
        return mx_strdup(str2);
    int length1 = mx_strlen(str1);
    int length2 = mx_strlen(str2);
    char *result = mx_strnew(length1 + length2 + 1);
    mx_strcpy(result, str1);
    result[length1] = delim;
    mx_strcpy(result + length1 + 1, str2);
    return result;
}
