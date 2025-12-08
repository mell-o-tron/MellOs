#pragma once
#include "../utils/typedefs.h"

typedef struct task_state {
    void* stack;
    void* stack_base;
} state_t;

typedef struct process {
    uint32_t pid;
    state_t * state;
    uint32_t * page_directory;
    bool must_relinquish;
    uint32_t weight; // D-WRR weight
    uint32_t quantum_left; // Remaining quantum for this round
} process_t;


extern void __attribute__((__cdecl__)) switch_task(process_t* current, process_t* new);

// void kprint_task_state(struct task_state *state);

process_t* create_task(void* code);

void init_scheduler();
void scheduler_daemon ();

void try_to_relinquish();
void try_to_terminate();
process_t* schedule_process(void * code);
void kill_task(uint32_t pid);
void list_processes();
