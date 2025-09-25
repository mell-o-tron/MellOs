//
// Created by matias on 9/25/25.
//

#ifndef MELLOS_MEMORY_MAPPER_H
#define MELLOS_MEMORY_MAPPER_H
#include "../utils/typedefs.h"
#include "multiboot_tags.h"
static char* names[5] = {
    "Available",
    "Reserved",
    "ACPI reclaimable",
    "NVS",
    "Bad RAM"
};


typedef struct {
    void *start;
    void *length;
} MemoryArea;


#define MAX_EXCLUDED_AREAS 4


// Define excluded areas array

MemoryArea get_largest_free_block(MultibootTags *multiboot_tags, void *framebuffer_addr, uint8_t bpp, bool print_info);

#endif //MELLOS_MEMORY_MAPPER_H

