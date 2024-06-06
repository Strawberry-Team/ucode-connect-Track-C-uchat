#include "libmx.h"

int mx_count_substr(const char *str, const char *sub) {
    if (str == NULL || sub == NULL)
        return -1;
    int sub_length = mx_strlen(sub);
    if (sub_length == 0)
        return 0;
    str = mx_strstr(str, sub);
    int count = 0;
    while (str != NULL) {
        count++;
        str = mx_strstr(str + sub_length, sub);
    }
    return count;
}
