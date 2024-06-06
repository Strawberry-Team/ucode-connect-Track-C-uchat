#include "libmx.h"

char *mx_strcat(char *s1, const char *s2) {
    int s1_size = mx_strlen(s1);

    for (int i = 0; s2[i] != '\0'; i++)
        s1[s1_size++] = s2[i];

    s1[s1_size] = '\0';

    return s1;
}
