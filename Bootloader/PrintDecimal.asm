; I could make a loop and set the number of digits I want to print, but I'm not going to because I'm a bit lazy. 
; Do you wanna do it? Yeah? You think you can make this abomination of a bootloader a bit better? Heh, you're welcome to do it if you like.


PrintDecimal:
							;SET BX TO THE NUMBER YOU WANT TO PRINT

    mov dx, 0
	mov ax, bx
	mov bx, 10000
	div bx
	
	mov ah, 0
	call printDigit 
	mov bx, dx
	
    mov dx, 0
	mov ax, bx
	mov bx, 1000
	div bx
	
	mov ah, 0
	call printDigit 
	mov bx, dx
	
	
	mov dx, 0
	mov ax, bx
	mov bx, 100
	div bx
	
	mov ah, 0
	call printDigit 
	mov bx, dx
	
	mov dx, 0
	mov ax, bx
	mov bx, 10
	div bx
	mov ah, 0
	call printDigit 	
	
	mov ax, dx
	mov ah, 0
	call printDigit  
	

	ret

printDigit:
	mov ah, 0x0e 	; tty mode
	add al, 48 	; converts to ASCII
	int 0x10 	; prints the remainder
	ret


