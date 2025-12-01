#include "process_memory.h"
#include "dynamic_mem.h"
#include "mem.h"
#include "stddef.h"

#define INITIAL_PAGE_CAPACITY 16
#define INITIAL_REGION_CAPACITY 8

bool process_memory_init(process_page_list_t* page_list) {
	if (page_list == NULL) {
		return false;
	}

	// todo: no kmalloc for processes
	page_list->pages = kmalloc(INITIAL_PAGE_CAPACITY * sizeof(uintptr_t));
	if (page_list->pages == NULL) {
		return false;
	}

	page_list->page_count = 0;
	page_list->page_capacity = INITIAL_PAGE_CAPACITY;

	page_list->regions = kmalloc(INITIAL_REGION_CAPACITY * sizeof(process_region_t));
	if (page_list->regions == NULL) {
		kfree(page_list->pages);
		page_list->pages = NULL;
		page_list->page_count = 0;
		page_list->page_capacity = 0;
		return false;
	}
	page_list->region_count = 0;
	page_list->region_capacity = INITIAL_REGION_CAPACITY;
	return true;
}

bool process_memory_add_page(process_page_list_t* page_list, uintptr_t page_addr) {
	if (page_list == NULL) {
		return false;
	}

	if (page_list->page_count >= page_list->page_capacity) {
		size_t new_capacity = page_list->page_capacity * 2;
		uintptr_t* new_pages = kmalloc(new_capacity * sizeof(uintptr_t));

		if (new_pages == NULL) {
			return false;
		}

		if (page_list->pages != NULL) {
			memcp((unsigned char*)page_list->pages, (unsigned char*)new_pages,
			      page_list->page_count * sizeof(uintptr_t));
			// todo: no kfree for processes

			kfree(page_list->pages);
		}

		page_list->pages = new_pages;
		page_list->page_capacity = new_capacity;
	}

	page_list->pages[page_list->page_count++] = page_addr;
	return true;
}

bool process_memory_remove_page(process_page_list_t* page_list, uintptr_t page_addr) {
	if (page_list == NULL || page_list->page_count == 0) {
		return false;
	}

	for (size_t i = 0; i < page_list->page_count; i++) {
		if (page_list->pages[i] == page_addr) {
			page_list->pages[i] = page_list->pages[page_list->page_count - 1];
			page_list->page_count--;
			return true;
		}
	}

	return false;
}

bool process_memory_owns_page(const process_page_list_t* page_list, uintptr_t page_addr) {
	if (page_list == NULL || page_list->page_count == 0) {
		return false;
	}

	for (size_t i = 0; i < page_list->page_count; i++) {
		if (page_list->pages[i] == page_addr) {
			return true;
		}
	}

	return false;
}

size_t process_memory_get_page_count(const process_page_list_t* page_list) {
	if (page_list == NULL) {
		return 0;
	}
	return page_list->page_count;
}

void process_memory_cleanup(process_page_list_t* page_list) {
	if (page_list == NULL) {
		return;
	}

	if (page_list->pages != NULL) {
		// todo: no kfree for processes
		kfree(page_list->pages);
		page_list->pages = NULL;
	}
	if (page_list->regions != NULL) {
		kfree(page_list->regions);
		page_list->regions = NULL;
	}

	page_list->page_count = 0;
	page_list->page_capacity = 0;
	page_list->region_count = 0;
	page_list->region_capacity = 0;
}

uintptr_t process_memory_get_page_at(const process_page_list_t* page_list, size_t index) {
	if (page_list == NULL || index >= page_list->page_count) {
		return 0;
	}

	return page_list->pages[index];
}

// todo: make this a linked_list_t instead of having a separate linked list impl for processes
bool process_memory_add_region(process_page_list_t* page_list, uintptr_t base, size_t page_count) {
	if (page_list == NULL || page_count == 0) return false;
	if (page_list->region_count >= page_list->region_capacity) {
		size_t new_cap = page_list->region_capacity * 2;
		process_region_t* new_regions = kmalloc(new_cap * sizeof(process_region_t));
		if (!new_regions) return false;
		if (page_list->regions) {
			memcp((unsigned char*)page_list->regions, (unsigned char*)new_regions,
			      page_list->region_count * sizeof(process_region_t));
			kfree(page_list->regions);
		}
		page_list->regions = new_regions;
		page_list->region_capacity = new_cap;
	}
	page_list->regions[page_list->region_count].base = base;
	page_list->regions[page_list->region_count].page_count = page_count;
	page_list->region_count++;
	return true;
}

bool process_memory_remove_region(process_page_list_t* page_list, uintptr_t base) {
	if (page_list == NULL || page_list->region_count == 0) return false;
	for (size_t i = 0; i < page_list->region_count; ++i) {
		if (page_list->regions[i].base == base) {
			page_list->regions[i] = page_list->regions[page_list->region_count - 1];
			page_list->region_count--;
			return true;
		}
	}
	return false;
}

bool process_memory_find_region(const process_page_list_t* page_list, uintptr_t base, size_t* out_page_count) {
	if (page_list == NULL) return false;
	for (size_t i = 0; i < page_list->region_count; ++i) {
		if (page_list->regions[i].base == base) {
			if (out_page_count) *out_page_count = page_list->regions[i].page_count;
			return true;
		}
	}
	return false;
}