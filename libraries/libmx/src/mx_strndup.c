#include "libmx.h"

char *mx_strndup(const char *s1, size_t n) {
    size_t size = mx_strlen(s1);

    if (n < size)
        size = n;

    char *dup_str = mx_strnew(size);

    if (dup_str == NULL)
        return NULL;

    return mx_strncpy(dup_str, s1, size);
}
