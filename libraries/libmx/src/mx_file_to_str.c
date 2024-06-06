#include "libmx.h"

static int calculate_length(const char *file) {
    int length = 0;
    char buf[1];

    int fd = open(file, O_RDONLY);
    if (fd == -1)
        return -1;
    int n_read;
    while ((n_read = read(fd, buf, 1)) != 0) {
        if (n_read == -1) {
            close(fd);
            return -1;
        }
        length++;
    }
    if (close(fd) == -1)
        return -1;
    return length;
}

char *mx_file_to_str(const char *file) {
    if (file == NULL)
        return NULL;
    int length = calculate_length(file);
    if (length == -1 || length == 0)
        return NULL;

    char *result = mx_strnew(length);
    if (result == NULL)
        return NULL;

    int fd = open(file, O_RDONLY);
    if (fd == -1) {
        mx_strdel(&result);
        return NULL;
    }

    int i = 0;
    int n_read;
    char buf[1];
    while ((n_read = read(fd, buf, 1)) != 0) {
        if (n_read == -1) {
            mx_strdel(&result);
            return NULL;
        }
        result[i++] = buf[0];
    }
    if (close(fd) == -1) {
        mx_strdel(&result);
        return NULL;
    }
    return result;
}
