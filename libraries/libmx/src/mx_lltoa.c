#include "libmx.h"

static int calculate_size(long long number) {
    int size = 1;
    if (number < 0)
        size++;
    for (long long i = number; i >= 10 || i <= -10; i /= 10)
        size++;
    return size;
}

char *mx_lltoa(long long number) {
    int size = calculate_size(number);
    char *str = mx_strnew(size);
    if (number < 0)
        str[0] = '-';
    for (int i = size - 1; i >= 0 && str[i] != '-'; i--) {
        long long temp_digit = number % 10;
        if (temp_digit < 0)
            temp_digit *= -1;
        str[i] = temp_digit + '0';
        number /= 10;
    }
    return str;
}

