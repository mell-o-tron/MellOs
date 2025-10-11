#include "circular_queue.h"
#include "dynamic_mem.h"
#include "stddef.h"

CircularQueue* cqueue_init(uint32_t capacity) {
    if (capacity == 0) {
        return NULL;
    }
    CircularQueue* q = (CircularQueue*)kmalloc(sizeof(CircularQueue));
    if (!q) return NULL;
    q->buffer = (void**)kmalloc(sizeof(void*) * capacity);
    if (!q->buffer) {
        kfree(q);
        return NULL;
    }
    q->capacity = capacity;
    q->read_idx = 0;
    q->write_idx = 0;
    q->size = 0;
    for (uint32_t i = 0; i < capacity; ++i) q->buffer[i] = (void*)0;
    return q;
}

void cqueue_destroy(CircularQueue* q) {
    if (!q) return;
    if (q->buffer) {
        kfree(q->buffer);
        q->buffer = NULL;
    }
    kfree(q);
}

int cqueue_get_queue_size(CircularQueue* q) {
    if (!q) return -1;
    return (int)q->size;
}

int cqueue_get_queue_capacity(CircularQueue* q) {
    if (!q) return -1;
    return (int)q->capacity;
}

int cqueue_get_free_space(CircularQueue* q) {
    if (!q) return -1;
    return (int)(q->capacity - q->size);
}

int cqueue_get_used_space(CircularQueue* q) {
    if (!q) return -1;
    return (int)q->size;
}

int cqueue_get_read_index(CircularQueue* q) {
    if (!q) return -1;
    return (int)q->read_idx;
}

int cqueue_get_write_index(CircularQueue* q) {
    if (!q) return -1;
    return (int)q->write_idx;
}

int cqueue_enqueue(CircularQueue* q, void* data) {
    if (!q) return -1;
    if (q->size == q->capacity) return -1; // full
    q->buffer[q->write_idx] = data;
    q->write_idx = (q->write_idx + 1) % q->capacity;
    q->size++;
    return 0;
}

void* cqueue_dequeue(CircularQueue* q) {
    if (!q || q->size == 0) {
        return (void*)0;
    }
    void* out = q->buffer[q->read_idx];
    q->buffer[q->read_idx] = (void*)0;
    q->read_idx = (q->read_idx + 1) % q->capacity;
    q->size--;
    return out;
}

void* cqueue_peek(CircularQueue* q) {
    if (!q || q->size == 0) return (void*)0;
    return q->buffer[q->read_idx];
}


