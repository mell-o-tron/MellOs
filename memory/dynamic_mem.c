#include "stdint.h"
#include "stddef.h"
#include "dynamic_mem.h"
#include "mem.h"
#include "allocator.h"

#define MAX_ORDER 23
#define MIN_ORDER 5
#define PAGE_LENGTH 4096

// ----- OLD ALLOCATOR -----

// A bitmap is used to keep track of the memory usage. 

allocator_t* kmallocator = NULL;

volatile void *dynamic_mem_loc = NULL;

void assign_kmallocator(allocator_t* allocator){
    kmallocator = allocator;
}

void set_dynamic_mem_loc (void *loc){
    dynamic_mem_loc = loc;
}

bitmap_t get_kmallocator_bitmap (){
    return get_allocator_bitmap(kmallocator);
}

void set_kmalloc_bitmap (bitmap_t loc, uint32_t length){
    set_alloc_bitmap(kmallocator, loc, length);
}


// ----- BUDDY/SLAB HYBRID ALLOCATOR -----

typedef struct Block {
    struct Block* next;
    int order;
    int free;      // 1 = free, 0 = full
} Block;

Block* free_list[MAX_ORDER + 1];

bool buddy_inited = false;
static void* allocator_base = NULL;
static size_t allocator_size = 0;

bool buddy_init(const uintptr_t base, const size_t size) {
    if (size < 1UL << 7) return false;

    allocator_base = (void *) base;
    allocator_size = size; 

    for (int i = MIN_ORDER; i <= MAX_ORDER; ++i) {
        free_list[i] = NULL;
    }
    int order = MAX_ORDER;
    while (1UL << order > size){
        order--;
    }

    Block* block = (void *) base;
    block->order = order;

    block->free = true;
    block->next = NULL;

    free_list[order] = block;
    buddy_inited = true;
    return true;
}

Block* get_buddy(Block* block) {
    const size_t size = 1 << block->order;
    const size_t offset = (char*)block - (char*)allocator_base;
    const size_t buddy_offset = offset ^ size;
    return (Block*)((char*)allocator_base + buddy_offset);
}

void* buddy_alloc(size_t size) {
    if (allocator_base == NULL) {
        return NULL;
    }

    uint16_t order = MIN_ORDER;
    while (1 << order < size + sizeof(Block)) {
        order++;
    }

    if (order > MAX_ORDER) {
        return NULL;
    }

    int current_order = order;
    while (current_order <= MAX_ORDER && free_list[current_order] == NULL) {
        current_order++;
    }

    if (current_order > MAX_ORDER) {
        return NULL;
    }

    Block* block = free_list[current_order];
    free_list[current_order] = block->next;

    while (current_order > order) {
        current_order--;
        Block* buddy = (Block*)((char*)block + (1 << current_order));
        buddy->order = current_order;
        buddy->free = 1;
        buddy->next = free_list[current_order];
        free_list[current_order] = buddy;
        block->order = current_order;
    }

    block->free = 0;
    block->next = NULL;

    return (char*)block + sizeof(Block);
}

void buddy_free(void* loc) {
    if (!loc) return;
    const uint32_t off = (uint32_t)loc - (uint32_t)dynamic_mem_loc;

    Block* block = (Block*)((char*)off - sizeof(Block));
    int order = block->order;
    block->free = 1;

    while (order < MAX_ORDER) {
        Block* buddy = get_buddy(block);
        if (!(buddy->free && buddy->order == order)) {
            break;
        }
        Block** prev = &free_list[order];
        while (*prev && *prev != buddy) {
            prev = &((*prev)->next);
        }

        if (*prev == buddy) {
            *prev = buddy->next;
        }

        if (block > buddy) {
            Block* tmp = block;
            block = buddy;
            buddy = tmp;
        }

        order++;
        block->order = order;
    }

    block->next = free_list[block->order];
    free_list[block->order] = block;
}

#define SLAB_OBJ_SIZES 6
const size_t slab_sizes[SLAB_OBJ_SIZES] = {8, 16, 32, 64, 128, 256};

