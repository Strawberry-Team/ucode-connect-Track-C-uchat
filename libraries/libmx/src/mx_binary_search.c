#include "libmx.h"

int mx_binary_search(char **arr, int size, const char *s, int *count) {
    if (arr == NULL || s == NULL || size <= 0) {
        *count = 0;
        return -1;
    }
    for (int i = 0; i < size; i++)
        if (arr[i] == NULL) {
            *count = 0;
            return -1;
        }

    int left = 0;
    int right = size - 1;
    while (left <= right) {
        int mid = (left + right) / 2;
        *count = *count + 1;
        if (mx_strcmp(arr[mid], s) == 0) {
            return mid;
        } else if (mx_strcmp(arr[mid], s) < 0)
            left = mid + 1;
        else
            right = mid - 1;
    }
    *count = 0;
    return -1;
}
