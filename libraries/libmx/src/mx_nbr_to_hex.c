#include "libmx.h"

static int calculate_size(unsigned long nbr) {
    int size = 1;
    for (unsigned long i = nbr; i >= 16; i /= 16)
        size++;
    return size;
}

char *mx_nbr_to_hex(unsigned long nbr) {
    int size = calculate_size(nbr);
    char *str = mx_strnew(size);

    for (int i = size - 1; i >= 0; i--) {
        if (nbr % 16 < 10)
            str[i] = nbr % 16 + 48;
        else
            str[i] = nbr % 16 + 87;
        nbr /= 16;
    }

    return str;
}

