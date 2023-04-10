#include "paging_utils.h"
#include "paging.h"

typedef enum {
    PD_PRESENT         = 0b1,
    PD_READWRITE       = 0b10,
    PD_USER            = 0b100,
    PD_WRITETHROUGH    = 0b1000,
    PD_CACHEDISABLE    = 0b10000,
    PD_ACCESSED        = 0b100000,
    PD_DIRTY           = 0b1000000, 
    PD_PAGESIZE        = 0b10000000,
    PD_GLOBAL          = 0b100000000,
} PD_FLAGS;

typedef enum {
    PT_PRESENT         = 0b1,
    PT_READWRITE       = 0b10,
    PT_USER            = 0b100,
    PT_WRITETHROUGH    = 0b1000,
    PT_CACHEDISABLE    = 0b10000,
    PT_ACCESSED        = 0b100000,
    PT_DIRTY           = 0b1000000, 
    PT_PAT             = 0b10000000,
    PT_GLOBAL          = 0b100000000,
} PT_FLAGS;


void init_paging() {
    
    /* First we create a blank page directory (which will point to the page tables) */
        
    //set each entry to not present
    int i;
    for(i = 0; i < 1024; i++) {
        page_directory[i] = 0x00000000 | (PT_READWRITE);    // Last 3 bits are 010:
                                                            // - U/S = 0 : only the supervisor can access the page
                                                            // - R/W = 1 : the page is read/write
                                                            // - P   = 0 : the page is not present
    }
    
    
    /* Then we create a page table */
    
    // holds the physical address where we want to start mapping these pages to.
    
    // the mapping should start from some address like 0x100000 (or, in general, from the beginning of a portion of usable RAM) TODO get decent memory map
    
    //we will fill all 1024 entries in the table, mapping 4 megabytes
    // the 4*3 = 12 zero bits at the end of the page table entry are used for attributes
    
    PD_FLAGS page_directory_flags = PD_PRESENT + PD_READWRITE;

    for(unsigned int i = 0; i < 1024; i++) {
        first_page_table[i] = (i * 0x1000) | page_directory_flags; // U/S, R/W same. P = 1 present.
    }
    
    for(unsigned int i = 0; i < 1024; i++) {
        second_page_table[i] = 1000000 + (i * 0x1000) | page_directory_flags; // U/S, R/W same. P = 1 present.
    }
    
    
    PT_FLAGS first_page_table_flags = PT_PRESENT + PT_READWRITE;
    
    /* Write the first page table into the page directory */
    page_directory[0] = ((unsigned int)first_page_table) | first_page_table_flags ;
    page_directory[1] = ((unsigned int)second_page_table) | first_page_table_flags ;

    /* Call assembly routines that tell the processor where to find the page directory */
    loadPageDirectory(page_directory);
    enablePaging();
}
