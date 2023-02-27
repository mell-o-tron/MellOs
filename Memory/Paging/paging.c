#include "paging_stuff_not_relevant_to_kernel.h"
#include "paging.h"
void init_paging() {
    
    /* First we create a blank page directory (which will point to the page tables) */
        
    //set each entry to not present
    int i;
    for(i = 0; i < 1024; i++) {
        page_directory[i] = 0x00000002;     // Last 3 bits are 010:
                                            // - U/S = 0 : only the supervisor can access the page
                                            // - R/W = 1 : the page is read/write
                                            // - P   = 0 : the page is not present
    }
    
    
    /* Then we create a page table */
    
    // holds the physical address where we want to start mapping these pages to.
    
    // the mapping should start from some address like 0x100000 (or, in general, from the beginning of a portion of usable RAM) TODO get decent memory map
    
    //we will fill all 1024 entries in the table, mapping 4 megabytes
    // the 4*3 = 12 zero bits at the end of the page table entry are used for attributes
    
    unsigned int offset = 0;
    
    for(unsigned int i = 0; i < 1024; i++) {
        first_page_table[i] = (offset + (i * 0x1000)) | 3; // U/S, R/W same. P = 1 present.
    }
    
    /* Write the first page table into the page directory */
    page_directory[0] = ((unsigned int)first_page_table) | 3;

    /* Call assembly routines that tell the processor where to find the page directory */
    loadPageDirectory(page_directory);
    enablePaging();
}
