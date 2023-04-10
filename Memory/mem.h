#ifndef MEM_H
#define MEM_H
#include "../Utils/Typedefs.h"
typedef struct {
    uint16_t lower_size;
    uint16_t upper_size;
} mem_t;
mem_t memget(void);
mem_t upper_memget (void);
void* memset(void* dest, unsigned char val, size_t count);
void memcp(unsigned char* source, unsigned char* dest, size_t count);
void* linear_alloc(size_t size);
void memcut(size_t size);
void memreset();
void memrec();
void memunrec();
int getFreeMem();
void initializeMem();
#endif
