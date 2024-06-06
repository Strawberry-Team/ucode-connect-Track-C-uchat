#include "libmx.h"

void *mx_memmem(const void *big, size_t big_len, const void *little, size_t little_len) {
    if (big_len <= 0 || little_len <= 0 || big_len < little_len)
        return NULL;

    void *ptr_result = mx_memchr(big, *(unsigned char *) little, big_len);
    while (ptr_result != NULL) {
        size_t remaining = big_len - ((unsigned char *) ptr_result - (unsigned char *) big);

        if (remaining < little_len) {
            break;
        }
        if (mx_memcmp(ptr_result, little, little_len) == 0) {
            return ptr_result;
        }

        ptr_result = mx_memchr((unsigned char *) ptr_result + 1, *(unsigned char *) little, big_len);
    }

    return NULL;
}
