#include "spinlock.h"

void spinlock_lock(spinlock_t* lock) {
    while (__atomic_test_and_set(lock, __ATOMIC_ACQUIRE))
        __builtin_ia32_pause();
}

void spinlock_unlock(spinlock_t* lock) {
    __atomic_clear(lock, __ATOMIC_RELEASE);
}

bool spinlock_try_lock(spinlock_t* lock) {
    return !__atomic_test_and_set(lock, __ATOMIC_ACQ_REL);
}

void spinlock_lock_irq_save(spinlock_t* lock, irqflags_t* flags) {
    *flags = local_irq_save();
    spinlock_lock(lock);
}

void spinlock_unlock_irq_restore(spinlock_t* lock, irqflags_t* flags) {
    spinlock_unlock(lock);
    local_irq_restore(*flags);
}

bool spinlock_try_lock_irq_save(spinlock_t* lock, irqflags_t* flags) {
    *flags = local_irq_save();
    if (spinlock_try_lock(lock))
        return true;
    local_irq_restore(*flags);
    return false;
}
