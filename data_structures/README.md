[← Main README](../README.md)

# data_structures

This directory contains data structure implementations for MellOs.

## allocator.h / allocator.c
Implements a bitmap-based memory allocator.

- **allocator_t**: Structure holding a bitmap, size, and granularity for memory allocation.
- **get_allocator_bitmap(allocator_t\*)**: Returns the bitmap used by the allocator.
- **set_alloc_bitmap(allocator_t\*, bitmap_t, uint32_t length)**: Initializes the allocator's bitmap and sets the managed size.
- **allocate(allocator_t\*, size_t n)**: Allocates a contiguous region of memory of at least `n` units, using a first-fit strategy. Returns a pointer to the allocated memory.
- **allocator_free(allocator_t\*, uint32_t index, size_t size)**: Frees a region of memory starting at `index` of the given size.

## bitmap.h / bitmap.c
Implements a simple bitmap for tracking resource usage (e.g., memory blocks).

- **bitmap_t**: Typedef for a pointer to an array of bytes representing the bitmap.
- **set_bitmap(bitmap_t, size_t i)**: Sets the bit at index `i`.
- **unset_bitmap(bitmap_t, size_t i)**: Clears the bit at index `i`.
- **get_bitmap(bitmap_t, size_t i)**: Returns 1 if the bit at index `i` is set, 0 otherwise.
- **create_bitmap(void\* loc, size_t n)**: Initializes a bitmap of `n` bits at the given memory location.

## circular_buffer.h / circular_buffer.c
Implements a circular (ring) buffer for character data (used by the keyboard driver).

- **cbuffer_t**: Structure with an array, top/bottom indices, and size.
- **add_to_cbuffer(cbuffer_t\*, char c, bool is_uppercase)**: Adds a character to the buffer, optionally converting to uppercase.
- **get_from_cbuffer(cbuffer_t\*)**: Retrieves and removes the next character from the buffer.
- **rem_from_cbuffer(cbuffer_t\*)**: Removes the next character from the buffer without returning it.

## circular_list.h / circular_list.c
Implements a doubly-linked circular list.

- **CircularList**: Node structure with data pointer, next, and previous pointers.
- **clist_append(CircularList\*\*, void\* data)**: Appends a new node with the given data to the list.
- **clist_remove(CircularList\*\*, void\* data)**: Removes the node containing the given data.
- **clist_print_all(CircularList\*)**: Prints all elements in the list (implementation may depend on kernel output routines).

## list.h / list.c
Implements a singly-linked list.

- **List**: Node structure with data pointer and next pointer.
- **list_append(List\*\*, void\* data)**: Appends a new node with the given data to the list.
- **list_remove(List\*\*, void\* data)**: Removes the node containing the given data.
- **print_all(List\*)**: Prints all elements in the list (implementation may depend on kernel output routines).
