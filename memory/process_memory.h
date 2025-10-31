#pragma once

#include "stdint.h"
#include "stdbool.h"

typedef struct {
	uintptr_t* pages;       // Array of virtual page addresses owned by process
	size_t page_count;      // Current number of pages
	size_t page_capacity;   // Allocated capacity
} ProcessPageList;

/**
 * Initialize the page list for a process
 * Returns true on success, false on failure
 */
bool process_memory_init(ProcessPageList* page_list);

/**
 * Add a page to a process's page list
 * Automatically resizes the array if needed
 * Returns true on success, false on failure (e.g., out of memory)
 */
bool process_memory_add_page(ProcessPageList* page_list, uintptr_t page_addr);

/**
 * Remove a page from a process's page list
 * Returns true if found and removed, false if not found
 */
bool process_memory_remove_page(ProcessPageList* page_list, uintptr_t page_addr);

/**
 * Check if a process owns a specific page
 * Returns true if the page is in the process's page list
 */
bool process_memory_owns_page(const ProcessPageList* page_list, uintptr_t page_addr);

/**
 * Get the number of pages owned by a process
 */
size_t process_memory_get_page_count(const ProcessPageList* page_list);

/**
 * Free all resources associated with a process's page list
 * Should be called when the process is terminated
 */
void process_memory_cleanup(ProcessPageList* page_list);

/**
 * Get page at index (for debugging/iteration)
 * Returns 0 if index is out of bounds
 */
uintptr_t process_memory_get_page_at(const ProcessPageList* page_list, size_t index);