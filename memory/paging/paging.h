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
    PT_WRITEBACK       = 0b0,
    PT_PRESENT         = 0b1,
    PT_READWRITE       = 0b10,
    PT_USER            = 0b100,
    PT_WRITETHROUGH    = 0b1000,
    PT_CACHEDISABLE    = 0b10000,
    PT_ACCESSED        = 0b100000,
    PT_DIRTY           = 0b1000000, 
    PT_PAT             = 0b10000000,
    PT_GLOBAL          = 0b100000000,
    PT_WRITEPROTECT    = 0b10000000,
    PT_WRITECOMBINING  = 0b10000000 | 0b1000,
} PT_FLAGS;


void init_paging(unsigned int * page_directory, unsigned int * first_page_table, unsigned int * second_page_table, uintptr_t high_mem_start);
void stop_paging();
void add_page_directory(unsigned int * page_directory, unsigned int * page_table, int index, uint32_t offset, PT_FLAGS ptf, PD_FLAGS pdf);

void initialize_page_directory(unsigned int* page_directory);
void switch_page_directory(unsigned int * page_directory);
