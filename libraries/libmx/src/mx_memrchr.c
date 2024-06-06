#include "libmx.h"

void *mx_memrchr(const void *s, int c, size_t n) {
    for (size_t i = n; i > 0; i--)
        if (((unsigned char *) s)[i - 1] == (unsigned char) c)
            return &((unsigned char *) s)[i - 1];
    return NULL;
}
