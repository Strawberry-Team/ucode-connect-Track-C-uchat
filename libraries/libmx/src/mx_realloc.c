#include "libmx.h"

void *mx_realloc(void *ptr, size_t size) {
    if (ptr == NULL)
        return malloc(size);

    if (size == 0) {
        free(ptr);
        return malloc(size);
    }

    size_t previous_size = malloc_size(ptr);

    if (previous_size >= size) {
        return ptr;
    }
    void *result = malloc(size);
    if (result == NULL)
        return NULL;

    mx_memcpy(result, ptr, previous_size);
    free(ptr);
    return result;
}