typedef struct LilSlab {
    uint8_t* bitmap;
    size_t obj_size;
    size_t capacity;
    void* memory;
    struct LilSlab* next;
} LilSlab;

LilSlab* slab_heads[SLAB_OBJ_SIZES] = { NULL };

void* slab_alloc(const size_t size) {
    int idx = -1;
    for (int i = 0; i < SLAB_OBJ_SIZES; i++) {
        if (size <= slab_sizes[i]) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        return NULL;
    }

    const LilSlab* slab = slab_heads[idx];

    while (slab) {
        for (size_t i = 0; i < slab->capacity; i++) {
            if (!(slab->bitmap[i / 8] & (1 << (i % 8)))) {
                slab->bitmap[i / 8] |= (1 << (i % 8));
                return (char *) slab->memory + i * slab->obj_size;
            }
        }
        slab = slab->next;
    }

    // Creates new slab
    void* slab_page = buddy_alloc(PAGE_LENGTH);
    if (!slab_page) {
        return NULL;
    }

    size_t capacity_guess = (PAGE_LENGTH - sizeof(LilSlab)) / slab_sizes[idx];
    size_t bitmap_size = (capacity_guess + 7) / 8;
    size_t capacity = (PAGE_LENGTH - sizeof(LilSlab) - bitmap_size) / slab_sizes[idx];

    if (capacity == 0) {
        return NULL;
    }

    LilSlab* new_slab = slab_page;
    new_slab->obj_size = slab_sizes[idx];
    new_slab->capacity = capacity;
    new_slab->bitmap = (uint8_t*)(new_slab + 1);

    memset(new_slab->bitmap, 0, (capacity + 7) / 8);

    new_slab->memory = (void*)((char*)new_slab->bitmap + ((capacity + 7) / 8));
    new_slab->next = slab_heads[idx];
    slab_heads[idx] = new_slab;

    new_slab->bitmap[0] |= 1;
    return new_slab->memory;
}

void slab_free(void* loc, size_t size) {
    uint32_t off = (uint32_t)loc - (uint32_t)dynamic_mem_loc;
    int idx = -1;
    for (int i = 0; i < SLAB_OBJ_SIZES; i++) {
        if (size <= slab_sizes[i]) {
            idx = i;
            break;
        }
    }

    if (idx == -1) return;

    LilSlab* slab = slab_heads[idx];
    while (slab) {
        if ((char*)off >= (char*)slab->memory &&
            (char*)off < (char*)slab->memory + slab->capacity * slab->obj_size) {

            size_t offset = (char*)off - (char*)slab->memory;
            size_t index = offset / slab->obj_size;
            slab->bitmap[index / 8] &= ~(1 << (index % 8));
            return;
        }
        slab = slab->next;
    }
}

void* kmalloc(size_t size) {
    if (!buddy_inited)
        return NULL;
    if (size <= 256) {
        long unsigned int offset = (long unsigned int) slab_alloc(size);
        return (void*)((long unsigned int)dynamic_mem_loc + offset);
    } else {
        long unsigned int offset = (long unsigned int) buddy_alloc(size);
        return (void*)((long unsigned int)dynamic_mem_loc + offset);
    }
}

void kfree(void* loc, size_t size) {
    if (size <= 256) {
        slab_free(loc, size);
    } else {
        buddy_free(loc);
    }
}

void* krealloc (void* oldloc, size_t oldsize, size_t newsize){
                                                            // switch this to 1 to change realloc mode
#if 0
    void* newloc = kmalloc(newsize);
    if (newloc == NULL) return NULL;
    kfree(oldloc, oldsize);                                 // no risk to lose reference
    
    size_t min = (oldsize > newsize) ? newsize : oldsize;
    memcp(oldloc, newloc, min);
    return newloc;
#else
    kfree(oldloc, oldsize);                                 // less fragmentation this way, but if no memory there is risk to lose a reference.
    void* newloc = kmalloc(newsize);
    if (newloc == NULL) return NULL;
    
    size_t min = (oldsize > newsize) ? newsize : oldsize;
    memcp(oldloc, newloc, min);
    return newloc;    
    
#endif
}
