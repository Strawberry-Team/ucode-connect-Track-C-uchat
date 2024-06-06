#include "libmx.h"

char **mx_strsplit(const char *s, char c) {
    if (s == NULL)
        return NULL;
    int count = mx_count_words(s, c);
    char **result = (char **) malloc((count + 1) * sizeof(char *));
    int i = 0;
    for (int counter = 0; counter < count; counter++) {
        int word_size = 0;
        while (s[i] != c && s[i] != '\0') {
            word_size++;
            i++;
        }
        if (word_size == 0 && i < mx_strlen(s)) {
            i++;
            counter--;
        } else {
            result[counter] = mx_strndup(&s[i - word_size], word_size);
        }
    }
    result[count] = NULL;
    return result;
}
