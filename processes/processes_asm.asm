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

global save_task_state

; save_task_state (state_pointer, new_eip_pointer)
save_task_state:
    mov [eax_buffer], eax
    mov [ebx_buffer], ebx
    mov eax, [esp+4]   ; state pointer
    mov ebx, [esp+8]   ; new eip pointer

    push ecx
    mov ecx, [eax_buffer]
    mov [eax], ecx

    mov ecx, [ebx_buffer]
    mov [eax + 4], ecx
    pop ecx

    mov [eax + 8], ecx
    mov [eax + 12], edx
    mov [eax + 16], esi
    mov [eax + 20], edi
    mov [eax + 24], ebp
    mov [eax + 28], esp

    mov [eax + 32], ebx    ; new eip pointer

    pushf
    pop dword [eax + 36]

    mov word [eax + 40], cs
    mov word [eax + 42], ds
    mov word [eax + 44], es
    mov word [eax + 46], fs
    mov word [eax + 48], gs
    mov word [eax + 50], ss

    ret

global load_task_state

procedure_address:
    dd 0

load_task_state:
    mov eax, [esp+4]    ; state pointer
    mov ebx, [esp+8]    ; return pointer

    mov ecx, [eax + 8]
    mov edx, [eax + 12]
    mov esi, [eax + 16]
    mov edi, [eax + 20]
    mov ebp, [eax + 24]
    mov esp, [eax + 28]


    push dword [eax + 36]
    popf

    push ecx
    mov ecx, [eax + 32]
    mov [procedure_address], ecx
    pop ecx

    ; push ebx
    mov [esp], ebx


    mov ebx, [eax + 4]
    mov eax, [eax]

    jmp [procedure_address]


global top_of_stack
top_of_stack:
    mov eax, [esp]
    ret
