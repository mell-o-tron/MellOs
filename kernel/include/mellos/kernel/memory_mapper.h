//
// Created by matias on 9/25/25.
//

#pragma once
#include "stdint.h"
#include "multiboot_tags.h"
static char* names[5] = {
    "Available",
    "Reserved",
    "ACPI reclaimable",
    "NVS",
    "Bad RAM"
};


typedef struct {
    uintptr_t start;
    size_t length;
} MemoryArea;


#define MAX_EXCLUDED_AREAS 4


// Define excluded areas array

uintptr_t get_multiboot_framebuffer_addr(const MultibootTags* mb);

void init_memory_mapper(MultibootTags *multiboot_tags, void *framebuffer_addr, uint8_t bpp);

MemoryArea get_largest_free_block();
