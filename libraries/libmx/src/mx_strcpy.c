#include "libmx.h"

char *mx_strcpy(char *dst, const char *src) {
    int counter = 0;

    while (src[counter] != '\0') {
        dst[counter] = src[counter];
        counter++;
    }

    dst[counter] = '\0';

    return dst;
}
