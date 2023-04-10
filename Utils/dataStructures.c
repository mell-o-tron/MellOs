#include "dataStructures.h"
#include "../Memory/mem.h"
#include "../Drivers/VGA_Text.h"


/************************************ LINKED LIST ******************************************/
struct List* newList(){
    struct List* nList = (struct List*)linear_alloc(sizeof(struct List));
    nList -> head = (struct node*)linear_alloc(sizeof(struct node));
    nList -> head -> val = 0;
    return nList;
}

void listAdd(struct List* list, int val){
    struct node* nNode = (struct node*)linear_alloc(sizeof(struct node));
    nNode -> val = val;
    if(list -> head -> next)
        nNode -> next = list -> head -> next;
    nNode -> next = list -> head;
    list -> head = nNode;
}

void listPrint(struct List* list){
    struct node* tmp = (struct node*)linear_alloc(sizeof(struct node));
    tmp = list -> head;
    while(tmp -> next){
        kprintChar(tmp -> val, 0);
        tmp = tmp -> next;
    }
    kprintChar(tmp -> val, 0);
    memcut(sizeof(struct node));    //removing temporary node
    return;
}


/************************************ QUEUE ******************************************/
struct ListQueue* newLQueue(){
    struct ListQueue* nQueue = (struct ListQueue*)linear_alloc(sizeof(struct ListQueue));
    struct List* list = newList();
    nQueue->list = list;
    return nQueue;
}

void lEnqueue (struct ListQueue* queue, int k){
    if(queue->tail) {
        struct node* nNode = (struct node*)linear_alloc(sizeof(struct node));
        nNode -> val = k;
        queue->tail->next = nNode;
        queue->tail = nNode;
    }
    else{   // first enqueue
        listAdd(queue->list, k);
        queue -> tail = queue->list->head;
        queue -> head = queue->list->head;
    }
}

void lDequeue (struct ListQueue* queue){
    if(queue->head){
        queue->head = queue->head->next;
    }
}

int lFirst (struct ListQueue* queue){
    return queue->head->val;
}
