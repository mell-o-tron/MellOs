#include "linked_list.h"
#include "dynamic_mem.h"
#include "stddef.h"

linked_list_t* linked_list_create() {
    linked_list_t* list = kmalloc(sizeof(linked_list_t));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

void linked_list_push_back(linked_list_t* list, void* data) {
    struct list_node* node = kmalloc(sizeof(struct list_node));
    node->data = data;
    node->next = NULL;
    node->prev = list->tail;

    if (list->tail) {
        list->tail->next = node;
    }
    list->tail = node;

    if (!list->head) {
        list->head = node;
    }

    list->size++;
}

void linked_list_push_front(linked_list_t* list, void* data) {
    struct list_node* node = kmalloc(sizeof(struct list_node));
    node->data = data;
    node->prev = NULL;
    node->next = list->head;

    if (list->head) {
        list->head->prev = node;
    }
    list->head = node;

    if (!list->tail) {
        list->tail = node;
    }

    list->size++;
}

void* linked_list_pop_back(linked_list_t* list) {
    if (!list->tail) return NULL;

    struct list_node* node = list->tail;
    void* data = node->data;

    list->tail = node->prev;
    if (list->tail) {
        list->tail->next = NULL;
    } else {
        list->head = NULL;
    }

    list->size--;
    kfree(node);
    return data;
}

void* linked_list_pop_front(linked_list_t* list) {
    if (!list->head) return NULL;

    struct list_node* node = list->head;
    void* data = node->data;

    list->head = node->next;
    if (list->head) {
        list->head->prev = NULL;
    } else {
        list->tail = NULL;
    }

    list->size--;
    kfree(node);
    return data;
}

bool linked_list_remove(linked_list_t* list, void* data) {
    struct list_node* current = list->head;

    while (current) {
        if (current->data == data) {
            if (current->prev) {
                current->prev->next = current->next;
            } else {
                list->head = current->next;
            }

            if (current->next) {
                current->next->prev = current->prev;
            } else {
                list->tail = current->prev;
            }

            list->size--;
            kfree(current);
            return true;
        }
        current = current->next;
    }
    return false;
}

size_t linked_list_size(linked_list_t* list) {
    return list->size;
}

void linked_list_destroy(linked_list_t* list) {
    while (list->head) {
        struct list_node* node = list->head;
        list->head = node->next;
        kfree(node);
    }
    kfree(list);
}

void* linked_list_get(linked_list_t* list, size_t index) {
    if (index >= list->size) return NULL;

    struct list_node* current = list->head;
    for (size_t i = 0; i < index; i++) {
        current = current->next;
    }
    return current->data;
}

list_node_t* linked_list_get_node(linked_list_t* list, void* filter_data, bool(*filter_func)(list_node_t*, void*)) {
	if (linked_list_is_empty(list)) {
		return NULL;
	}
	struct list_node* current = list->head;
    while (current) {
        if (filter_func(current, filter_data)) {
            return current;
        }
    	if (current->next == NULL) {
    		return NULL;
    	}
        current = current->next;
    }
    return NULL;
}

bool linked_list_is_empty(linked_list_t* list) {
	return list->size == 0;
}