print_dec:
	mov dx, 0x00
	mov ax, bx
	mov bx, 10000
	div bx

	mov ah, 0x00
	call print_digit
	mov bx, dx
	
	mov dx, 0x00
	mov ax, bx
	mov bx, 1000
	div bx
	
	mov ah, 0x00
	call print_digit
	mov bx, dx
	
	mov dx, 0x00
	mov ax, bx
	mov bx, 100
	div bx

	mov ah, 0x00
	call print_digit
	mov bx, dx

	mov dx, 0x00
	mov ax, bx
	mov bx, 10
	div bx
	mov ah, 0x00
	call print_digit
	
	mov ax, dx
	mov ah, 0x00
	call print_digit
	ret
	
print_digit:
	mov ah, 0x0E
	add al, 48
	int 0x10
	ret