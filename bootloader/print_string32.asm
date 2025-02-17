VIDEO_MEMORY equ 0xB8000
GREY_ON_BLACK equ 0x07

print_string32:
	pusha
	mov edx, VIDEO_MEMORY
	.loop:
		mov al, [ebx]
		mov ah, GREY_ON_BLACK

		cmp al, 0x00
		je .end

		mov [edx], ax
		inc ebx
		add edx,2
		
		jmp .loop
	.end:
		ret