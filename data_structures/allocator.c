#include "bitmap.h"
#include "../utils/typedefs.h"
#include "allocator.h"
#include "../drivers/vga_text.h"

// FOR DEBUG
bitmap_t get_allocator_bitmap (allocator_t * allocator){
    return allocator -> bitmap;
}


void set_alloc_bitmap (allocator_t * allocator, bitmap_t loc, int length){
    allocator -> bitmap = create_bitmap(loc, length);
    allocator -> size = length;
}
// allocates some space and returns the pointer
int allocate (allocator_t * allocator, int n){   // first fit :(
    int contiguous = 0;
    int current_champion = 0;
    for (int i = 0; i < allocator -> size; i++){
        
        if (contiguous == 0) current_champion = i;
        
        if (get_bitmap(allocator -> bitmap, i) == 0) contiguous++;
        if (get_bitmap(allocator -> bitmap, i) == 1) {
            contiguous = 0;
            continue;
        }
        
        if (contiguous >= n){
            
            for (int j = current_champion; j < current_champion + n; j++){
                set_bitmap(allocator -> bitmap, j);
            }
            
            return (current_champion);
        }
    }
    return -1;
}

// deallocates space
int allocator_free(allocator_t * allocator, uint32_t index, int size){     // also size as input, more practical to implement this way
    
    if (index > allocator -> size || index < 0)
        return -1;
        
    if (index + size > allocator -> size)
        return -2;
    
    for (int i = index; i < index + size; i++){
        unset_bitmap(allocator -> bitmap, i);
    }
    return 0;
}
