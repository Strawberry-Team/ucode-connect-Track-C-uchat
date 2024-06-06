#include "libmx.h"

int mx_selection_sort(char **arr, int size) {
    int counter = 0;
    for (int i = 0; i < size - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < size; j++) {
            if (mx_strlen(arr[min_idx]) > mx_strlen(arr[j]))
                min_idx = j;
            else if (mx_strlen(arr[min_idx]) == mx_strlen(arr[j]) && mx_strcmp(arr[min_idx], arr[j]) > 0)
                min_idx = j;
        }
        if (min_idx != i) {
            char *t = arr[min_idx];
            arr[min_idx] = arr[i];
            arr[i] = t;
            counter++;
        }
    }
    return counter;
}
