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
// I do care! I will explain for future reference: - Alex23087
// GDT descriptors are 16 bits structured as follows:
// 0-12: index in the GDT
// 13  : 0 for GDT, 1 for LDT
// 14-15: Request Privilege Level (RPL) (Privilege level, only 00 and 11 are used)
// 0x08 is 0000000000001 0 00, Kernel code segment, in the GDT, with high privileges
// 0x10 is 0000000000010 0 00, Kernel data segment, in the GDT, with high privileges
#define GDT_CODE_SEGMENT 0x08
#define GDT_DATA_SEGMENT 0x10

void gdt_init();
