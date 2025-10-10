#include "file_system/file_system.h"
#include "utils/string.h"
#include "memory/dynamic_mem.h"
#ifdef VGA_VESA
#include "drivers/vesa/vesa.h"
#include "drivers/vesa/vesa_text.h"
#include "drivers/mouse.h"
#else
#include "drivers/vga_text.h"
#endif
#include "utils/format.h"

#define TEST_SIZE 10

// Simple test: allocate TEST_SIZE bytes, check not null, free
int malloc_test(){
    void * code_loc = kmalloc(TEST_SIZE);

    if (code_loc == NULL){                          // null check
        return 1;
    } else {
        kfree(code_loc);
    }

    return 0;
}

// Allocate 1024 blocks of 1KB, check not null, free all
int stress_test(){
    void* locs[1024];
    for (int i = 0; i < 1024; i++){
        locs[i] = kmalloc(1 * 1024);
        if (locs[i] == NULL){
            for (int j = 0; j < i; j++){
                kfree(locs[j]);
            }
            return 1;
        }
    }

    for (int i = 0; i < 1024; i++){
        kfree(locs[i]);
    }

    return 0;
}

// Allocate sizes from 1 to 1023 bytes, check not null, free, reallocate and check same address
// The current allocator is deterministic, so this should always return the same address
// It is a cheap way to check that we are not losing memory on deallocation
int variable_size_alloc_deterministic_test(){
    for(int i = 1; i < 1024; i++){
        void* loc = kmalloc(i);
        if (loc == NULL){
            return 1;
        }
        uint32_t prev_loc = (uint32_t)loc;
        kfree(loc);
        void* loc2 = kmalloc(i);
        if (loc2 == NULL){
            return 1;
        }
        if ((uint32_t)loc2 != prev_loc){
            printf("Alloc returned different address on same size alloc: %x vs %x\n", (uint32_t)loc2, prev_loc);
            printf("Size was %d\n", i);
            printf("Header says size is %d\n", *((uint8_t*)(prev_loc - 1)));
            return 1;
        }
        kfree(loc2);
    }
    return 0;
}

int run_all_mem_tests() {
    return 
        malloc_test() +
        stress_test() +
        variable_size_alloc_deterministic_test();
}