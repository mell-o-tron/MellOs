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

    ; Save registers eax and ebx
    push ecx
    mov ecx, [eax_buffer]
    mov [eax + task_state.eax], ecx

    mov ecx, [ebx_buffer]
    mov [eax + task_state.ebx], ecx
    pop ecx

    ; Save other registers
    mov [eax + task_state.ecx], ecx
    mov [eax + task_state.edx], edx
    mov [eax + task_state.esi], esi
    mov [eax + task_state.edi], edi
    mov [eax + task_state.ebp], ebp
    mov [eax + task_state.esp], esp

    mov [eax + task_state.eip], ebx    ; new eip pointer

    ; Save flags
    pushf
    pop dword [eax + task_state.eflags]

    ; Save interrupt context
    ; mov 

    ; Save segment registers
    mov word [eax + task_state.cs], cs
    mov word [eax + task_state.ds], ds
    mov word [eax + task_state.es], es
    mov word [eax + task_state.fs], fs
    mov word [eax + task_state.gs], gs
    mov word [eax + task_state.ss], ss

    ; cdecl does not save ebx automatically
    ; so we need to restore it ourselves
    mov ebx, [ebx_buffer]
    ret

global load_task_state

procedure_address:
    dd 0

load_task_state:
    mov eax, [esp+4]    ; state pointer
    ; mov ebx, [esp+8]    ; return pointer

    ; Restore registers
    mov ecx, [eax + task_state.eax]
    mov edx, [eax + task_state.edx]
    mov esi, [eax + task_state.esi]
    mov edi, [eax + task_state.edi]
    mov ebp, [eax + task_state.ebp]

    ; Restore flags
    push dword [eax + task_state.eflags]
    popf

    ; Put eip in a specific memory
    ; location (jump to it later)
    push ecx
    mov ecx, [eax + task_state.eip]
    mov [procedure_address], ecx
    pop ecx

    ; push ebx
    ; mov [esp], ebx

    ; Restore esp, eax and ebx last
    mov esp, [eax + task_state.esp]
    mov ebx, [eax + task_state.ebx]
    mov eax, [eax + task_state.eax]

    ; Jump to the new eip
    jmp [procedure_address]


global top_of_stack
top_of_stack:
    mov eax, [esp]
    ret
