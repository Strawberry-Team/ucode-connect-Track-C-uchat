#include "libmx.h"

void mx_pop_index(t_list **list, int index) {
    if (list == NULL || *list == NULL)
        return;
    if (index < 1) {
        mx_pop_front(list);
        return;
    }
    t_list *current_list = *list;
    int i = 0;
    while (i < index - 1 && current_list != NULL) {
        current_list = current_list->next;
        i++;
    }

    if (current_list == NULL) {
        mx_pop_back(list);
        return;
    }

    t_list *temp = current_list->next;
    current_list->next = current_list->next->next;
    free(temp);
}
