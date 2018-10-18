#include <stdlib.h>

#include "list.h"
#include "allocation.h"

struct ListNode {
    ListValue value;
    ListNode* next;
    ListNode* previous;
};

struct List {
    size_t size;
    ListNode* first;
    ListNode* last;
};

List* list_new(void) {
    List* list;
    MALLOC(list, List);
    list->size = 0;
    list->first = NULL;
    list->last = NULL;
    return list;
}

void list_free(List* list) {
    ListNode* temp;
    for (ListNode* e = list->last; e;) {
        temp = e->previous;
        free(e);
        e = temp;
    }
    free(list);
}

size_t list_size(List* list) {
    return list->size;
}

ListNode* list_first(List* list) {
    return list->first;
}

ListNode* list_last(List* list) {
    return list->last;
}

void list_append(List* list, ListValue value) {
    ListNode* node;
    MALLOC(node, ListNode);
    node->value = value;
    node->next = NULL;

    if (list->size == 0) {
        node->previous = NULL;
        list->first = node;
        list->last = node;
    } else {
        node->previous = list->last;
        list->last->next = node;
        list->last = node;
    }

    list->size++;
}

ListValue list_pop_first(List* list) {
    ListNode* first = list->first;

    list->first = list->first->next;
    list->size--;

    ListValue value = first->value;
    free(first);

    return value;
}

bool list_contains(List* list, ListValue v) {
    for (ListNode* e = list->first; e; e = e->next) {
        if (e->value == v) {
            return true;
        }
    }
    return false;
}

ListValue list_value(ListNode* node) {
    return node->value;
}

ListNode* list_next(ListNode* node) {
    return node->next;
}

ListNode* list_previous(ListNode* node) {
    return node->previous;
}

void list_dump(List* list, StringifyFunction stringify) {
    printf("[");
    for (ListNode* e = list->first; e;) {
        ListValue value = e->value;
        e = list_next(e);
        printf("%s%s", stringify(value), e ? ", " : "");
    }
    printf("]\n");
}
