/**
 * @file processes.c
 * @brief D-WRR scheduler implementation
 * @author assembler-0
 */

#include "processes.h"

#include "utils/format.h"
#include "spinlock.h"
#include "memory/dynamic_mem.h"
#include "memory/mem.h"
#include "utils/typedefs.h"
#ifdef VGA_VESA
#include "drivers/vesa/vesa_text.h"
#else
#include "drivers/vga_text.h"
#endif
#include "utils/conversions.h"
#include "utils/assert.h"
#include "drivers/port_io.h"


/******* Tasks *******/

extern void save_task_state(struct task_state *state, void* new_eip);
extern void load_task_state(struct task_state *state);
volatile int process_table_lock = 0;

// void kprint_task_state(struct task_state *state) {
//     char tmp[10];

//     kprint("eax: 0x");
//     kprint(tostring(state -> eax, 16, tmp));

//     kprint("\nebx: 0x");
//     kprint(tostring(state -> ebx, 16, tmp));

//     kprint("\necx: 0x");
//     kprint(tostring(state->ecx, 16, tmp));

//     kprint("\nedx: 0x");
//     kprint(tostring(state->edx, 16, tmp));

//     kprint("\nesi: 0x");
//     kprint(tostring(state->esi, 16, tmp));

//     kprint("\nedi: 0x");
//     kprint(tostring(state->edi, 16, tmp));

//     kprint("\nebp: 0x");
//     kprint(tostring(state->ebp, 16, tmp));

//     kprint("\nesp: 0x");
//     kprint(tostring(state->esp, 16, tmp));

//     kprint("\neip: 0x");
//     kprint(tostring((uint32_t)state->eip, 16, tmp));

//     kprint("\neflags: 0x");
//     kprint(tostring(state->eflags, 16, tmp));
//     kprint("\n");
// }

process_t* create_empty_task(){
    irqflags_t flags = SpinLockIrqSave(&process_table_lock);
    process_t* res = kmalloc(sizeof(process_t));
    assert_msg(res != NULL, "Failed to allocate memory for new process");
    state_t* s = kmalloc(sizeof(state_t));
    assert_msg(s != NULL, "Failed to allocate memory for new process state");

    res->state = s;
    res->must_relinquish = false;
    res->weight = 1;
    res->quantum_left = 1;

    memset(s, 0, sizeof(state_t));
    SpinUnlockIrqRestore(&process_table_lock, flags);
    return res;
}

void init_process() {
    /* A bit of dark magic:
     * We want to preempt tasks using the timer interrupt.
     * If we simply switch to the task, it will jump to code
     * that is supposed to be run in a non-interrupt context.
     * This means that the CPU never leaves the interrupt state,
     * and thus never re-enables interrupts.
     * To solve this, we manually set up the stack as if we were
     * leaving an interrupt, and then use iret to jump to the task.
    */
    // kprint("Process initialized!\n");
    uint32_t* ret_addr;
    // Get the second value on the stack (the return address)
    asm volatile (
        "movl 4(%%ebp), %0"
        : "=r" (ret_addr) // output
    );
    // printf("Return address: 0x%x\n", ret_addr);

    // Send End-of-Interrupt (EOI) signal to the PICs
    outb(0x20, 0x20);

    // Set up the stack to mimic an interrupt return
    asm volatile (
        "sti\n\t"            // Enable interrupts
        "add 8, %%ebp\n\t"   // Skip over saved ebp and return address
        // Push values in the order expected by iret: eip, cs, eflags, esp, ss
        "push $0x10\n\t"     // ss (kernel data segment selector, adjust if needed)
        "push %%esp\n\t"     // esp (current stack pointer)
        "pushf\n\t"          // eflags (push current flags)
        "push $0x08\n\t"     // cs (kernel code segment selector, adjust if needed)
        "push %0\n\t"        // eip (return address)
        :
        : "r"(ret_addr)
        : "memory"
    );

    // Finally, use iret to jump to the task
    asm volatile ("iret");
}

