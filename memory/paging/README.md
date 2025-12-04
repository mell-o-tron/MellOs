[← Memory README](../README.md) | [← Main README](../../README.md)

# paging

This directory contains paging and virtual memory management code for MellOs, including page table setup and address translation. :warning: we currently do not have a page manager :warning:

## paging.h / paging.c
- **PT_FLAGS, PD_FLAGS**: Enums for page table and directory flags.
- **init_paging(unsigned int \* page_directory, unsigned int \* first_page_table, unsigned int \* second_page_table, uintptr_t high_mem_start)**: Initializes paging with the given tables and memory layout.
- **stop_paging()**: Disables paging.
- **add_page_directory(unsigned int \* page_directory, unsigned int \* page_table, int index, uint32_t offset, PT_FLAGS ptf, PD_FLAGS pdf)**: Adds a page table to the directory.
- **initialize_page_directory(unsigned int\* page_directory)**: Initializes a page directory.
- **switch_page_directory(unsigned int \* page_directory)**: Switches the active page directory.

## paging_utils.h
- **loadPageDirectory(unsigned int\*)**: Loads a page directory into the CPU.
- **enablePaging(), disablePaging()**: Enables/disables paging in the CPU.

## pat.h / pat.c
- **PAT_MSR**: MSR address for the Page Attribute Table.
- **setup_pat()**: Sets up the Page Attribute Table.
