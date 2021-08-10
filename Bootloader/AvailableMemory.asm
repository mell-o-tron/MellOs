Extended_Size_1: db 0, 0
Extended_Size_2: db 0, 0


getMemoryMap:
    mov bx, Lower_Mem_MSG
    call PrintString
    clc
    int 0x12
    jc ERROR
    mov bx, ax
    call PrintDecimal
    mov bx, KB
    call PrintString
    mov bx, NL
    call PrintString
    
    mov bx, Upper_Mem_MSG
    call PrintString
    mov ax, 0xe801
    int 0x15
    jc ERROR
    ;test ax, ax
    ;je ERROR
    
    mov [Extended_Size_1], ax
    mov [Extended_Size_2], bx
    
     xor dx, dx
     xor bx, bx
     mov ax, [Extended_Size_2]
     mov cx, 16
     div cx
     mov [Extended_Memory_Size], ax

     xor dx, dx
     xor bx, bx
     mov ax, [Extended_Size_1]
     mov cx, 1024
     div cx
     add [Extended_Memory_Size], ax

    mov bx, [Extended_Memory_Size]
    call PrintDecimal
    mov bx, MB
    call PrintString
    mov bx, NL
    call PrintString
    ret
    
Lower_Mem_MSG:
	db 'Lower: ', 0
	
Upper_Mem_MSG:
	db 'Upper: ', 0
	
KB:
	db 'KB', 0
	
MB:
	db 'MB', 0
