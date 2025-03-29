#include "processes.h"
#include "../memory/dynamic_mem.h"
#include "../utils/typedefs.h"
#ifdef VGA_VESA
#include "../drivers/vesa/vesa_text.h"
#else
#include "../drivers/vga_text.h"
#endif
#include "../utils/conversions.h"


/******* Tasks *******/

extern void save_task_state(struct task_state *state, void* new_eip);
extern void load_task_state(struct task_state *state, void* return_point);



void kprint_task_state(struct task_state *state) {
    char tmp[10];

    kprint("eax: 0x");
    kprint(tostring(state -> eax, 16, tmp));

    kprint("\nebx: 0x");
    kprint(tostring(state -> ebx, 16, tmp));

    kprint("\necx: 0x");
    kprint(tostring(state->ecx, 16, tmp));

    kprint("\nedx: 0x");
    kprint(tostring(state->edx, 16, tmp));

    kprint("\nesi: 0x");
    kprint(tostring(state->esi, 16, tmp));

    kprint("\nedi: 0x");
    kprint(tostring(state->edi, 16, tmp));

    kprint("\nebp: 0x");
    kprint(tostring(state->ebp, 16, tmp));

    kprint("\nesp: 0x");
    kprint(tostring(state->esp, 16, tmp));

    kprint("\neip: 0x");
    kprint(tostring((uint32_t)state->eip, 16, tmp));

    kprint("\neflags: 0x");
    kprint(tostring(state->eflags, 16, tmp));
    kprint("\n");
}

void populate_task_noargs(state_t* state, void* code){
    state -> eip = code;
}


/******* PROCESSES *******/

process_t * processes [1000];
uint32_t cur_pid = 0; // for now, PIDs are just indices in the above array
uint32_t max_pid = 0;

bool scheduler_active = false;

void scheduler_daemon (){

    if (scheduler_active) {
        kprint("Telling current process to shut up... ");
        process_t * cur_proc = processes[cur_pid];

        // check if no process running
        if (cur_proc == NULL){
            kprint(" ...No process currently running\n");
            return;
        }

        kprint(" ... Process 0x");
        kprint(tostring_inplace(cur_pid, 16));
        kprint(" notified\n");

        // save_task_state(cur_proc -> state, cur_proc -> state -> eip);
        cur_proc ->must_relinquish = true;
    }
    return;
}

void begin_execution() {

    if (processes[cur_pid] == NULL) return;

    kprint("code to be executed: ");
    kprint(tostring_inplace(processes[cur_pid] -> state -> eip, 16));
    kprint("\n");

    load_task_state(processes[cur_pid] -> state, &&END_PORCAMADO);

    END_PORCAMADO:
    kprint("finito\n");


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

    if(max_pid >= 1000){
        kprint("no more processes available!\n");
        for(;;);
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

