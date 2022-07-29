memory_detection:
    clc
    int 0x12
    jnc after_memory_detection
    mov bx, memory_failed
    call print_string
    jmp $
after_memory_detection:
    mov [0x5000], ax
    ret
upper_memory_detection:
    ;detect upper memory.
    mov ax, 0xE801
    int 0x15
    xor dx, dx
    mov cx, 1024
    div cx
    mov cx, ax
    xor dx, dx
    mov ax, bx
    mov bx, 64
    mul bx
    mov bx, 1024
    div bx
    add cx, ax
    mov ax, cx
    mov [0x5002], ax
    ret

memory_failed: db "Failed to detect memory", 0