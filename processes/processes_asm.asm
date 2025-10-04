[bits 32]
eax_buffer:
    dd 0

ebx_buffer:
    dd 0

; typedef struct task_state {
;     uint32_t    eax, ebx, ecx, edx;
;     uint32_t    esi, edi, ebp, esp;
;     void*       eip;
;     uint32_t    eflags;
;     uint16_t    cs, ds, es, fs, gs, ss;
; } state_t;

struc task_state
    .eax resd 1
    .ebx resd 1
    .ecx resd 1
    .edx resd 1
    .esi resd 1
    .edi resd 1
    .ebp resd 1
    .esp resd 1
    .eip resd 1
    .eflags resd 1
    .cs resw 1
    .ds resw 1
    .es resw 1
    .fs resw 1
    .gs resw 1
    .ss resw 1
endstruc

global save_task_state

; save_task_state (state_pointer, new_eip_pointer)
save_task_state:
    mov [eax_buffer], eax
    mov [ebx_buffer], ebx
    mov eax, [esp+4]   ; state pointer
    mov ebx, [esp+8]   ; new eip pointer

    push ecx
    mov ecx, [eax_buffer]
    mov [eax + task_state.eax], ecx

    mov ecx, [ebx_buffer]
    mov [eax + task_state.ebx], ecx
    pop ecx

    mov [eax + task_state.ecx], ecx
    mov [eax + task_state.edx], edx
    mov [eax + task_state.esi], esi
    mov [eax + task_state.edi], edi
    mov [eax + task_state.ebp], ebp
    mov [eax + task_state.esp], esp

    mov [eax + task_state.eip], ebx    ; new eip pointer

    pushf
    pop dword [eax + task_state.eflags]

    mov word [eax + task_state.cs], cs
    mov word [eax + task_state.ds], ds
    mov word [eax + task_state.es], es
    mov word [eax + task_state.fs], fs
    mov word [eax + task_state.gs], gs
    mov word [eax + task_state.ss], ss

    ret

global load_task_state

procedure_address:
    dd 0

load_task_state:
    mov eax, [esp+4]    ; state pointer
    mov ebx, [esp+8]    ; return pointer

    mov ecx, [eax + task_state.eax]
    mov edx, [eax + task_state.edx]
    mov esi, [eax + task_state.esi]
    mov edi, [eax + task_state.edi]
    mov ebp, [eax + task_state.ebp]
    mov esp, [eax + task_state.esp]


    push dword [eax + task_state.eflags]
    popf

    push ecx
    mov ecx, [eax + task_state.eip]
    mov [procedure_address], ecx
    pop ecx

    ; push ebx
    mov [esp], ebx


    mov ebx, [eax + task_state.ebx]
    mov eax, [eax + task_state.eax]

    jmp [procedure_address]


global top_of_stack
top_of_stack:
    mov eax, [esp]
    ret
