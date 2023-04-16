#pragma once


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


void init_paging(unsigned int * page_directory, unsigned int * first_page_table);
void * get_virtual_address (int pd_index, int pt_index, int offset);
void add_page (unsigned int * page_directory, unsigned int * page_table, int index, int offset, PT_FLAGS ptf, PD_FLAGS pdf);

void initialize_page_directory (unsigned int* page_directory);
