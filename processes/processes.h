#pragma once

#include "mellos/pipe.h"

#include "stdbool.h"

typedef struct task_state {
    void* stack;
} state_t;

typedef struct process {
    uint32_t pid;
    int32_t errno;
    state_t * state;
    uint32_t * page_directory;
    bool must_relinquish;
    pipe_t *stdin;
    pipe_t *stdout;
    pipe_t *stderr;
} process_t;


extern void __attribute__((__cdecl__)) switch_task(process_t* current, process_t* new);

// void kprint_task_state(struct task_state *state);

process_t* create_task(void* code);

process_t *get_current_process();
process_t *get_process_by_pid(uint32_t pid);

void init_scheduler();
void begin_execution();

void try_to_relinquish();
void try_to_terminate();
process_t* schedule_process(void * code);
