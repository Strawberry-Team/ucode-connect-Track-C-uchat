#include "libmx.h"

void *mx_memchr(const void *s, int c, size_t n) {
    for (size_t i = 0; i < n; i++)
        if (((unsigned char *) s)[i] == (unsigned char) c)
            return &((unsigned char *) s)[i];
    return NULL;
}
