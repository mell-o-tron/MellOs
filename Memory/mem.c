/*
 * How to use Dynamic Memory Allocation in Mellotron (an introduction to the Lazy Linear Allocation Method)
 * 
 * You should try to use DMA almost exclusively in block scopes; 
 * Use the memrec() function at the beginning of the block and the memunrec() function at the end: these will free all of the memory you allocated in the block.
 * 
 * Have a look at the other functions, they're pretty ez to understand. Yes, this method sucks. Can you do better? Bet you can.
 */


#include "mem.h"
#include "./../Drivers/VGA_Text.h"
#include "../Misc/colors.h"
#define FREE_MEM 0x100000

void* memset(void* dest, unsigned char val, size_t count){ 
    /* Indicate failure */
    if (!dest)
        return NULL;

	unsigned char* destC = (unsigned char*)dest;
	int i;
	for (i = 0; i < count; i++)
		destC[i] = val;
	return dest;
}

/* Copy blocks of memory */
void memcp(unsigned char* source, unsigned char* dest, size_t count){
    if (!source || !dest)
        return;

    for (int i = 0; i < count; i++)
        *(dest + i) = *(source + i);
}

static size_t freeMem = FREE_MEM;

void initializeMem(){
    freeMem = FREE_MEM;
}


extern char ker_tty[4000];

/* allocate space linearly (Welcome to the worst allocation method ever), starting from address 0x10000 */
void* linear_alloc (size_t size){
    /* Don't want to be returning block sizes of zero */
    if (!size)
        return NULL;

    memset((void*)freeMem, 0, size); 
    void* address = (void*)freeMem;
    freeMem += size;
    
    if(freeMem > upper_memget().lower_size + FREE_MEM){
        // display out of memory error
        clear_tty(DEFAULT_COLOR, ker_tty);
        display_tty(ker_tty);
        SetCursorPosRaw(0);
        kprint("\n> Kernel Error - Out of memory :(");
        while(1){;}
    }
    
    return address;
}
/* "free" last bytes of memory */
void memcut(size_t size) {freeMem -= size;}

/* free the whole dynamic memory */
void memreset(){freeMem = FREE_MEM;}


/* the following two functions can be called at the beginning and end of a function; they implement a sort of "scoping" for dynamic memory, that makes the whole system a bit less shit */
static size_t tmpMem;
void memrec(){
    tmpMem = freeMem;
}

void memunrec(){
    freeMem = tmpMem;
}

int getFreeMem(){
   return freeMem; 
}


/* the following is used to get somewhat of a memory map, set up by the bootloader */

mem_t memget(){                     // These values better be protected somehow... TODO
    mem_t* mem = (mem_t*)0x5000;
    return *mem;
}

mem_t upper_memget (){
    mem_t* mem = (mem_t*)0x5100;
    return *mem;
}
