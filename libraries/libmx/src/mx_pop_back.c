#include "libmx.h"

void mx_pop_back(t_list **head) {
    if (head == NULL || *head == NULL)
        return;
    if ((*head)->next == NULL) {
        free(*head);
        *head = NULL;
    } else {
        t_list *temp_list = *head;
        while (temp_list->next->next != NULL)
            temp_list = temp_list->next;
        free(temp_list->next);
        temp_list->next = NULL;
    }
}
