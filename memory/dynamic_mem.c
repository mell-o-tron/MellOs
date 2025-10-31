#include "dynamic_mem.h"
#include "assert.h"
#include "mellos/kernel/kernel.h"
#include "mem.h"
#include "memory_area_spec.h"
#include "paging/paging.h"
#include "stddef.h"
#include "stdint.h"

#include <conversions.h>
#include <vga_text.h>

#define MAX_ORDER 24
#define MIN_ORDER 5
#define PAGE_LENGTH 4096

#define BUDDY_DEFAULT_GROW (1u << 5)
#define BUDDY_GROW_ALIGN PAGE_LENGTH

#define ALIGN_UP(x, a) (((x) + ((a) - 1)) & ~((a) - 1))

// ----- BUDDY/SLAB HYBRID ALLOCATOR -----

/** The padding in this struct is used to signal that the
 * block is being allocated with the buddy allocator:
 * if it's != 0, kfree will assume it is a slab allocation.
 */
typedef struct Block {
    struct Block* next;
    size_t order;
    bool free;       // 1 = free, 0 = full
    uint8_t padding; // IMPORTANT: Last byte should be 0
} Block;

Block* free_list[MAX_ORDER + 1];

bool buddy_inited = false;
static void* allocator_base = NULL;
static size_t allocator_size = 0;
static bool buddy_grow(size_t min_bytes);

bool buddy_init(size_t size) {
    if (size < 1UL << 7)
        return false;
    // Use virtual memory for the heap instead of physical
    allocator_base = (void *) KERNEL_HEAP_START;
    allocator_size = size;

    for (size_t i = MIN_ORDER; i <= MAX_ORDER; ++i) {
        free_list[i] = NULL;
    }

    size_t order = MAX_ORDER;
    while (1UL << order > size) {
        order--;
    }

    allocate_pages_virtual(0, (ALIGN_UP(size, PAGE_LENGTH)) / PAGE_LENGTH,
                           (uintptr_t)allocator_base);
    Block* block = allocator_base;
    block->order = order;
    block->free = true;
    block->next = NULL;
    block->padding = 0;

    free_list[order] = block;
    buddy_inited = true;
    buddy_grow(BUDDY_DEFAULT_GROW);
    return true;
}

static void buddy_add_region(void* base, size_t size) {
    uintptr_t cur = (uintptr_t)base;
    uintptr_t end = cur + size;

    // Ensure we start page-aligned (not strictly required for buddy, but sane)
    cur = ALIGN_UP(cur, PAGE_LENGTH);

    while (cur < end) {
        size_t max_block = (size_t)1 << MAX_ORDER;

        // Find largest order that fits AND is aligned at 'cur'
        size_t order = MAX_ORDER;
        while (order > MIN_ORDER) {
            size_t blk = (size_t)1 << order;
            if (blk <= (end - cur) && (cur % blk) == 0)
                break;
            order--;
        }

        // Create a free block header at 'cur'
        Block* b = (Block*)cur;
        b->order = order;
        b->free = true;
        b->padding = 0;
        b->next = free_list[order];
        free_list[order] = b;

        cur += ((size_t)1 << order);
    }
}

static bool buddy_grow(size_t min_bytes) {
    if (!allocator_base) {
        return false;
    }

    size_t grow_bytes = min_bytes;
    if (grow_bytes < BUDDY_DEFAULT_GROW) {
        grow_bytes = BUDDY_DEFAULT_GROW;
    }

    // page-align growth
    grow_bytes = ALIGN_UP(grow_bytes, BUDDY_GROW_ALIGN);

    uintptr_t start = (uintptr_t)allocator_base + allocator_size;
    uintptr_t v = start;
    uintptr_t end = start + grow_bytes;



    for (; v < end; v += PAGE_LENGTH) {
        if (allocate_pages_virtual(0, 1, v) == 0) {
            return false;
        }
    }

    // Tell the buddy about the new memory
    buddy_add_region((void*)start, grow_bytes);
    allocator_size += grow_bytes;
    return true;
}

Block* get_buddy(Block* block) {
    const size_t size = 1 << block->order;
    const size_t offset = (char*)block - (char*)allocator_base;
    const size_t buddy_offset = offset ^ size;
    return (Block*)((char*)allocator_base + buddy_offset);
}

void* buddy_alloc_internal(size_t size) {
    if (allocator_base == NULL) {
        return NULL;
    }

    size_t order = MIN_ORDER;
    while (((size_t)1) << order < size + sizeof(Block)) {
        order++;
    }

    if (order > MAX_ORDER) {
        return NULL;
    }

    size_t current_order = order;
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
        buddy->free = true;
        buddy->next = free_list[current_order];
        buddy->padding = 0;
        free_list[current_order] = buddy;
        block->order = current_order;
    }

    block->free = false;
    block->next = NULL;

    return (char*)block + sizeof(Block);
}

