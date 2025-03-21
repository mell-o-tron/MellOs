#pragma once

typedef struct List {
    void* data;
    struct List* next;
} List;

void list_append(List** list, void* data);
void list_remove(List** list, void* data);