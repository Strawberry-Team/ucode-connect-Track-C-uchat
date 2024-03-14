#include "libmx.h"

void mx_push_index(t_list **list, void *data, int index) {
    if(list == NULL)
        return;
    if (index < 1 || *list == NULL) {
        mx_push_front(list, data);
        return;
    }
    t_list *current_list = *list;
    int i = 0;
    while (i < index - 1 && current_list != NULL) {
        current_list = current_list->next;
        i++;
    }

    if (current_list == NULL) {
        mx_push_back(list, data);
        return;
    }

    t_list *node = mx_create_node(data);
    node->next = current_list->next;
    current_list->next = node;
}
