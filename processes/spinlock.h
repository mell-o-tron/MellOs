/// Ported from VoidFrame by assembler-0
/// Public domain as of 11-10-25 (dd-mm-yy)
#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "stdbool.h"
#include "cpu/irq.h"

#define DEADLOCK_TIMEOUT_CYCLES 100000000ULL
#define MAX_BACKOFF_CYCLES 1024
// move this somewhere else
static inline uint64_t __attribute__((always_inline)) rdtsc(void) {
    uint32_t lo, hi;
    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

// Exponential backoff delay
static inline void backoff_delay(uint64_t cycles) {
    uint64_t start = rdtsc();
    while (rdtsc() - start < cycles) {
        __builtin_ia32_pause();
    }
}

// Advanced spinlock with multiple anti-race mechanisms
static inline void SpinLock(volatile int* lock) {
    uint64_t start = rdtsc();
    uint64_t backoff = 1;
    uint32_t attempts = 0;

    while (1) {
        // Try to acquire without contention first
        if (!*lock && !__atomic_test_and_set(lock, __ATOMIC_ACQUIRE)) {
            return;
        }

        // Deadlock detection
        if (rdtsc() - start > DEADLOCK_TIMEOUT_CYCLES) {
            backoff_delay(MAX_BACKOFF_CYCLES);
            start = rdtsc();
            attempts = 0;
            continue;
        }

        attempts++;

        // Adaptive spinning strategy
        if (attempts < 100) {
            // Initial fast spinning with pause
            for (int i = 0; i < 64; i++) {
                if (!*lock) break;
                __builtin_ia32_pause();
            }
        } else {
            // Switch to exponential backoff after many attempts
            backoff_delay(backoff);
            backoff = (backoff * 2) > MAX_BACKOFF_CYCLES ? MAX_BACKOFF_CYCLES : (backoff * 2);
        }
    }
}

// MCS-style queue lock (more fair, less cache bouncing)
typedef struct mcs_node {
    volatile struct mcs_node* next;
    volatile int locked;
} mcs_node_t;

static inline void MCSLock(volatile mcs_node_t** lock, mcs_node_t* node) {
    node->next = NULL;
    node->locked = 1;

    mcs_node_t* prev = __atomic_exchange_n(lock, node, __ATOMIC_ACQUIRE);
    if (prev) {
        prev->next = node;
        while (node->locked) __builtin_ia32_pause();
    }
}

static inline void MCSUnlock(volatile mcs_node_t** lock, mcs_node_t* node) {
    if (!node->next) {
        mcs_node_t* expected = node;
        if (__atomic_compare_exchange_n(lock, &expected, NULL, false, __ATOMIC_RELEASE, __ATOMIC_RELAXED)) {
            return;
        }
        while (!node->next) __builtin_ia32_pause();
    }
    node->next->locked = 0;
}

// Reader-Writer spinlock (if you need shared/exclusive access)
typedef struct {
    volatile int readers;
    volatile int writer;
    volatile uint32_t owner;
    volatile int recursion;
} rwlock_t;

#define RWLOCK_INIT { .readers = 0, .writer = 0, .owner = 0, .recursion = 0 }

static inline void ReadLock(rwlock_t* lock, uint32_t owner_id) {
    if (lock->writer && lock->owner == owner_id) {
        // The current process holds the write lock, so it can "read"
        return;
    }
    while (1) {
        while (lock->writer) __builtin_ia32_pause();
        __atomic_fetch_add(&lock->readers, 1, __ATOMIC_ACQUIRE);
        if (!lock->writer) break;
        __atomic_fetch_sub(&lock->readers, 1, __ATOMIC_RELAXED);
    }
}

static inline void ReadUnlock(rwlock_t* lock, uint32_t owner_id) {
    if (lock->writer && lock->owner == owner_id) {
        __atomic_thread_fence(__ATOMIC_RELEASE);
        return;
    }
    __atomic_fetch_sub(&lock->readers, 1, __ATOMIC_RELEASE);
}

static inline void WriteLock(rwlock_t* lock, uint32_t owner_id) {
    if (lock->writer && lock->owner == owner_id) {
        lock->recursion++;
        return;
    }

    while (__atomic_test_and_set(&lock->writer, __ATOMIC_ACQUIRE)) {
        while (lock->writer) __builtin_ia32_pause();
    }
    while (lock->readers) __builtin_ia32_pause();

    lock->owner = owner_id;
    lock->recursion = 1;
}

static inline void WriteUnlock(rwlock_t* lock) {
    if (lock->recursion <= 0) {
        lock->recursion = 0;
        lock->owner = 0;
        __atomic_clear(&lock->writer, __ATOMIC_RELEASE);
        return;
    }
    if (--lock->recursion == 0) {
        lock->owner = 0;
        __atomic_clear(&lock->writer, __ATOMIC_RELEASE);
    }
}

static inline void SpinUnlock(volatile int* lock) {
    __atomic_clear(lock, __ATOMIC_RELEASE);
}

static inline irqflags_t SpinLockIrqSave(volatile int* lock) {
    irqflags_t flags = local_irq_save_and_cli();
    SpinLock(lock);  // Uses the advanced version above
    return flags;
}

static inline void SpinUnlockIrqRestore(volatile int* lock, irqflags_t flags) {
    __atomic_clear(lock, __ATOMIC_RELEASE);
    local_irq_restore(flags);
}

#endif // SPINLOCK_H
