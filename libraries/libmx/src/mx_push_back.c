#include "libmx.h"

void mx_push_back(t_list **list, void *data) {
    if (list == NULL)
        return;
    if (*list == NULL) {
        *list = mx_create_node(data);
        return;
    }
    t_list *current_list = *list;
    while (current_list->next != NULL)
        current_list = current_list->next;
    current_list->next = mx_create_node(data);
}