process_t* create_task(void* code){
    process_t* res = create_empty_task();

    const int stack_size = 0x1000; // 4KB stack

    uint32_t* stack = kmalloc(stack_size);
    res->state->stack_base = stack;
    assert_msg(stack != NULL, "Failed to allocate stack");

    // Set up the initial stack frame for the new task
    uint32_t* cur_stack = stack + stack_size - 4;
    // Push default values for the registers that will be popped by the asm function

    // printf("Creating task with code at 0x%x and stack at 0x%x\n", (uint32_t)code, (uint32_t)stack);
    *(--cur_stack) = (uint32_t)code; // eip
    *(--cur_stack) = (uint32_t)init_process; // eip
    *(--cur_stack) = 4; // ebx
    *(--cur_stack) = 3; // edi
    *(--cur_stack) = 2; // esi
    *(--cur_stack) = (uint32_t)stack; // ebp

    res->state->stack = cur_stack; // Point to top of stack
    res->weight = 1; // Default weight, can be set dynamically
    res->quantum_left = res->weight;
    res->status = PROC_READY;

    return res;
}


/******* PROCESSES *******/

#define MAX_PROCESSES 1000
process_t **processes;
uint32_t allocated_processes = MAX_PROCESSES;
uint32_t cur_pid = 0; // for now, PIDs are just indices in the above array
uint32_t max_pid = 0;

bool scheduler_active = false;

void init_scheduler() {
    scheduler_active = true;
    processes = kmalloc(sizeof(process_t *) * MAX_PROCESSES);

    processes[0] = create_empty_task();
    processes[0]->weight = 1;
    processes[0]->quantum_left = 1;
    max_pid = 1;
}

// D-WRR: Dynamic Weighted Round Robin with dynamic adjustment
void execute_next () {
    irqflags_t flags = SpinLockIrqSave(&process_table_lock);
    uint32_t prev_pid = cur_pid;
    uint32_t total_weight = 0;
    for (uint32_t i = 0; i < max_pid; i++) {
        if (processes[i] != NULL) {
            total_weight += processes[i]->weight;
        }
    }
    if (total_weight == 0) {
        SpinUnlock(&process_table_lock);
        kprint("no next process found");
        return;
    }
    // Dynamic adjustment: increase weight for processes that were skipped, decrease for those that ran
    for (uint32_t i = 0; i < max_pid; i++) {
        if (processes[i] != NULL) {
            if (i == cur_pid) {
                if (processes[i]->weight > 1) processes[i]->weight--;
            } else {
                if (processes[i]->weight < 10) processes[i]->weight++;
            }
        }
    }
    uint32_t attempts = 0;
    while (attempts < max_pid) {
        cur_pid = (cur_pid + 1) % max_pid;
        process_t* proc = processes[cur_pid];
        if (proc && proc->quantum_left > 0) {
            proc->quantum_left--;
            break;
        }
        attempts++;
    }
    // Reset quantum for all if exhausted
    if (attempts == max_pid) {
        for (uint32_t i = 0; i < max_pid; i++) {
            if (processes[i] != NULL) {
                processes[i]->quantum_left = processes[i]->weight;
            }
        }
        // Try again
        for (uint32_t i = 0; i < max_pid; i++) {
            cur_pid = (cur_pid + 1) % max_pid;
            process_t* proc = processes[cur_pid];
            if (proc && proc->quantum_left > 0) {
                proc->quantum_left--;
                break;
            }
        }
    }
    SpinUnlockIrqRestore(&process_table_lock, flags);
    switch_task(processes[prev_pid], processes[cur_pid]);
    return;
}

void scheduler_daemon () {
    if (scheduler_active) {
        irqflags_t flags = SpinLockIrqSave(&process_table_lock);
        process_t *cur_proc = processes[cur_pid];
        if (cur_proc == NULL || cur_proc->status == PROC_TERMINATED) {
            SpinUnlockIrqRestore(&process_table_lock, flags);
            execute_next();
            return;
        }
        cur_proc->must_relinquish = true;
        cur_proc->status = PROC_READY;
        SpinUnlockIrqRestore(&process_table_lock, flags);
        try_to_relinquish();
    }
    return;
}

