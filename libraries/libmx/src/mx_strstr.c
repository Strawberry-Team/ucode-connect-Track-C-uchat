#include "libmx.h"

char *mx_strstr(const char *haystack, const char *needle) {
    if (mx_strlen(needle) == 0)
        return (char *) haystack;
    if (mx_strlen(haystack) < mx_strlen(needle))
        return NULL;
    for (int i = 0; i < mx_strlen(haystack); i++) {
        if (mx_strncmp(&haystack[i], needle, mx_strlen(needle)) == 0) {
            return mx_strchr(&haystack[i], needle[0]);
        }
    }
    return NULL;
}
