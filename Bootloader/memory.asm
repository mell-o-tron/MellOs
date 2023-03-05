; Memory detection using int 0x15 with eax=e820
; if ebx is resetted to 0 the list is complete
; es:di -> destination buffer for 24 byte entries
; bp -> entry count

mmap_entry equ 0x5000       ; address to store the entries

memory_detection:
    mov  di, 0x5004
    xor  ebx, ebx
    xor  bp, bp
    mov  edx, 0x0534D4150
    mov  eax, 0xE820
    mov  [es:di + 20], dword 1
    mov  ecx, 24
    int  0x15
    jc   short .failed
    mov  edx, 0x0534D4150       ; some BIOSes trash this register
    cmp  eax, edx
    jne  short .failed
    test ebx, ebx		
	je   short .failed
	jmp  short .middle
.loop:
    mov  eax, 0xe820		
	mov  [es:di + 20], dword 1	
	mov  ecx, 24		
	int  0x15
	jc   short .end		
	mov  edx, 0x0534D4150
.middle:
    jcxz .skip
	cmp  cl, 20		
	jbe  short .succesfull
	test byte [es:di + 20], 1	
	je   short .skip
.succesfull:
    mov  ecx, [es:di + 8]	
	or   ecx, [es:di + 12]	
	jz   .skip		
	inc  bp         ; good entry, so bp++
	add  di, 24
.skip:
    test ebx, ebx		
	jne  short .loop
.end:
    mov [mmap_entry], bp        ; store the entry count
	clc			
	ret
.failed:
    stc			; "function unsupported" error
	ret
    


    
; detects contiguous memory after 0x100000

detect_cont_memory:     ; TODO error management
    mov ax, 0x8A
    mov dx, 0
    int 0x15  ; result in AX
    mov bx, ax
    ret
    
    
memory_failed: db "Failed to detect memory", 0
