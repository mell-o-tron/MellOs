#pragma once
#include "kernel_stdio.h"
#include "multiboot_tags.h"
#include "stdint.h"
#include "vesa.h"

static char* names[5] = {"Available", "Reserved", "ACPI reclaimable", "NVS", "Bad RAM"};

typedef struct {
	uintptr_t start;
	size_t length;
} MemoryArea;

#define MAX_EXCLUDED_AREAS 4

// Define excluded areas array

uintptr_t get_multiboot_framebuffer_addr(MultibootTags* mb);

void init_memory_mapper(MultibootTags* multiboot_tags, PIXEL* framebuffer_addr, uint8_t bpp);

MemoryArea map_memory();

// Prints the bootloader-provided physical memory map to the current stdout device
void dump_memory_map(FILE* stream);
