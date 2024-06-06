#include "libmx.h"

char *mx_strjoin(const char *s1, const char *s2) {
    if (s1 == NULL && s2 == NULL) {
        return NULL;
    }
    if (s1 == NULL && s2 != NULL) {
        return mx_strdup(s2);
    }
    if (s1 != NULL && s2 == NULL) {
        return mx_strdup(s1);
    }

    char *result = mx_strnew(mx_strlen(s1) + mx_strlen(s2));
    result = mx_strcpy(result, s1);
    result = mx_strcat(result, s2);
    return result;
}
