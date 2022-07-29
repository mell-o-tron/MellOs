detect_memory:
	;detect low memory
	mov bx, LOWER
	call print_string
	clc
	int 0x12
	jc detect_memory_error
	mov bx, ax
	call print_dec
	mov bx, KB
	call print_string

	;detect upper memory
	mov bx, UPPER
	call print_string
	mov ax, 0xE801
	int 0x15
	jc detect_memory_error
	
	mov [EXTENDED_SIZE_1], ax
	mov [EXTENDED_SIZE_2], bx
	
	xor dx, dx
	xor bx, bx
	mov ax, [EXTENDED_SIZE_2]
	mov cx, 16
	div cx
	mov [EXTENDED_MEMORY_SIZE], ax
	
	xor dx, dx
	xor bx, bx
	mov ax, [EXTENDED_SIZE_1]
	mov cx, 1024
	div cx
	add [EXTENDED_MEMORY_SIZE], ax
	
	mov bx, [EXTENDED_MEMORY_SIZE]
	call print_dec
	mov bx, MB
	call print_string
	ret

detect_memory_error:
	mov bx, MEMORY_ERROR
	call print_string
	ret

LOWER:
	db "Lower memory: ", 0x00

UPPER:
	db "Upper memory: ", 0x00

KB:
	db "KB", 0x0A, 0x0D, 0x00

MB:
	db "MB", 0x0A, 0x0D, 0x00

MEMORY_ERROR:
	db "Error detecting memory.", 0x0A, 0x0D, 0x00

EXTENDED_SIZE_1:
	dw 0x00

EXTENDED_SIZE_2:
	dw 0x00

EXTENDED_MEMORY_SIZE:
	dw 0x00
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