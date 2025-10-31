#pragma once
#include "stdbool.h"
#include "stdint.h"

#include <mellos/kernel/multiboot_tags.h>

typedef enum {
    PD_PRESENT = 0b1,
    PD_READWRITE = 0b10,
    PD_USER = 0b100,
    PD_WRITETHROUGH = 0b1000,
    PD_CACHEDISABLE = 0b10000,
    PD_ACCESSED = 0b100000,
    PD_DIRTY = 0b1000000,
    PD_PAGESIZE = 0b10000000,
    PD_GLOBAL = 0b100000000,
} PD_FLAGS;

typedef enum {
    PT_WRITEBACK = 0b0,
    PT_PRESENT = 0b1,
    PT_READWRITE = 0b10,
    PT_USER = 0b100,
    PT_WRITETHROUGH = 0b1000,
    PT_CACHEDISABLE = 0b10000,
    PT_ACCESSED = 0b100000,
    PT_DIRTY = 0b1000000,
    PT_PAT = 0b10000000,
    PT_GLOBAL = 0b100000000,
    PT_WRITEPROTECT = 0b10000000,
    PT_WRITECOMBINING = 0b10000000 | 0b1000,
} PT_FLAGS;

typedef struct {
    uint32_t **page_directory;
    uint32_t page_table_index;
    uint32_t page;
    uint32_t page_offset;
    uintptr_t address; // physical address
    uint32_t owner;    // pid
} pagedata_t;

bool allocate_pages_virtual(uint32_t owner, size_t count, uintptr_t addr);
void free_physical_frame(uint32_t physical_address);
uint32_t alloc_physical_frame_internal(uint32_t i, uint32_t frames);
uint32_t alloc_physical_frames(uint32_t frames);
void initialize_page_directory(uint32_t *directory);
bool free_page(pagedata_t *page);
bool allocate_page(uint32_t owner, size_t count);
void init_paging(uintptr_t fb, MultibootTags* multiboot_info_addr);
void stop_paging();
void put_page_table_to_directory(uint32_t **directory, uint32_t page_table, uint32_t index,
                                 PD_FLAGS pdf);
pagedata_t* set_page_ownership(uint32_t** page_directory_table, uint32_t page_directory_index,
                               uint32_t page_table_index, uint32_t page_index, uint32_t owner);
void switch_page_directory(uint32_t* page_directory_);

/**
 * Initialize the frame allocator with a specific range
 * @param min_frame: Starting frame number (inclusive)
 * @param max_frame: Ending frame number (exclusive)
 */
void init_frame_allocator_range(uint32_t min_frame, uint32_t max_frame);

/**
 * Standard frame allocator initialization (uses full range)
 */
void init_frame_allocator(void);

/**
 * Allocate physical frames within the configured range
 * @param frames: Number of consecutive frames to allocate
 * @return Physical address of first frame, or 0 on failure
 */
uint32_t alloc_physical_frames_ranged(uint32_t frames);

/**
 * Get the current range limits
 */
void get_frame_allocator_range(uint32_t* out_min_frame, uint32_t* out_max_frame);
