#include "libmx.h"

int mx_insertion_sort(char **arr, int size) {
    int counter = 0;
    for (int i = 1; i < size; i++) {
        for (int j = i; j > 0 && (mx_strlen(arr[j - 1]) > mx_strlen(arr[j])); j--) {
            char *t = arr[j];
            arr[j] = arr[j - 1];
            arr[j - 1] = t;
            counter++;
        }
    }
    return counter;
}
