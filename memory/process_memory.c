#include "process_memory.h"
#include "dynamic_mem.h"
#include "mem.h"
#include "stddef.h"

#define INITIAL_PAGE_CAPACITY 16

bool process_memory_init(ProcessPageList* page_list) {
	if (page_list == NULL) {
		return false;
	}
		
	page_list->pages = kmalloc(INITIAL_PAGE_CAPACITY * sizeof(uintptr_t));
	if (page_list->pages == NULL) {
		return false;
	}

	page_list->page_count = 0;
	page_list->page_capacity = INITIAL_PAGE_CAPACITY;
	return true;
}

bool process_memory_add_page(ProcessPageList* page_list, uintptr_t page_addr) {
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
			memcp(page_list->pages, new_pages, 
				  page_list->page_count * sizeof(uintptr_t));
			kfree(page_list->pages);
		}

		page_list->pages = new_pages;
		page_list->page_capacity = new_capacity;
	}

	page_list->pages[page_list->page_count++] = page_addr;
	return true;
}

bool process_memory_remove_page(ProcessPageList* page_list, uintptr_t page_addr) {
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

bool process_memory_owns_page(const ProcessPageList* page_list, uintptr_t page_addr) {
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

size_t process_memory_get_page_count(const ProcessPageList* page_list) {
	if (page_list == NULL) {
		return 0;
	}
	return page_list->page_count;
}

void process_memory_cleanup(ProcessPageList* page_list) {
	if (page_list == NULL) {
		return;
	}

	if (page_list->pages != NULL) {
		kfree(page_list->pages);
		page_list->pages = NULL;
	}

	page_list->page_count = 0;
	page_list->page_capacity = 0;
}

uintptr_t process_memory_get_page_at(const ProcessPageList* page_list, size_t index) {
	if (page_list == NULL || index >= page_list->page_count) {
		return 0;
	}

	return page_list->pages[index];
}