void try_to_relinquish(){
    SpinLock(&process_table_lock);
    process_t * cur_proc = processes[cur_pid];
    if (cur_proc && cur_proc->must_relinquish) {
        cur_proc->must_relinquish = false;
        cur_proc->status = PROC_READY;
        SpinUnlock(&process_table_lock);
        execute_next();
        return;
    }
    SpinUnlock(&process_table_lock);
    return;
}

void try_to_terminate(){
    SpinLock(&process_table_lock);
    process_t * cur_proc = processes[cur_pid];
    if (cur_proc && cur_proc->must_relinquish) {
        cur_proc->status = PROC_TERMINATED;
        if(cur_proc->state) {
            if(cur_proc->state->stack_base) {
                kfree(cur_proc->state->stack_base);
            }
            kfree(cur_proc->state);
        }
        kfree(cur_proc);
        processes[cur_pid] = NULL;
        SpinUnlock(&process_table_lock);
        execute_next();
        return;
    }
    SpinUnlock(&process_table_lock);
}

process_t* schedule_process(void * code){

    if(max_pid >= allocated_processes){
        const uint32_t allocated = allocated_processes + 20;
        processes = krealloc(processes, max_pid * sizeof(process_t *), allocated * sizeof(process_t *));
        allocated_processes = allocated;
    } else if (max_pid > allocated_processes) {
        kprint("Illegal schedule process number, halting.");
        for (;;){}
    }

    process_t* new_process = create_task(code);

    processes[max_pid] = new_process;
    assert_msg(processes[max_pid] != NULL, "Failed to schedule new process");

    max_pid += 1;

    return new_process;
}

void kill_task(uint32_t pid) {
    // Check whether the PID is valid
    if (pid >= allocated_processes || processes[pid] == NULL) {
        kprint("Invalid PID: ");
        kprint(tostring_inplace(pid, 10));
        kprint("\n");
        return;
    }

    // Do not allow killing the main process (PID 0)
    if (pid == 0) {
        kprint("Cannot kill the main process (PID 0)\n");
        return;
    }

    process_t* task_to_kill = processes[pid];

    // If it is the currently running process, force it to relinquish the CPU
    if (pid == cur_pid) {
        task_to_kill->must_relinquish = true;
        // TODO: This is problematic: if you relinquish here, the rest of this
        // function will not execute. The task should probably be scheduled to
        // be killed by another task
        try_to_relinquish();
    }

    if (task_to_kill->state) {
        // Free the stack memory
        if(task_to_kill->state->stack_base) {
            kfree(task_to_kill->state->stack_base);
        }
        // Free the memory of the process state
        kfree(task_to_kill->state);
    }

    // Free the memory of the process itself
    kfree(task_to_kill);

    // Remove the entry from the process array
    processes[pid] = NULL;

    kprint("Task ");
    kprint(tostring_inplace(pid, 10));
    kprint(" killed successfully\n");
}

void list_processes() {
    kprint("Processes:\n");
    kprint("PID\tState\n");
    kprint("---\t------\n");

    for (uint32_t i = 0; i < max_pid; i++) {
        if (processes[i] != NULL) {
            kprint(tostring_inplace(i, 10));
            kprint("\t");
            switch (processes[i]->status) {
                case PROC_READY: kprint("READY"); break;
                case PROC_RUNNING: kprint("RUNNING"); break;
                case PROC_BLOCKED: kprint("BLOCKED"); break;
                case PROC_TERMINATED: kprint("TERMINATED"); break;
                default: kprint("UNKNOWN"); break;
            }
            kprint("\n");
        }
    }

    if (max_pid == 0 || (max_pid == 1 && processes[0] != NULL)) {
        kprint("No additional processes running\n");
    }
}
