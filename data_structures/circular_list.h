#pragma once

typedef struct CircularList {
    void* data;
    struct CircularList* next;
    struct CircularList* prev;
} CircularList;

void clist_append(CircularList** list, void* data);
void clist_remove(CircularList** list, void* data);
void clist_print_all(CircularList* list);