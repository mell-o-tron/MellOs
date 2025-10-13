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
linked_list_t* list_create();
void list_destroy(linked_list_t *list);

// Element access and modification
void list_push_back(linked_list_t *list, void *data);
void list_push_front(linked_list_t *list, void *data);
void* list_pop_back(linked_list_t *list);
void* list_pop_front(linked_list_t *list);
void* list_get(linked_list_t *list, size_t index);
bool list_remove(linked_list_t *list, void *data);

// List information
size_t list_size(linked_list_t *list);
bool list_is_empty(linked_list_t *list);
