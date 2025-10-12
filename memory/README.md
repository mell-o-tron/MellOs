[← Main README](../README.md)

# memory

This directory contains memory management code for MellOs, including dynamic memory allocation and memory utility functions.

## dynamic_mem.h / dynamic_mem.c
- **init_allocators(void\* loc, size_t size)**: Initializes memory allocators.
- **kmalloc(size_t size)**: Allocates a block of memory of size `size`.
- **kfree(void\* loc)**: Frees a block of memory.


## Subdirectories
- [Paging](paging/README.md)