#include "linked_list.h"
#include "dynamic_mem.h"
#include "stddef.h"

linked_list_t* create_list() {
    linked_list_t* list = kmalloc(sizeof(linked_list_t));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

void list_push_back(linked_list_t* list, void* data) {
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

void list_push_front(linked_list_t* list, void* data) {
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

void* list_pop_back(linked_list_t* list) {
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

void* list_pop_front(linked_list_t* list) {
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

bool list_remove(linked_list_t* list, void* data) {
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

size_t list_size(linked_list_t* list) {
    return list->size;
}

void destroy_list(linked_list_t* list) {
    while (list->head) {
        struct list_node* node = list->head;
        list->head = node->next;
        kfree(node);
    }
    kfree(list);
}

void* list_get(linked_list_t* list, size_t index) {
    if (index >= list->size) return NULL;

    struct list_node* current = list->head;
    for (size_t i = 0; i < index; i++) {
        current = current->next;
    }
    return current->data;
}
