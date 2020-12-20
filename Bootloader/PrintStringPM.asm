[bits 32]
VIDEO_MEMORY equ 0xb8000 			; beginning of video memory: to print in protected mode you have to write on 
						; video memory.
WHITE_ON_BLACK equ 0x0f			; WB parameter

PrintStringPM:
	pusha
	mov edx, VIDEO_MEMORY			; sets EDX to beginning of video memory
	
	PrPM_Loop:
		mov al, [ebx]			; store the char at EBX in AL
		mov ah, WHITE_ON_BLACK		; store attributes in AH
		
		cmp al, 0			; looks for string termination (NULL char)
		je PrPM_End
		
		mov [edx], ax			; moves (char, attributes) to video memory
		inc ebx			; next char
		add edx, 2			; next (char, attributes)
		
		jmp PrPM_Loop
		
	PrPM_End:
		popa
		ret 
