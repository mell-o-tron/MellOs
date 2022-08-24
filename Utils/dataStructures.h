#ifndef DSTRUCT
#define DSTRUCT
#include "../Utils/Typedefs.h"
struct node {
    int val;
    struct node* next;
    
}__attribute__((packed));

struct List {
    struct node* head;
    
}__attribute__((packed));

struct ListQueue {
    struct List* list;
    struct node* head;
    struct node* tail;
    
}__attribute__((packed));
struct List* newList();
void listAdd(struct List* myList, int val);
void listPrint(struct List* list);
struct ListQueue* newLQueue();
void lEnqueue (struct ListQueue* queue, int k);
void lDequeue (struct ListQueue* queue);
int lFirst (struct ListQueue* queue);
#endif
