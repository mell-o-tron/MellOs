#include "mellos/kernel/memory_mapper.h"
#include "autoconf.h"
#include "stdbool.h"
#include "stdint.h"

#include "mellos/kernel/kernel_stdio.h"
#include "mellos/kernel/multiboot_tags.h"
#include "memory_area_spec.h"

#include "dynamic_mem.h"
#include "mem.h"

#include "vesa.h"

MultibootTags* multiboot_tags_local = NULL;
void* framebuffer_addr_local = NULL;
uint8_t bpp_local = 0;

__attribute__((section(".low.bss"))) static struct {
	uint32_t fb_width_local;
	uint32_t fb_height_local;
	uint32_t fb_pitch_local; // bytes per line
} fb_info = {0};

#define MB1_MMAP_MAX_BYTES 65536
__attribute__((section(".low.bss"))) static struct {
	uint32_t flags;
	uint32_t mmap_length;
	void* mmap_copy; // points into mmap_buf when valid
	uint8_t mmap_buf[MB1_MMAP_MAX_BYTES];
} mb1_cache = {0};

#define FB_INFO_BIT 12 // bit 12 for framebuffer

__attribute__((section(".low.text"))) inline uintptr_t
get_multiboot_framebuffer_addr(MultibootTags* mb) {
	if (!mb) {
		return 0;
	}
	const uint32_t flags = mb->flags;
	if ((flags & (1U << FB_INFO_BIT)) == 0) {
		return 0;
	}
	return (intptr_t)mb->framebuffer_addr[0];
}
__attribute__((section(".low.text"))) void init_memory_mapper(MultibootTags* multiboot_tags,
                                                              void* framebuffer_addr, uint8_t bpp) {
	multiboot_tags_local = multiboot_tags;
	framebuffer_addr_local = framebuffer_addr;
	bpp_local = bpp;

	// Cache framebuffer geometry early to avoid later derefs of bootloader memory
	if (multiboot_tags && CHECK_FLAG(multiboot_tags->flags, FB_INFO_BIT)) {
		fb_info.fb_width_local = multiboot_tags->framebuffer_width;
		fb_info.fb_height_local = multiboot_tags->framebuffer_height;
		fb_info.fb_pitch_local = multiboot_tags->framebuffer_pitch; // bytes per line
	} else {
		fb_info.fb_width_local = 0;
		fb_info.fb_height_local = 0;
		fb_info.fb_pitch_local = 0;
	}

	// Copy Multiboot v1 memory map early so later code doesn’t depend on bootloader pointers
	if (multiboot_tags && CHECK_FLAG(multiboot_tags->flags, 6) && multiboot_tags->mmap_length) {
		// NOTE: mmap_addr is a physical address provided by the bootloader. This copy must
		// happen while that region is mapped (typically identity-mapped early in boot).
		if (multiboot_tags->mmap_length <= MB1_MMAP_MAX_BYTES) {
			size_t count = multiboot_tags->mmap_length;
			char* dest = (char*)mb1_cache.mmap_buf;
			char* source = (void*)(uintptr_t)multiboot_tags->mmap_addr;
			while (count > 0) {
				*dest++ = *source++;
				count--;
			}
			mb1_cache.flags = multiboot_tags->flags;
			mb1_cache.mmap_length = multiboot_tags->mmap_length;
			mb1_cache.mmap_copy = mb1_cache.mmap_buf;
			// kprintf("meminfo: flags=%08x mmap_len=%x mmap_addr=%08x (cached)\n",
			//         (unsigned)multiboot_tags->flags,
			//         (unsigned)multiboot_tags->mmap_length,
			//         (unsigned)multiboot_tags->mmap_addr);
		} else {
			// Map too large for our static buffer; keep cache empty but preserve flags for
			// diagnostics
			mb1_cache.flags = multiboot_tags->flags;
			mb1_cache.mmap_length = 0;
			mb1_cache.mmap_copy = NULL;
			// kprintf("meminfo: mmap_length (%x) exceeds BSS buffer (%x), not caching\n",
			//         (unsigned)multiboot_tags->mmap_length,
			//         (unsigned)MB1_MMAP_MAX_BYTES);
		}
	} else {
		mb1_cache.flags = multiboot_tags ? multiboot_tags->flags : 0;
		mb1_cache.mmap_length = 0;
		mb1_cache.mmap_copy = NULL;
		// if (multiboot_tags) {
		//     kprintf("meminfo: flags=%08x (no mmap available)\n",
		//     (unsigned)multiboot_tags->flags);
		// } else {
		//     kprintf("meminfo: multiboot tags ptr is NULL at init\n");
		// }
	}
}

