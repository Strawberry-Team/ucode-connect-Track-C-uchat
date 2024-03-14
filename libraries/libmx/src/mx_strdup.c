#include "libmx.h"

char *mx_strdup(const char *s1) {
    char *dup_str = mx_strnew(mx_strlen(s1));

    if (dup_str == NULL)
        return NULL;

    return mx_strcpy(dup_str, s1);
}
