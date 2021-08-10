PrintString:
	mov ah, 0x0e ;tty mode


	prloop:
		mov al, [bx]
		cmp al, 0
		je prend
		
		int 0x10
		add bx, 1
		jmp prloop
		
		

	prend:
		ret
