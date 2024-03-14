#include "libmx.h"

int mx_bubble_sort(char **arr, int size) {
    int counter = 0;
    int flag = 1;
    char *temp;
    while (flag) {
        flag = 0;
        for (int i = 0; i < size - 1; i++)
            if (mx_strcmp(arr[i], arr[i + 1]) > 0) {
                temp = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = temp;
                flag = 1;
                counter++;
            }
    }
    return counter;
}
