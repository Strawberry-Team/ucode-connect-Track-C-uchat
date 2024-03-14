#include "libmx.h"

unsigned long mx_hex_to_nbr(const char *hex) {
    if (hex == NULL)
        return 0;
    unsigned long temp = 0;
    unsigned long base = 1;
    int size = 0;
    for (int i = 0; hex[i] != '\0'; i++)
        size++;
    for (int i = size - 1; i >= 0; i--) {
        if (mx_isdigit(hex[i]))
            temp += (hex[i] - 48) * base;
        else if (hex[i] >= 'a' && hex[i] <= 'f')
            temp += (hex[i] - 87) * base;
        else if (hex[i] >= 'A' && hex[i] <= 'F')
            temp += (hex[i] - 55) * base;
        else
            return 0;
        base *= 16;
    }
    return temp;
}
