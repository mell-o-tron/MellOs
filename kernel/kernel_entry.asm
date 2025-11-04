bits 32
%macro OUT_SERIAL 1
    mov al, %1
    
                                ; wait for transmit buffer to be empty
    mov dx, 0x3f8 + 5           ; line Status Register (LSR)
    %%wait_transmit:
        in al, dx
        test al, 0x20           ; empty bit
        jz %%wait_transmit
    
                                ; safe to send
    mov dx, 0x3f8
    mov al, %1
    out dx, al
%endmacro
%include "kconfig.asm"
CODE_SEG equ GDT_code - GDT_start
DATA_SEG equ GDT_data - GDT_start
TMP_MEM  equ 0x60000            ; Tmp location to store the multiboot tags

; kernel_entry.asm
global _kernel_start
extern main
extern kpanic

section .entry
_kernel_start:
    cli
    cld

    ; Preserve Multiboot EBX early
    mov [TMP_MEM], ebx
    jmp init_serial
.pre_mb_test:
    ; EAX contains Multiboot magic for v1 (0x2BADB002). Emit a quick serial breadcrumb.
    push eax
    mov eax, 0x2BADB002
    cmp eax, [esp]
    pop eax
    jne .mb_bad
    OUT_SERIAL 'M' ; Magic OK
    jmp .after_magic
.mb_bad:
    OUT_SERIAL 'm' ; Magic BAD
.after_magic:

    lgdt [GDT_descriptor]

    mov ax, DATA_SEG
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov ebp, 0x90000		; 32 bit stack base pointer
	mov esp, ebp

    xor ebp, ebp

    ; Pass saved EBX (Multiboot info) as cdecl arg to main
    push dword [TMP_MEM]
    call check_and_enable_features
    call main
    jmp $

check_and_enable_features:
    ; Test for CPUID capability (ID bit in EFLAGS)
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21 ; Flip the ID bit
    push eax
    popfd
    pushfd
    pop eax
    xor eax, ecx
    jz .fault ; If ID bit couldn't be flipped, CPUID not supported.

    OUT_SERIAL 'C' ; CPUID detected

    ; Get basic CPU features (CPUID Leaf 1)
    mov eax, 1
    cpuid
%ifdef CONFIG_CPU_FEAT_SSE
    ; Check for SSE support (EDX bit 25)
    test edx, 1 << 25   ; SSE
    jz .fault

    mov eax, cr0
    and eax, ~(1 << 2)  ; Clear EM (Emulation)
    or eax, 1 << 1      ; Set MP (Monitor Coprocessor)
    mov cr0, eax

    mov eax, cr4
    or eax, 1 << 9      ; Set OSFXSR (Operating System FXSAVE/FXRSTOR Support)
    or eax, 1 << 10     ; Set OSXMMEXCPT (Operating System Unmasked SIMD Floating-Point Exception Support)
    mov cr4, eax
    OUT_SERIAL 'S' ; SSE Enabled
%endif
.fault:
    ret

GDT_start:
    GDT_null:
        dd 0x0
        dd 0x0

    GDT_code:
        dw 0xffff
        dw 0x0
        db 0x0
        db 0b10011010
        db 0b11001111
        db 0x0

    GDT_data:
        dw 0xffff
        dw 0x0
        db 0x0
        db 0b10010010
        db 0b11001111
        db 0x0

GDT_end:

GDT_descriptor:
    dw GDT_end - GDT_start - 1
    dd GDT_start

init_serial:
    mov dx, 0x3f8
    mov al, 0x00
    out dx, al         ; disable interrupts
    inc dx
    mov al, 0x80
    out dx, al         ; enable DLAB
    dec dx
    mov al, 0x03
    out dx, al         ; divisor low
    inc dx
    mov al, 0x00
    out dx, al         ; divisor high
    dec dx
    mov al, 0x03
    out dx, al         ; 8 bits, no parity, one stop
    inc dx
    mov al, 0x03
    out dx, al         ; enable FIFO, clear them
    dec dx
    mov al, 0x0b
    out dx, al         ; IRQs enabled, RTS/DSR set
    jmp _kernel_start.pre_mb_test

section .low.text
    
%include "../cpu/interrupts/interrupt.asm"

section .text
%include "../memory/paging/paging.asm"

section .data

section .rodata		    ; read only data

%include "IncBins.asm"	; including binaries
