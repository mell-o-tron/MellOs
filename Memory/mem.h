#ifndef MEM_H
#define MEM_H
void* memset(void* dest, unsigned char val, int count);
void* kmalloc(int size);
void memcut(int size);
void memreset();
void memrec();
void memunrec();
int getFreeMem();
void initializeMem();
#endif
