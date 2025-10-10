#pragma once
#include <mellos/fd.h>
#include <mellos/pipe.h>

#include "stdio.h"

#include "stddef.h"
#include "stdint.h"
#include "../global/include/stdbool.h"

typedef struct task_state {
    uint32_t    eax, ebx, ecx, edx;
    uint32_t    esi, edi, ebp, esp;
    void*       eip;
    uint32_t    eflags;
    uint16_t    cs, ds, es, fs, gs, ss;
} state_t;

typedef struct process {
    uint32_t pid;
    int32_t errno;
    bool must_relinquish;
    state_t * state;
    uint32_t * page_directory;
    pipe_t *stdin;
    pipe_t *stdout;
    pipe_t *stderr;
} process_t;


extern void save_task_state(struct task_state *state, void* new_eip);
extern void load_task_state(struct task_state *state, void* return_point);

void kprint_task_state(struct task_state *state);

void populate_task_noargs(state_t* state, void* code);

process_t *get_current_process();
process_t *get_process_by_pid(uint32_t pid);

void init_scheduler();
void scheduler_daemon();
void begin_execution();

void try_to_relinquish();
void try_to_terminate();
process_t* schedule_process(void * code);
