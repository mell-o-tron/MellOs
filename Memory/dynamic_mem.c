#include "../Utils/bitmap.h"
#include "../Utils/Typedefs.h"
#include "dynamic_mem.h"
#include "mem.h"

// A bitmap is used to keep track of the memory usage. 

volatile bitmap_t allocation_bitmap = NULL;

volatile int allocation_bitmap_size = 0;

volatile void *dynamic_mem_loc = NULL;

// FOR DEBUG
bitmap_t get_allocation_bitmap (){
    return allocation_bitmap;
}


void set_alloc_bitmap (bitmap_t loc, int length){
    allocation_bitmap = create_bitmap(loc, length);
    allocation_bitmap_size = length;
}

void set_dynamic_mem_loc (void *loc){
    dynamic_mem_loc = loc;
}

// allocates some space and returns the pointer
void *kmalloc (int n){   // first fit :(
    int contiguous = 0;
    int current_champion = 0;
    for (int i = 0; i < allocation_bitmap_size; i++){
        
        if (contiguous == 0) current_champion = i;
        
        if (get_bitmap(allocation_bitmap, i) == 0) contiguous++;
        if (get_bitmap(allocation_bitmap, i) == 1) {
            contiguous = 0;
            continue;
        }
        
        if (contiguous >= n){
            
            for (int j = current_champion; j < current_champion + n; j++){
                set_bitmap(allocation_bitmap, j);
            }
            
            return (void*) (current_champion + (int)dynamic_mem_loc);
        }
    }
    return NULL;
}

// deallocates space
int kfree(void* loc, int size){     // also size as input, more practical to implement this way
    
    int bitmap_index = (int) loc - (int) dynamic_mem_loc;
    
    if (bitmap_index > allocation_bitmap_size || bitmap_index < 0)
        return -1;
        
    if (bitmap_index + size > allocation_bitmap_size)
        return -2;
    
    for (int i = bitmap_index; i < bitmap_index + size; i++){
        unset_bitmap(allocation_bitmap, i);
    }
    return 0;
}

// like free, but zeroes out the memory
int kdisintegrate(void* loc, int size){
    int free_res = kfree(loc, size);
    if (free_res == -1)
        return -1;
    
    for (int i = 0; i < size; i++){
        ((char*)loc)[i] = 0;
    }
    return 0;
}


void* krealloc (void* oldloc, int oldsize, int newsize){
                                                            // switch this to 1 to change realloc mode
#if 0
    void* newloc = kmalloc(newsize);
    if (newloc == NULL) return NULL;
    kfree(oldloc, oldsize);                                 // no risk to lose reference
    
    int min = (oldsize > newsize) ? newsize : oldsize;
    memcp(oldloc, newloc, min);
    return newloc;
#else
    kfree(oldloc, oldsize);                                 // less fragmentation this way, but if no memory there is risk to lose a reference.
    void* newloc = kmalloc(newsize);
    if (newloc == NULL) return NULL;
    
    int min = (oldsize > newsize) ? newsize : oldsize;
    memcp(oldloc, newloc, min);
    return newloc;    
    
#endif
}
