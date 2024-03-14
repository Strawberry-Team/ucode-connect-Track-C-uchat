#include "libmx.h"

int mx_read_line(char **lineptr, size_t buf_size, char delim, const int fd) {
    if (buf_size < 1 || buf_size > 2147483647 || fd <= 0 || lineptr == NULL)
        return -2;

    static char *buf_remainder = NULL;
    char *str = NULL;
    int size_result = 0;
    if (buf_remainder != NULL) {
        str = buf_remainder;
        char *closest_delim = mx_strchr(buf_remainder, delim);

        if (closest_delim != NULL) {
            *closest_delim = '\0';
            buf_remainder = mx_strdup(closest_delim + 1);
        } else
            buf_remainder = NULL;


        char *temp_ptr = NULL;
        size_result = mx_strlen(str);
        temp_ptr = str;
        str = mx_strndup(str, size_result);
        mx_strdel(&temp_ptr);

        if (buf_remainder != NULL) {
            if (mx_strlen(buf_remainder) == 0) {
                mx_strdel(&buf_remainder);
            }
            *lineptr = str;
            return size_result;
        }
    }

    char *buf = mx_strnew(buf_size);
    ssize_t n_read = -1;
    while ((n_read = read(fd, buf, buf_size)) > 0) {
        buf[n_read] = '\0';

        char *closest_delim = mx_strchr(buf, delim);

        if (closest_delim != NULL) {
            *closest_delim = '\0';
            buf_remainder = mx_strdup(closest_delim + 1);
        } else
            buf_remainder = NULL;

        char *temp_ptr = NULL;
        size_result += mx_strlen(buf);
        temp_ptr = str;
        str = mx_strjoin(str, buf);
        mx_strdel(&temp_ptr);
        if (buf_remainder != NULL) {
            if (mx_strlen(buf_remainder) == 0) {
                mx_strdel(&buf_remainder);
            }
            break;
        }
    }
    mx_strdel(&buf);

    if (n_read == -1) {
        mx_strdel(&str);
        mx_strdel(&buf_remainder);
        return -2;
    }

    if (str == NULL) {
        return -1;
    }

    *lineptr = str;
    return size_result;
}
