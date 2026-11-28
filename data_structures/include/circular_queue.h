#pragma once

#include <stdint.h>

// Instance-based circular queue of void* pointers.
// Non-blocking, not thread-safe.

typedef struct {
    void **buffer;      // storage for pointers (allocated)
    uint32_t capacity;  // max number of elements
    uint32_t read_idx;  // index of next element to dequeue
    uint32_t write_idx; // index to write next element
    uint32_t size;      // current number of elements
} CircularQueue;

// Create a new queue with the given capacity. Returns NULL on failure.
// Memory for the queue and its buffer is dynamically allocated.
CircularQueue* cqueue_init(uint32_t capacity);
// Destroy a queue created with cqueue_init. Safe to call with NULL.
void cqueue_destroy(CircularQueue* q);

// Query functions for a given queue instance.
int cqueue_get_queue_size(CircularQueue* q);
int cqueue_get_queue_capacity(CircularQueue* q);
int cqueue_get_free_space(CircularQueue* q);
int cqueue_get_used_space(CircularQueue* q);

int cqueue_get_read_index(CircularQueue* q);
int cqueue_get_write_index(CircularQueue* q);

// Operations on a given queue instance.
// enqueue returns 0 on success, -1 if full or invalid.
int cqueue_enqueue(CircularQueue* q, void* data, size_t);
// dequeue returns element pointer or NULL if empty.
void* cqueue_dequeue(CircularQueue* q);
// peek returns the next element without removing it, or NULL if empty.
void* cqueue_peek(CircularQueue* q);