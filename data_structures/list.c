#include "autoconf.h"
#include "list.h"
#include "stdint.h"
#include "dynamic_mem.h"
#include "conversions.h"
#ifdef CONFIG_GFX_VESA
#include "vesa_text.h"
#else
#include "vga_text.h"
#endif

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