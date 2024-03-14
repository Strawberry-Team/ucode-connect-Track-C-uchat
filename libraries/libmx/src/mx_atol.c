#include "libmx.h"

long mx_atol(const char *str) {
    bool continue_flag = true;
    unsigned long long n = 0;
    int minus = 1;
    bool number_flag = false;
    int counter_whitespaces = 0;
    bool sign_flag = false;
    for(int i = 0; continue_flag == true; i++) {
        if (mx_isspace(str[i]) && (i - counter_whitespaces) == 0)
            counter_whitespaces++;
        else if ((str[i] == '-' || str[i] == '+') && sign_flag == false) {
            if (str[i] == '-')
                minus = -1;
            sign_flag = true;
        } else if (mx_isdigit(str[i]) && n == 0) {
            n = (n * 10) + (str[i] - '0');
            number_flag = true;
            sign_flag = true;
        } else if (mx_isdigit(str[i])) {
            n = (n * 10) + (str[i] - '0');
            if(n - 1> INT_MAX)
                return 0;
        } else if (str[i] == '\0')
            continue_flag = false;
        else
            continue_flag = false;
    }
    if (number_flag == false)
        return 0;
    if (minus == 1 && n > LONG_MAX)
        return -1;
    if (minus == -1 && n > LONG_MAX)
        return 0;
    return minus * (long) n;
}
