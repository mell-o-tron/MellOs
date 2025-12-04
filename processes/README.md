[← Main README](../README.md)

# processes

This directory contains process management code for MellOs, including process control, scheduling, and synchronization.

## processes.h / processes.c
- **task_state**: Structure representing the CPU state of a task.
- **process**: Structure representing a process (PID, page directory, etc.).
- **switch_task(process_t\* current, process_t\* new)**: Switches execution from one process to another.
- **init_scheduler()**: Initializes the process scheduler.
- **scheduler_daemon()**: Scheduler daemon, summoned at specified intervals by the timer.
- **try_to_relinquish()**: Attempts to yield the CPU.
- **try_to_terminate()**: Attempts to terminate the current process.
- **schedule_process(void\* code)**: Schedules a new process.

## spinlock.h / spinlock.c
- **spinlock_t**: Boolean type for spinlocks.
- **spinlock_lock(spinlock_t\* lock)**: Acquires a spinlock.
- **spinlock_unlock(spinlock_t\* lock)**: Releases a spinlock.
- **spinlock_lock_irq_save(spinlock_t\* lock, irqflags_t\* flags)**: Acquires a spinlock and saves interrupt flags.
- **spinlock_unlock_irq_restore(spinlock_t\* lock, irqflags_t\* flags)**: Releases a spinlock and restores interrupt flags.

## stack_utils.h
- **PUSH_BS(val), POP_BS_AND_IGNORE(), PEEK_BS()**: Macros for stack manipulation.
