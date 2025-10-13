#include "paging_utils.h"
#include "stdint.h"
#include "paging.h"

#include <dynamic_mem.h>
#include <processes.h>
#include <stddef.h>
#include <mellos/kernel/kernel.h>
#include <mellos/kernel/memory_mapper.h>

#define NUM_MANY_DIRECTORIES 32

// these are in kernel.c for some reason
extern uint32_t base_page_directory[1024];
extern uint32_t first_page_table[1024];
extern uint32_t second_page_table[1024];
extern uint32_t framebuffer_pages[2][1024];
uint32_t page_table_identity[1024]; __attribute__((aligned(4096)));
uint32_t page_table_higher_half[1024]; __attribute__((aligned(4096)));


uint32_t *page_directories[1024]; __attribute__((aligned(4096)));
uint32_t directory_count = 0;

extern PD_FLAGS framebuffer_page_dflags;
extern PT_FLAGS framebuffer_page_tflags;

#define KERNEL_START 0xC0000000
#define KERNEL_START_INDEX 768

// todo dynamically allocate these
uint32_t *directory_owner_table;
uint32_t *page_owner_table[1024];

// writes the page directory table full of directories with write perms
void initialize_page_directory() {
    for (int i = 0; i < 1024; i++) {

        base_page_directory[i] = 0;
    }

    // Kernel area
    for (int i = KERNEL_START_INDEX; i < 1024; i++) {
        base_page_directory[i] = 0x00000000 | (PD_PRESENT | PD_READWRITE);   // Last 3 bits are 011:
                                                                        // - U/S = 0 : only the supervisor can access the page
                                                                        // - R/W = 1 : the page is read/write
                                                                        // - P   = 1 : the page is present (this is now kernel memory)
        set_page_ownership(base_page_directory, 0, i, 0);
    }
}



int page_ownership_check(unsigned int page_dir_index, unsigned int page_index) {
    if (page_owner_table[page_dir_index] == NULL) return -1;
    return page_owner_table[page_dir_index][page_index];
}

void set_directory_ownership(unsigned int pd_index, int owner) {
    directory_owner_table[pd_index] = owner;
}

void set_page_ownership(unsigned int * page_table, unsigned int page_directory_index, unsigned int page_index, int owner) {
    base_page_directory[page_directory_index] = ((unsigned int)page_table) | PD_PRESENT | PD_READWRITE | PD_USER;
    page_owner_table[page_directory_index][page_index] = owner;
}

// also checks for the directory
bool is_page_present(unsigned int pd_index, unsigned int * page_table, unsigned int page_index) {
    if (base_page_directory[pd_index] & PD_PRESENT && page_table[page_index] & PT_PRESENT) {
        return true;
    }
    return false;
}

bool is_page_directory_present(unsigned int pd_index) {
    if (base_page_directory[pd_index] & PD_PRESENT) {
        return true;
    }
    return false;
}

void map_framebuffer_pages(uintptr_t fb_addr) {

    for (uint32_t i = 0; i < 512; i++){
        uintptr_t phys_base = (fb_addr & ~0x3FFFFF) + (i * 0x400000);
        put_page_table_to_directory(
            base_page_directory,
            framebuffer_pages[1],
            2 + NUM_MANY_DIRECTORIES + i,
            phys_base,
            framebuffer_page_tflags, framebuffer_page_dflags
            );
    }
}

void put_page_table_to_directory(uint32_t *directory, unsigned int * page_table, int index, uint32_t offset, PT_FLAGS ptf, PD_FLAGS pdf){
    if (index >= 1024) {
        kpanic_message("Invalid page directory index");
    }
    for(unsigned int i = 0; i < 1024; i++) {
        page_table[i] = (offset + (i * 0x1000)) | ptf; // U/S, R/W same. P = 1 present.
    }

    directory[index] = ((unsigned int)page_table) | pdf;
}

void higher_half_init() {
    // For now, since the kernel is mapped at 0x400000, we identity-map from 0x0 to 0x800000. Eventually, it would be nice to have a proper "kernel on the high memory"
    //add_page_directory(page_directory, second_page_table,  1, 0x400000, first_page_table_flags, page_directory_flags);

    // 0 is for low kernel todo: free it
    put_page_table_to_directory(base_page_directory, first_page_table, 1, 0, PD_PRESENT | PD_READWRITE, PD_PRESENT | PD_READWRITE);
    // owner is pid
    set_page_ownership(first_page_table, 1, 0, 0);
    set_page_ownership(first_page_table, 1, 1, 0);

    // paging is already enabled but this pokes cr3 so the cpu gets the new info
    loadPageDirectory(base_page_directory);

    page_directories[0] = page_table_higher_half;
    page_directories[1] = first_page_table;

    base_page_directory[0] = (unsigned int)page_table_identity | (PD_READWRITE | PD_USER);
}

void setup_paging_with_dual_mapping() {

    for (int i = 0; i < 512; i++) {
        page_table_identity[i] = (0x00100000 + i * 0x1000) | (PT_PRESENT | PT_READWRITE);
    }
    base_page_directory[0] = ((unsigned int)page_table_identity) | (PD_PRESENT | PD_READWRITE); // Identity mapping

    for (int i = 0; i < 512; i++) {
        page_table_higher_half[i] = (0x00100000 + i * 0x1000) | (PT_PRESENT | PT_READWRITE);
    }
    base_page_directory[768] = ((unsigned int)page_table_higher_half) | (PD_PRESENT | PD_READWRITE); // Higher-half mapping

    loadPageDirectory(base_page_directory);
    enablePaging();

    void(*higher_half)(void) = (void (*)())(0xC0000000 + ((uintptr_t)&higher_half_init - 0x00100000));

    higher_half();
}

void init_paging(MemoryArea memory_area) {

    initialize_page_directory(); // adds kernel area

    // Map kernel to higher half
    setup_paging_with_dual_mapping();
}

// returns stack top
void *allocate_process_page(process_t process, size_t size) {

    int pageIterator = 0;
    int directoryIterator = 1;
    while (page_directories[directoryIterator][pageIterator] & PD_PRESENT) {
        pageIterator++;
        if (pageIterator >= 1024) {
            return NULL;
        }
    }
    return NULL;
}

void stop_paging(){
    disablePaging();
}

void switch_page_directory(unsigned int * page_directory){
    loadPageDirectory(page_directory);
    enablePaging();
}
