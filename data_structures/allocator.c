#include "bitmap.h"
#include "../utils/typedefs.h"
#include "allocator.h"
#ifdef VGA_VESA
#include "../drivers/vesa_text.h"
#else
#include "../drivers/vga_text.h"
#endif

// FOR DEBUG
bitmap_t get_allocator_bitmap (allocator_t * allocator){
    return allocator -> bitmap;
}

void set_alloc_bitmap (allocator_t * allocator, bitmap_t loc, uint32_t length){
    allocator -> bitmap = create_bitmap(loc, length);
    allocator -> size = length;
}

// allocates some space and returns the pointer
uint32_t allocate (allocator_t * allocator, size_t n){   // first fit :(
    n = (n + (allocator -> granularity) - 1) / (allocator -> granularity);
    size_t contiguous = 0;
    uint32_t current_champion = 0;
    for (size_t i = 0; i < allocator -> size; i++){
        
        if (contiguous == 0) current_champion = i;
        
        if (get_bitmap(allocator -> bitmap, i) == 0) {
            contiguous++;
        } else {
            contiguous = 0;
            continue;
        }
        
        if (contiguous >= n){
            
            for (size_t j = (size_t)current_champion; j < ((size_t)current_champion) + n; j++){
                set_bitmap(allocator -> bitmap, j);
            }
            
            return (current_champion * allocator -> granularity);
        }
    }
    return NULL;
}

// deallocates space
int allocator_free(allocator_t * allocator, uint32_t index, size_t size){     // also size as input, more practical to implement this way
    size = (size + (allocator -> granularity) - 1) / (allocator -> granularity);
    
    if (index > allocator -> size || index < 0)
        return -1;
        
    if (index + size > allocator -> size)
        return -2;
    
    for (size_t i = index; i < index + size; i++){
        unset_bitmap(allocator -> bitmap, i);
    }
    return 0;
}
