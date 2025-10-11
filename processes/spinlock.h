#pragma once

#include "../utils/typedefs.h"
#include "../cpu/interrupts/irq.h"

typedef bool spinlock_t;

void spinlock_lock(spinlock_t* lock);
void spinlock_unlock(spinlock_t* lock);
bool spinlock_try_lock(spinlock_t* lock);

/* 
 * These variants will prevent a deadlock from happening if the lock is acquired, 
 * and then acquired again on the same CPU in an interrupt handler (except for an NMI)
 */
void spinlock_lock_irq_save(spinlock_t* lock, irqflags_t* flags);
void spinlock_unlock_irq_restore(spinlock_t* lock, irqflags_t* flags);
bool spinlock_try_lock_irq_save(spinlock_t* lock, irqflags_t* flags);

static inline void spinlock_init(spinlock_t* lock) {
	__atomic_store_n(lock, false, __ATOMIC_RELAXED);
}
