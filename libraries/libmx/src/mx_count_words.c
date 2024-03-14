#include "libmx.h"

int mx_count_words(const char *str, char c) {
    if (str == NULL)
        return -1;
    int count = 0;
    int counter = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == c && (i - counter) == 0) {
            counter++;
        } else if (str[i] != c) {
            if (str[i + 1] == c || str[i + 1] == '\0')
                count++;
        }
    }
    return count;
}
