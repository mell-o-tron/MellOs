enter_protected_mode:
	cli
	lgdt[gdt_descriptor]
	mov eax, cr0
	or al, 0x01
	mov cr0, eax
	jmp CODE_SEG:_main32