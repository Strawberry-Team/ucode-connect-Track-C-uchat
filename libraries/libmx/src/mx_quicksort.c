#include "libmx.h"

static void swap(char **str1, char **str2) {
    char *current = *str1;
    *str1 = *str2;
    *str2 = current;
}

int mx_quicksort(char **arr, int left, int right) {
    if (arr == NULL)
        return -1;
    int counter = 0;
    if (left < right) {
        int current_left = left;
        int current_right = right;
        char *pivot = arr[(current_left + current_right) / 2];

        while (current_left <= current_right) {
            while (mx_strlen(arr[current_left]) < mx_strlen(pivot)) current_left++;
            while (mx_strlen(arr[current_right]) > mx_strlen(pivot)) current_right--;

            if (current_left <= current_right) {
                if (mx_strlen(arr[current_right]) != mx_strlen(arr[current_left])) {
                    swap(&arr[current_left], &arr[current_right]);
                    counter++;
                }
                current_right--;
                current_left++;
            }
        }

        counter += mx_quicksort(arr, left, current_right);
        counter += mx_quicksort(arr, current_left, right);
    }
    return counter;
}
