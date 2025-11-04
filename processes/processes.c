#include "processes.h"

#include "stdio.h"

#include "dynamic_mem.h"
#include "mem.h"
#include "stddef.h"

#include <mellos/kernel/kernel_stdio.h>
#ifdef CONFIG_GFX_VESA
#include "vesa_text.h"
#else
#include "vga_text.h"
#endif
#include "assert.h"
#include "mellos/kernel/stdio_devices.h"
#include "port_io.h"

/******* Tasks *******/

extern void save_task_state(struct task_state* state, void* new_eip);
extern void load_task_state(struct task_state* state);

int errno = 0;

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

process_t* create_empty_task() {
	process_t* res = kmalloc(sizeof(process_t));
	assert_msg(res != NULL, "Failed to allocate memory for new process");
	state_t* s = kmalloc(sizeof(state_t));
	assert_msg(s != NULL, "Failed to allocate memory for new process state");

	res->state = s;
	res->must_relinquish = false;

	// Initialize process page list structure
	res->page_list = kmalloc(sizeof(process_page_list_t));
	assert_msg(res->page_list != NULL, "Failed to allocate process page list");
	bool pm_ok = process_memory_init(res->page_list);
	assert_msg(pm_ok, "Failed to initialize process page list");

	memset(s, 0, sizeof(state_t));
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
	asm volatile("movl 4(%%ebp), %0"
	             : "=r"(ret_addr) // output
	);
	// printf("Return address: 0x%x\n", ret_addr);

	// Send End-of-Interrupt (EOI) signal to the PICs
	outb(0x20, 0x20);

	// Set up the stack to mimic an interrupt return
	asm volatile("sti\n\t"          // Enable interrupts
	             "add 8, %%ebp\n\t" // Skip over saved ebp and return address
	             // Push values in the order expected by iret: eip, cs, eflags, esp, ss
	             "push $0x10\n\t" // ss (kernel data segment selector, adjust if needed)
	             "push %%esp\n\t" // esp (current stack pointer)
	             "pushf\n\t"      // eflags (push current flags)
	             "push $0x08\n\t" // cs (kernel code segment selector, adjust if needed)
	             "push %0\n\t"    // eip (return address)
	             :
	             : "r"(ret_addr)
	             : "memory");

	// Finally, use iret to jump to the task
	asm volatile("iret");
}

process_t* create_task(void* code) {
	process_t* res = create_empty_task();

	const int32_t stack_size = 0x1000; // 4KB stack

	uint32_t* stack = kmalloc(stack_size);
	assert_msg(stack != NULL, "Failed to allocate stack");

	// Set up the initial stack frame for the new task
	uint32_t* cur_stack = stack + stack_size - 4;
	// Push default values for the registers that will be popped by the asm function

	// printf("Creating task with code at 0x%x and stack at 0x%x\n", (uint32_t)code,
	// (uint32_t)stack);
	*(--cur_stack) = (uint32_t)code;         // eip
	*(--cur_stack) = (uint32_t)init_process; // eip
	*(--cur_stack) = 4;                      // ebx
	*(--cur_stack) = 3;                      // edi
	*(--cur_stack) = 2;                      // esi
	*(--cur_stack) = (uint32_t)stack;        // ebp

	res->state->stack = (void*)cur_stack; // Point to top of stack

	return res;
}

/******* PROCESSES *******/

#define MAX_PROCESSES 1000
process_t** processes;
uint32_t allocated_processes = MAX_PROCESSES;
uint32_t cur_pid = 0; // for now, PIDs are just indices in the above array
uint32_t max_pid = 0;

bool scheduler_active = false;

void init_scheduler() {
	scheduler_active = true;
	uint32_t size = sizeof(process_t*) * MAX_PROCESSES;
	processes = kmalloc(size);

	processes[0] = create_empty_task();

	init_stdio_devices(processes[0]);

	processes[0]->stdout = open_fd_standalone(FD_TYPE_PIPE, O_WRONLY, FD_PERM_WRITE, NULL);
	processes[0]->stdin = open_fd_standalone(FD_TYPE_PIPE, O_RDONLY, FD_PERM_READ, NULL);
	processes[0]->stderr = open_fd_standalone(FD_TYPE_PIPE, O_WRONLY, FD_PERM_WRITE, NULL);
	open_pipe(processes[0]->stdin, NULL, 128);
	open_pipe(NULL, processes[0]->stdout, 128);
	open_pipe(NULL, processes[0]->stderr, 128);

	max_pid = 1;
}

void scheduler_daemon() {

	if (scheduler_active) {
		// printf("Telling current process to shut up... ");
		process_t* cur_proc = processes[cur_pid];

		// check if no process running
		if (cur_proc == NULL) {
			// printf(" ...No process currently running\n");
			return;
		}

		// printf(" ... Process 0x%x notified", cur_pid);
		// kprint(tostring_inplace(cur_pid, 16));
		// kprint(" notified\n");

		// save_task_state(cur_proc -> state, cur_proc -> state -> eip);
		cur_proc->must_relinquish = true;
		try_to_relinquish();
	}
	return;
}

void execute_next() {
	uint32_t i = 0;
	uint32_t prev_pid = cur_pid;
	do {
		cur_pid = (cur_pid + 1) % 1000;
		i++;

		if (i > 1000) {
			kfprintf(stderr, "No next process found");
			return;
		}
	} while (processes[cur_pid] == NULL);

	switch_task(processes[prev_pid], processes[cur_pid]);
	return;
}

void try_to_relinquish() {
	process_t* cur_proc = processes[cur_pid];
	if (cur_proc->must_relinquish) {
		cur_proc->must_relinquish = false;
		execute_next();
	}
	return;
}

void try_to_terminate() {
	process_t* cur_proc = processes[cur_pid];
	if (cur_proc->must_relinquish) {
		processes[cur_pid] = NULL;
		execute_next();
	}
}

process_t* schedule_process(void* code, process_t* parent, fd_t* stdin_target, fd_t* stdout_target,
                            fd_t* stderr_target) {

	if (max_pid >= allocated_processes) {
		const uint32_t allocated = allocated_processes + 20;
		processes =
		    krealloc(processes, max_pid * sizeof(process_t*), allocated * sizeof(process_t*));
		allocated_processes = allocated;
	}

	process_t* new_process = create_task(code);

	processes[max_pid] = new_process;
	assert_msg(processes[max_pid] != NULL, "Failed to schedule new process");

	max_pid += 1;

	init_stdio_devices(new_process);

	new_process->stdin = open_fd_standalone(FD_TYPE_PIPE, O_RDONLY, 0, NULL);
	new_process->stdout = open_fd_standalone(FD_TYPE_PIPE, O_WRONLY, 0, NULL);
	new_process->stderr = open_fd_standalone(FD_TYPE_PIPE, O_WRONLY, 0, NULL);

	open_pipe(new_process->stdin, stdin_target, 128);
	open_pipe(stdout_target, new_process->stdout, 128);
	open_pipe(stderr_target, new_process->stderr, 128);

	new_process->parent = parent;

	return new_process;
}

process_t* get_current_process() {
	if (!scheduler_active) {
		return NULL;
	}
	return processes[cur_pid];
}

process_t* get_process_by_pid(uint32_t pid) {
	return processes[pid];
}