extern char __kload_start[];
extern char __image_lma_end[];
extern uintptr_t kernel_heap_phys_start;
extern uintptr_t kernel_heap_phys_end;

MemoryArea map_memory() {
	uint32_t num_excluded_areas = 0;
	struct {
		intptr_t start;
		intptr_t end;
	} excluded_areas[MAX_EXCLUDED_AREAS];

	// Exclude Kernel image
	excluded_areas[num_excluded_areas].start = (intptr_t)__kload_start;
	excluded_areas[num_excluded_areas].end = (intptr_t)__image_lma_end;
	num_excluded_areas++;

	// Exclude Kernel Heap physical area
	excluded_areas[num_excluded_areas].start = (intptr_t)kernel_heap_phys_start;
	excluded_areas[num_excluded_areas].end = (intptr_t)kernel_heap_phys_end;
	num_excluded_areas++;

	// Exclude Multiboot structures if they are in the available memory range
	if (multiboot_tags_local) {
		excluded_areas[num_excluded_areas].start = (intptr_t)multiboot_tags_local;
		excluded_areas[num_excluded_areas].end = (intptr_t)multiboot_tags_local + sizeof(MultibootTags);
		num_excluded_areas++;

		if (CHECK_FLAG(multiboot_tags_local->flags, 3) && multiboot_tags_local->mods_count > 0) {
			// This is a simplification; ideally we'd exclude each module's range.
			// But since we don't have modules yet (found nothing in search), we'll skip for now
			// or just exclude the mods_addr area if it looks valid.
		}
	}

	uintptr_t base_mem = 0L;
	size_t len_mem = 0L;

	if (CHECK_FLAG(mb1_cache.flags, 6) && mb1_cache.mmap_copy && mb1_cache.mmap_length) {

		// Add framebuffer area to excluded areas
#ifdef CONFIG_GFX_VESA
		if (framebuffer_addr_local && fb_info.fb_height_local &&
		    (fb_info.fb_pitch_local || (fb_info.fb_width_local && bpp_local))) {
			excluded_areas[num_excluded_areas].start = (intptr_t)framebuffer_addr_local;
			intptr_t fb_size = 0;
			if (fb_info.fb_pitch_local) {
				fb_size = (intptr_t)fb_info.fb_pitch_local * (intptr_t)fb_info.fb_height_local;
			} else {
				// Fallback: width * height * (bpp/8)
				fb_size = (intptr_t)fb_info.fb_width_local * (intptr_t)fb_info.fb_height_local *
				          (intptr_t)(bpp_local / 8);
			}
			excluded_areas[num_excluded_areas].end = (intptr_t)framebuffer_addr_local + fb_size;
			num_excluded_areas++;
		}
#endif
		for (multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)mb1_cache.mmap_copy;
		     (uintptr_t)mmap < (uintptr_t)mb1_cache.mmap_copy + mb1_cache.mmap_length;
		     mmap = (multiboot_memory_map_t*)((uintptr_t)mmap + mmap->size + sizeof(mmap->size))) {
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

		} // end of for loop
	} else {
		//kprintf("meminfo: cached memory map unavailable (bit 6 not set or not copied)\n");
		return (MemoryArea){(intptr_t)NULL, 0};
	}

	return (MemoryArea){base_mem, len_mem};
}

void dump_memory_map(FILE* stream) {
	if (!(CHECK_FLAG(mb1_cache.flags, 6) && mb1_cache.mmap_copy && mb1_cache.mmap_length)) {
		kfprintf(stream, "meminfo: memory map not available (bit 6 unset or not copied)\n");
		kfprintf(stream, "flags=0x%08x, mmap_length=0x%x, cached=%s\n", (unsigned)mb1_cache.flags,
		        (unsigned)mb1_cache.mmap_length, mb1_cache.mmap_copy ? "yes" : "no");
		return;
	}

	kfprintf(stream, "Memory Map (Multiboot v1, cached):\n");
	kfprintf(stream, "cached_length = %x bytes\n", (unsigned)mb1_cache.mmap_length);

	for (multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)mb1_cache.mmap_copy;
	     (uintptr_t)mmap < (uintptr_t)mb1_cache.mmap_copy + mb1_cache.mmap_length;
	     mmap = (multiboot_memory_map_t*)((uintptr_t)mmap + mmap->size + sizeof(mmap->size))) {
		const char* type_name =
		    (mmap->type >= 1 && mmap->type <= 5) ? names[mmap->type - 1] : "Unknown";
		kfprintf(stream, "base = %016lX, length = %016lX, type = %s\n", (uintptr_t)mmap->addr,
		        (uintptr_t)mmap->len, type_name);
	}
}
