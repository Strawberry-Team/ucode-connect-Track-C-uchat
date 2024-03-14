#include "libmx.h"

void mx_push_front(t_list **list, void *data) {
    if(list == NULL)
        return;
    t_list *first_node = mx_create_node(data);
    first_node->next = *list;
    *list = first_node;
}
