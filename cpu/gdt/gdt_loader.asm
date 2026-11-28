[bits 32]
section .data
align 4
gdtr:   dw 0         ; limit (2)
        dd 0         ; base  (4)

section .text
global gdt_load
gdt_load:
    push    ebp
    mov     ebp, esp
    cli

    ; [ebp+0]  = saved ebp
    ; [ebp+4]  = return address
    ; [ebp+8]  = arg1: limit
    ; [ebp+12] = arg2: base (GDTEntry*)
    ; [ebp+16] = arg3: codeSegment
    ; [ebp+20] = arg4: dataSegment

    ; write GDTR = {limit, base}
    movzx   eax, word [ebp+8]     ; limit
    mov     [gdtr+0], ax
    mov     eax, [ebp+12]         ; base
    mov     [gdtr+2], eax

    lgdt    [gdtr]

    ; Far jump to reload CS with new GDT
    mov     eax, [ebp+16]
    push    eax
    lea     eax, [rel .reload_cs]
    push    eax
    retf

.reload_cs:
    mov     ax, [ebp+20]
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax

    pop     ebp
    ret
