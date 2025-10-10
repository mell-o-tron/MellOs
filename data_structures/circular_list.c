#include "circular_list.h"
#include "dynamic_mem.h"

#ifdef VGA_VESA
#include "vesa_text.h"
#else
#include "vga_text.h"
#endif

void clist_append(CircularList** list, void* data){
    CircularList* new_node = kmalloc(sizeof(CircularList));
    new_node->data = data;
    new_node->next = NULL;
    new_node->prev = NULL;

    if(*list == NULL){
        *list = new_node;
        new_node->next = new_node;
        new_node->prev = new_node;
        return;
    }

    CircularList* current = *list;
    while(current->next != *list){
        current = current->next;
    }
    current->next = new_node;
    new_node->prev = current;
    new_node->next = *list;
    (*list)->prev = new_node;
}

void clist_remove(CircularList** list, void* data){
    CircularList* current = *list;
    CircularList* prev = NULL;
    while(current != NULL && current->data != data){
        prev = current;
        current = current->next;
        if (current == *list) return;
    }

    if(prev == NULL){
        if (current->next == current) {
            *list = NULL;
        } else {
            *list = current->next;
            current->prev->next = current->next;
            current->next->prev = current->prev;
        }
    } else {
        prev->next = current->next;
        prev->next->prev = prev;
    }
}

void clist_print_all(CircularList* list){
    CircularList* current = list;
    if (current != NULL){
        do{
            kprint("Element: ");
            kprint_hex((uint32_t)current->data);
            kprint("\n");
            current = current->next;
        } while(current != list);
    }
}