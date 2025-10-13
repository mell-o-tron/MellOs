#pragma once

#include <linked_list.h>

#include "mellos/pipe.h"

typedef struct task_state {
    void* stack;
} state_t;

typedef struct process {
    uint32_t pid;
    int32_t errno;
    state_t * state;
    uint32_t * page_directory;
    bool must_relinquish;
    fd_table_t fd_table;
    struct process *parent;
    linked_list_t *children_list;
    fd_t *stdin;
    fd_t *stdout;
    fd_t *stderr;
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
process_t* schedule_process(void *code, process_t *parent, fd_t *stdin_target, fd_t *stdout_target, fd_t *stderr_target);
void add_child(process_t *parent, process_t *child);
void remove_child(process_t *parent, process_t *child);