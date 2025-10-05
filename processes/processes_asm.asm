[bits 32]
eax_buffer:
    dd 0

ebx_buffer:
    dd 0

; typedef struct task_state {
;     void* stack;
; } state_t;

struc process
    .pid resd 1
    .state resd 1
    .page_directory resd 1
    .must_relinquish resb 1
endstruc

struc task_state
    .stack resd 1
endstruc

global switch_task
; void switch_task(process_t* current, process_t* new);
switch_task:
    ; Save current task state
    ; Calling convention is cdecl (since we are using gcc on x86).
    ; So eax, ecx, edx are caller-saved, others are callee-saved.
    push ebx
    push esi
    push edi
    push ebp

    ; Get the two parameters.
    ; Since we pushed 4 * 4byte registers, we compute the offsets accordingly.
    mov esi, [esp + 4 * (4 + 1)] ; current process pointer
    mov edi, [esp + 4 * (4 + 2)] ; new process

    ; Save current esp to current process state
    mov eax, [esi + process.state]           ; eax = pointer to task_state
    mov [eax + task_state.stack], esp        ; store esp into task_state.stack
    ; Load new task state
    mov eax, [edi + process.state]           ; eax = pointer to task_state
    mov esp, [eax + task_state.stack]

    ; Restore registers
    pop ebp
    pop edi
    pop esi
    pop ebx
    ret



global top_of_stack
top_of_stack:
    mov eax, [esp]
    ret
