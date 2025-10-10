#include "processes.h"

#include "stdio.h"

#include "dynamic_mem.h"
#include "stddef.h"
#ifdef VGA_VESA
#include "vesa_text.h"
#else
#include "vga_text.h"
#endif
#include "conversions.h"


/******* Tasks *******/

extern void save_task_state(struct task_state *state, void* new_eip);
extern void load_task_state(struct task_state *state, void* return_point);

void kprint_task_state(struct task_state *state) {
    printf("eax: %x\n", state->eax);
    printf("ebx: %x\n", state->ebx);
    printf("ecx: %x\n", state->ecx);
    printf("edx: %x\n", state->edx);
    printf("esi: %x\n", state->esi);
    printf("edi: %x\n", state->edi);
    printf("ebp: %x\n", state->ebp);
    printf("esp: %x\n", state->esp);
    printf("eip: %x\n", (uint32_t) state->eip);
    printf("eflags: %x\n", state->eflags);
}

void populate_task_noargs(state_t* state, void* code){
    state -> eip = code;
}


/******* PROCESSES *******/

#define MAX_PROCESSES 1000
process_t **processes;
uint32_t allocated_processes = MAX_PROCESSES;
uint32_t cur_pid = 0; // for now, PIDs are just indices in the above array
uint32_t max_pid = 0;

bool scheduler_active = false;

FILE *stdin = NULL;
FILE *stderr = NULL;
FILE *stdout = NULL;

void init_scheduler() {
    processes = kmalloc(sizeof(process_t *) * MAX_PROCESSES);
}

void scheduler_daemon () {

    if (scheduler_active) {
        kprint("Telling current process to shut up... ");
        process_t *cur_proc = processes[cur_pid];

        // check if no process running
        if (cur_proc == NULL){
            kprint(" ...No process currently running\n");
            return;
        }

        kprint(" ... Process 0x"); kprint(tostring_inplace(cur_pid, 16)); kprint(" notified");
        //kprint(tostring_inplace(cur_pid, 16));
        //kprint(" notified\n");

        // save_task_state(cur_proc -> state, cur_proc -> state -> eip);
        cur_proc->must_relinquish = true;
    }
}

void begin_execution() {

    if (processes[cur_pid] == NULL) return;

    kprint("code to be executed: 0x"); kprint(tostring_inplace((uint32_t)(processes[cur_pid]->state->eip), 16)); kprint("\n");
    //kprint(tostring_inplace(processes[cur_pid] -> state -> eip, 16));
    //kprint("\n");

    load_task_state(processes[cur_pid] -> state, &&END_PORCAMADO);

    END_PORCAMADO:
    printf("finito\n");


    // cannot return! new stack.
    for(;;);
}

void execute_next () {
    uint32_t i = 0;

    do {
        cur_pid = (cur_pid + 1) % 1000;
        i++;

        if (i > 1000){
            kprint("no next process found");
            return;
        }
    }
    while (processes [cur_pid] == NULL);


    load_task_state(processes[cur_pid] -> state, &&end_of_scheduler);

    end_of_scheduler:

    return;
}


void try_to_relinquish(){
    process_t * cur_proc = processes[cur_pid];
    if (cur_proc -> must_relinquish) {
        save_task_state(cur_proc -> state, &&restart_point);
        cur_proc -> must_relinquish = false;
        execute_next();
    }
    restart_point:

    return;
}

void try_to_terminate(){
    process_t * cur_proc = processes[cur_pid];
    if (cur_proc -> must_relinquish) {
        processes[cur_pid] = NULL;
        execute_next();
    }
}

process_t* schedule_process(void * code){

    if(max_pid == allocated_processes){
        const uint32_t allocated = allocated_processes + 20;
        krealloc(processes[0], max_pid * sizeof(process_t *), allocated * sizeof(process_t *));
        allocated_processes = allocated;
    } else if (max_pid > allocated_processes) {
        kprint("Illegal schedule process number, halting.");
        for (;;){}
    }

    process_t* res = kmalloc(sizeof(process_t));
    state_t* s = kmalloc(sizeof(state_t));

    // TEMPORARY - set stack and registers here instead of copying them
    save_task_state(s, 0);

    // PROBLEM if move stack lose reference to lots of previously defined variables hehe.
    // s -> ebp = 0x10000;
    // s -> esp = 0x10000;


    populate_task_noargs(s, code);
    res -> state = s;
    res -> must_relinquish = false;

    processes[max_pid] = res;

    max_pid += 1;

    return res;
}

process_t *get_current_process() {
    if (!scheduler_active) {
        return NULL;
    }
    return processes[cur_pid];
}

process_t *get_process_by_pid(uint32_t pid) {
    return processes[pid];
}