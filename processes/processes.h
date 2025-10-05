#pragma once
#include "../utils/typedefs.h"

typedef struct task_state {
    uint32_t    eax, ebx, ecx, edx;
    uint32_t    esi, edi, ebp, esp;
    void*       eip;
    uint32_t    eflags;
    uint16_t    cs, ds, es, fs, gs, ss;
} state_t;

typedef struct process {
    uint32_t pid;
    bool must_relinquish;
    state_t * state;
    uint32_t * page_directory;
} process_t;


extern void save_task_state(struct task_state *state, void* new_eip);
extern void load_task_state(struct task_state *state);

void kprint_task_state(struct task_state *state);

void populate_task_noargs(state_t* state, void* code);

void init_scheduler();
void scheduler_daemon ();
void begin_execution();

void try_to_relinquish();
void try_to_terminate();
process_t* schedule_process(void * code);
