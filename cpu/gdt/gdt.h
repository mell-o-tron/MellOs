//GDT Implementation for the Kernel (see README.md for more info)
//Inspiration & Sources: nanobyte(https://www.youtube.com/watch?v=5LbXClJhxcs), osdev.org(https://wiki.osdev.org/Global_Descriptor_Table).

#pragma once
#include "../../utils/typedefs.h"

// Helper macros
#define LIMIT_LOW(limit)                (limit & 0xFFFF)
#define BASE_LOW(base)                  (base & 0xFFFF)
#define BASE_MIDDLE(base)               ((base >> 16) & 0xFF)
#define FLAGS_LIMIT_HI(limit, flags)    (((limit >> 16) & 0xF) | (flags & 0xF0))
#define BASE_HIGH(base)                 ((base >> 24) & 0xFF)

#define ENTRY(base, limit, access, flags) {                     \
    LIMIT_LOW(limit),                                           \
    BASE_LOW(base),                                             \
    BASE_MIDDLE(base),                                          \
    access,                                                     \
    FLAGS_LIMIT_HI(limit, flags),                               \
    BASE_HIGH(base)                                             \
}


//Dont care too much about these
#define GDT_CODE_SEGMENT 0x08
#define GDT_DATA_SEGMENT 0x10

void gdt_init();
