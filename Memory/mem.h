#ifndef MEM_H
#define MEM_H
#include "../Utils/Typedefs.h"
typedef struct {
    uint16_t lower_size;
    uint16_t upper_size;
} mem_t;
mem_t memget(void);
mem_t upper_memget (void);
void* memset(void* dest, unsigned char val, int count);
void* linear_alloc(int size);
void memcut(int size);
void memreset();
void memrec();
void memunrec();
int getFreeMem();
void initializeMem();
#endif
