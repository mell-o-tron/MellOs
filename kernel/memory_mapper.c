//
// Created by matias on 9/25/25.
//

#include "memory_mapper.h"

#include "vesa_text.h"

#include "format.h"

#include "memory_area_spec.h"
#include "multiboot_tags.h"

MultibootTags *multiboot_tags_local = NULL;
void *framebuffer_addr_local = NULL;
uint8_t bpp_local = 0;


void init_memory_mapper(MultibootTags *multiboot_tags, void *framebuffer_addr, uint8_t bpp) {
    multiboot_tags_local = multiboot_tags;
    framebuffer_addr_local = framebuffer_addr;
    bpp_local = bpp;
}

MemoryArea get_largest_free_block() {
    uint32_t num_excluded_areas = 0;
    struct {
        uint64_t start;
        uint64_t end;
    } excluded_areas[MAX_EXCLUDED_AREAS];


    excluded_areas[num_excluded_areas].start = MAPPED_KERNEL_START;
    excluded_areas[num_excluded_areas].end = MAPPED_KERNEL_START + 0x400000;
    num_excluded_areas++;
    uint64_t base_mem = 0L;
    uint64_t len_mem = 0L;

    if (CHECK_FLAG(multiboot_tags_local->flags, 6)) {
        printf("mmap_addr = 0x%x, mmap_length = 0x%x\n",
               (unsigned) multiboot_tags_local->mmap_addr,
               (unsigned) multiboot_tags_local->mmap_length);

        // Add framebuffer area to excluded areas
        excluded_areas[num_excluded_areas].start = (unsigned long long) framebuffer_addr_local;
        excluded_areas[num_excluded_areas].end = (unsigned long long) framebuffer_addr_local +
                                                 multiboot_tags_local->framebuffer_height *
                                                 multiboot_tags_local->framebuffer_width * bpp_local;
        num_excluded_areas++;

        for (multiboot_memory_map_t *mmap = (multiboot_memory_map_t *) multiboot_tags_local->mmap_addr;
             (unsigned long) mmap <
             multiboot_tags_local->mmap_addr + multiboot_tags_local->mmap_length;
             mmap = (multiboot_memory_map_t *) ((unsigned long) mmap + mmap->size +
                                                sizeof(mmap->size))) {
            if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
                const uint64_t region_start = mmap->addr;
                const uint64_t region_end = mmap->addr + mmap->len;
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
                        const uint64_t lower_size = excluded_areas[i].start - region_start;
                        const uint64_t upper_size = region_end - excluded_areas[i].end;

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

            printf("base = 0x%016llx, length = 0x%016llx, type = %s\n",
                          (unsigned long long) mmap->addr,
                          (unsigned long long) mmap->len,
                          names[mmap->type - 1]);
        }

        /*
        int i = 0;
        uint32_t offset = 0;
        multiboot_memory_map_t current_entry;

        stupid_printf("mmap len: %u\n", multiboot_tags->mmap_length);

        while (i < multiboot_tags->mmap_length) {

            current_entry = *(multiboot_memory_map_t *)(multiboot_tags->mmap_addr
        + offset - sizeof(uint32_t)/8); offset += current_entry.size; char
        buf[512]; snprintf(buf, sizeof buf, "offset: %u size: %u\n", offset,
        current_entry.size); kprint(buf); snprintf(buf, sizeof buf, "a: %lu end:
        %lu type: %s\n", current_entry.addr, current_entry.addr -
        current_entry.len, names[current_entry.type - 1]); kprint(buf); i++;
        }*/
    } else {
        printf("bit 6 not set");
        return (MemoryArea){0, 0};
    }

    return (MemoryArea){(void *)base_mem, (void *)len_mem};
}
