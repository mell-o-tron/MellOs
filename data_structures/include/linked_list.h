#pragma once

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"

typedef struct list_node {
    void *data;
    struct list_node *next;
    struct list_node *prev;
} list_node_t;

typedef struct linked_list {
    list_node_t *head;
    list_node_t *tail;
    size_t size;
} linked_list_t;

// List initialization and cleanup
linked_list_t* linked_list_create();
void linked_list_destroy(linked_list_t *list);

// Element access and modification
void linked_list_push_back(linked_list_t *list, void *data);
void linked_list_push_front(linked_list_t *list, void *data);
void* linked_list_pop_back(linked_list_t *list);
void* linked_list_pop_front(linked_list_t *list);

list_node_t* linked_list_get_node(linked_list_t *list, void* filter_data, bool(*filter_func)(list_node_t*, void*));
void* linked_list_get(linked_list_t *list, size_t index);
bool linked_list_remove(linked_list_t *list, void *data);

// List information
size_t linked_list_size(linked_list_t *list);
bool linked_list_is_empty(linked_list_t *list);
