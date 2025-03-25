[bits 32]

section .entry
[extern main]

[global start_stage_3]
start_stage_3:
    ; jmp $
    push es
    mov ax, 0x0
    mov es, ax
    call main		        ; calls stage 3 loader function main()
    pop es

    mov eax, 0x9000
    jmp eax
    jmp $

section .text

section .data

section .rodata		    ; read only data
