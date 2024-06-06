#include "libmx.h"

static void swap_data(t_list *list) {
    void *temp_data = list->data;
    list->data = list->next->data;
    list->next->data = temp_data;
}

t_list *mx_sort_list(t_list *lst, bool(*cmp)(void *, void *), bool isReversed) {
    if (lst == NULL || cmp == NULL)
        return lst;

    bool sort_flag = false;
    while (!sort_flag) {
        sort_flag = true;
        for (t_list *i = lst; i->next != NULL; i = i->next) {
            bool cmp_result = cmp(i->data, i->next->data);
            if ((cmp_result && !isReversed) || (!cmp_result && isReversed)) {
                swap_data(i);
                sort_flag = false;
            }
        }
    }
    return lst;
}
