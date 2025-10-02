#include "paging_utils.h"
#include "stdint.h"
#include "paging.h"


void initialize_page_directory(unsigned int* page_directory) {
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0x00000000 | (PT_READWRITE);    // Last 3 bits are 010:
                                                            // - U/S = 0 : only the supervisor can access the page
                                                            // - R/W = 1 : the page is read/write
                                                            // - P   = 0 : the page is not present
    }   
}
//todo: helper function to identity map
//todo: helper function to parse addresses into entry/table/offset
//todo: helper function to check if a page exists
//todo: helper function to map virtual address to physical address
//todo: helper function to unmap pages
//todo: helper function to get physical address from virtual address
//todo: helper function to map a single page

void add_page_directory(unsigned int * page_directory, unsigned int * page_table, int index, uint32_t offset, PT_FLAGS ptf, PD_FLAGS pdf){
    if (index >= 1024) {
        for (;;) {;}
        // todo: handle error here or smthn
    }
    for(unsigned int i = 0; i < 1024; i++) {
        page_table[i] = (offset + (i * 0x1000)) | ptf; // U/S, R/W same. P = 1 present.
    }

    page_directory[index] = ((unsigned int)page_table) | pdf;
}


void init_paging(unsigned int * page_directory, unsigned int * first_page_table, unsigned int * second_page_table, const uintptr_t high_mem_start) {

    initialize_page_directory(page_directory);

    PD_FLAGS page_directory_flags = PD_PRESENT | PD_READWRITE;
    PT_FLAGS first_page_table_flags = PT_PRESENT | PT_READWRITE;

    add_page_directory(page_directory, first_page_table,  0, 0, first_page_table_flags, page_directory_flags);
    
    // For now, since the kernel is mapped at 0x400000, we identity-map from 0x0 to 0x800000. Eventually, it would be nice to have a proper "kernel on the high memory"
    add_page_directory(page_directory, second_page_table,  1, 0x400000, first_page_table_flags, page_directory_flags);

    loadPageDirectory(page_directory);
}

void stop_paging(){
    disablePaging();
}

void switch_page_directory(unsigned int * page_directory){
    loadPageDirectory(page_directory);
    enablePaging();
}
