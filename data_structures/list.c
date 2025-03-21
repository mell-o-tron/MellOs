#include "list.h"
#include "../utils/typedefs.h"
#include "../drivers/vga_text.h"

void list_append(List** list, void* data){
    List* new_node = kmalloc(sizeof(List));
    new_node->data = data;
    new_node->next = NULL;

    if(*list == NULL){
        *list = new_node;
        return;
    }

    List* current = *list;
    while(current->next != NULL){
        current = current->next;
    }
    current->next = new_node;
}

void print_all(List* list){
    List* current = list;
    while(current != NULL){
        char buf[256];
        tostring((uint32_t)current->data, 16, buf);
        kprint(buf);
        current = current->next;
    }
}

void list_remove(List** list, void* data){
    List* current = *list;
    List* prev = NULL;
    while(current != NULL && current->data != data){
        prev = current;
        current = current->next;
    }

    if(current == NULL) return;

    if(prev == NULL){
        *list = current->next;
    } else {
        prev->next = current->next;
    }
}