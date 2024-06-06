#include "libmx.h"

void mx_del_node_if(t_list **list, void *del_data, bool (*cmp)(void *a, void *b)) {
    t_list *buff;
    t_list *head_buff = *list;
    if ((buff = (*list)->next)) {
        while (buff->next) {
            if (cmp(buff->data, del_data)) {
                head_buff->next = buff->next;
                free(buff);
                buff = head_buff->next;
            } else {
                head_buff = head_buff->next;
                buff = buff->next;
            }
        }
        if (cmp(buff->data, del_data)) {
            head_buff->next = NULL;
            free(buff);
        }
        buff = *list;
        if (cmp(buff->data, del_data)) {
            (*list) = (*list)->next;
            free(buff);
        }
    } else if (cmp(head_buff->data, del_data))
        free(*list);
}
