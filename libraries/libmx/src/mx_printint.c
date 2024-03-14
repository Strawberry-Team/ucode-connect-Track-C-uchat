#include "libmx.h"

void mx_printint(int n) {
    if (n < 0) {
        mx_printchar('-');
        n *= -1;
    }
    if (n == 0) {
        mx_printchar('0');
        return;
    }
    int counter = 0;
    for (unsigned i = (unsigned) n; i != 0; i = (i - (i % 10)) / 10)
        counter++;
    int num[counter];
    int j = counter;
    for (unsigned i = (unsigned) n; i != 0; i = (i - (i % 10)) / 10) {
        num[j - 1] = i % 10;
        j--;
    }
    for (int i = 0; i < counter; i++) {
        mx_printchar(num[i] + 48);
    }
}
