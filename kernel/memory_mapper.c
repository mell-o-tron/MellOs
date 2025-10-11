//
// Created by matias on 9/25/25.
//

#include "mellos/kernel/memory_mapper.h"
#include "stdint.h"
#include "stdbool.h"

#include "stdio.h"

#include "memory_area_spec.h"
#include "mellos/kernel/multiboot_tags.h"

MultibootTags *multiboot_tags_local = NULL;
void *framebuffer_addr_local = NULL;
uint8_t bpp_local = 0;

#define FB_INFO_BIT 12 // bit 12 for framebuffer

inline uintptr_t get_multiboot_framebuffer_addr(const MultibootTags* mb) {
    if (!mb) {
        return 0;
    }
    const uint32_t flags = mb->flags;
    if ((flags & (1U << FB_INFO_BIT)) == 0) {
        return 0;
    }
    return (intptr_t)mb->framebuffer_addr[0];
}

void init_memory_mapper(MultibootTags *multiboot_tags, void *framebuffer_addr, uint8_t bpp) {
    multiboot_tags_local = multiboot_tags;
    framebuffer_addr_local = framebuffer_addr;
    bpp_local = bpp;
}

MemoryArea get_largest_free_block() {
    uint32_t num_excluded_areas = 0;
    struct {
        intptr_t start;
        intptr_t end;
    } excluded_areas[MAX_EXCLUDED_AREAS];


    excluded_areas[num_excluded_areas].start = MAPPED_KERNEL_START;
    excluded_areas[num_excluded_areas].end = MAPPED_KERNEL_START + 0x400000;
    num_excluded_areas++;
    uintptr_t base_mem = 0L;
    size_t len_mem = 0L;

    if (CHECK_FLAG(multiboot_tags_local->flags, 6)) {

        printf("mmap_addr = 0x%x, mmap_length = 0x%x\n",
               (unsigned) multiboot_tags_local->mmap_addr,
               (unsigned) multiboot_tags_local->mmap_length);


        // Add framebuffer area to excluded areas
#ifdef VGA_VESA
        excluded_areas[num_excluded_areas].start = (intptr_t) framebuffer_addr_local;
        excluded_areas[num_excluded_areas].end = (intptr_t) framebuffer_addr_local +
                                                 (intptr_t) multiboot_tags_local->framebuffer_height *
                                                 (intptr_t) multiboot_tags_local->framebuffer_width *
                                                 (intptr_t) bpp_local;
        num_excluded_areas++;
#endif

        for (multiboot_memory_map_t *mmap = (multiboot_memory_map_t *) multiboot_tags_local->mmap_addr;
             (uintptr_t) mmap <
             multiboot_tags_local->mmap_addr + multiboot_tags_local->mmap_length;
             mmap = (multiboot_memory_map_t *) ((uintptr_t) mmap + mmap->size +
                                                sizeof(mmap->size))) {
            if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
                const uintptr_t region_start = mmap->addr;
                const uintptr_t region_end = mmap->addr + mmap->len;
                bool overlaps = false;

                // Check overlap with all excluded areas
                for (int i = 0; i < num_excluded_areas; i++) {
                    if (region_start >= excluded_areas[i].start &&
                        region_end <= excluded_areas[i].end) {
                        overlaps = true;
                        break;
                    }

                    if (region_start < excluded_areas[i].end &&
                        region_end > excluded_areas[i].start) {
                        // Find largest non-overlapping portion
                        const size_t lower_size = excluded_areas[i].start - region_start;
                        const size_t upper_size = region_end - excluded_areas[i].end;

                        if (lower_size > upper_size) {
                            // Use lower portion
                            if (lower_size > len_mem) {
                                len_mem = lower_size;
                                base_mem = region_start;
                            }
                        } else {
                            // Use upper portion
                            if (upper_size > len_mem) {
                                len_mem = upper_size;
                                base_mem = excluded_areas[i].end;
                            }
                        }
                        overlaps = true;
                        break;
                    }
                }

                // If no overlap found, check if this is largest region
                if (!overlaps && mmap->len > len_mem) {
                    len_mem = mmap->len;
                    base_mem = mmap->addr;
                }
            }

            printf("base = 0x%016lx, length = 0x%016lx, type = %s\n",
                          (uintptr_t) mmap->addr,
                          (uintptr_t) mmap->len,
                          names[mmap->type - 1]);
        } // end of for loop
    } else {
        printf("bit 6 not set");
        return (MemoryArea){(intptr_t)NULL, 0};
    }

    return (MemoryArea){base_mem, len_mem};
}