void* buddy_alloc(size_t size) {
    if (allocator_base == NULL)
        return NULL;

    // no growth
    void* p = buddy_alloc_internal(size);
    if (p)
        return p;
    // grow by the minimum required amount
    size_t want = 1u << MAX_ORDER;

    size_t order = MIN_ORDER;
    while ((((size_t)1) << order) < size + sizeof(Block))
        order++;
    if (order <= MAX_ORDER)
        want = (size_t)1 << order;

    if (!buddy_grow(want)) {
        return NULL;
    }

    return buddy_alloc_internal(size);
}

void buddy_free(void* loc) {
    if (!loc)
        return;
    const uint32_t off = (uint32_t)loc - (uint32_t)KERNEL_HEAP_START;

    Block* block = (Block*)((char*)off - sizeof(Block));
    size_t order = block->order;
    block->free = true;

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

#define SLAB_OBJ_SIZES_COUNT 6
#define SLAB_MAX_OBJ_SIZE 256
const size_t slab_sizes[SLAB_OBJ_SIZES_COUNT] = {8, 16, 32, 64, 128, SLAB_MAX_OBJ_SIZE};

typedef struct LilSlab {
    uint8_t* bitmap;
    size_t obj_size;
    size_t capacity;
    void* memory;
    struct LilSlab* next;
} LilSlab;

typedef struct SlabObjHeader {
    uint8_t size;
} SlabObjHeader;

LilSlab* slab_heads[SLAB_OBJ_SIZES_COUNT];

void* slab_alloc(const size_t size) {
    int idx = -1;
    for (int i = 0; i < SLAB_OBJ_SIZES_COUNT; i++) {
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
                return (char*)slab->memory + i * slab->obj_size;
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
    uint32_t off = (uint32_t)loc - (uint32_t)KERNEL_HEAP_START;
    int idx = -1;
    for (int i = 0; i < SLAB_OBJ_SIZES_COUNT; i++) {
        if (size <= slab_sizes[i]) {
            idx = i;
            break;
        }
    }

    if (idx == -1)
        return;

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
    long unsigned int offset;
    if (size <= SLAB_MAX_OBJ_SIZE - sizeof(SlabObjHeader)) {
        void* obj = slab_alloc(size + sizeof(SlabObjHeader));
    	if (!obj) return NULL;
        SlabObjHeader* header = (SlabObjHeader*)((uintptr_t)obj);


        header->size = (uint8_t)(size);
        offset = (unsigned long)obj + sizeof(SlabObjHeader);
    } else {
        offset = (long unsigned int)buddy_alloc_internal(size);
    }
    assert(offset != 0);
    if (offset == 0)
        return NULL;
    return (void*)((long unsigned int)KERNEL_HEAP_START + offset);
}

void kfree(void* loc) {
    // To free a location, we need to check if it was allocated with slab or buddy
    SlabObjHeader* header = (SlabObjHeader*)loc - (sizeof(SlabObjHeader));
    // If the padding byte is non-zero, it was allocated with slab
    // and the size is stored in the header
    uint8_t size = header->size;
    if (size > 0) {
        slab_free(header, size + sizeof(SlabObjHeader));
    } else {
        buddy_free(loc);
    }
}

#pragma GCC push_options
#pragma GCC optimize("O0")

void* krealloc(void* oldloc, size_t oldsize, size_t newsize) {
    // switch this to 1 to change realloc mode
#if 0
    void* newloc = kmalloc(newsize);
    if (newloc == NULL) return NULL;
    kfree(oldloc, oldsize);                                 // no risk to lose reference
    
    size_t min = (oldsize > newsize) ? newsize : oldsize;
    memcp(oldloc, newloc, min);
    return newloc;
#else
    kfree(
        oldloc); // less fragmentation this way, but if no memory there is risk to lose a reference.
    void* newloc = kmalloc(newsize);
    if (newloc == NULL)
        return NULL;

    size_t min = (oldsize > newsize) ? newsize : oldsize;
    memcp(oldloc, newloc, min);
    return newloc;

#endif
}

#pragma GCC pop_options

void init_allocators() {
	memset(slab_heads, 0, sizeof(slab_heads));
    if (!buddy_init(0x4000)) {
        // TODO: This is not ok: if in graphics mode, the
        // fb has not been initialized yet, so we can't print.
        kpanic_message("Buddy allocator fault");
    }
}
