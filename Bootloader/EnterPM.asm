[bits 16]

switchToPM:
	cli				; turn off interrupts
	lgdt [GDT_descriptor]		; Load GDT
	
	mov eax, cr0
	or eax, 0x1			; sets the first bit of eax to 1, while leaving the rest intact
	mov cr0, eax
	
	jmp CODE_SEG:init_pm		; far jump to code segment, flushes CPU cache, removing real mode instructions

[bits 32]

init_pm:
	mov ax, DATA_SEG
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	mov ebp, 0x90000		; 32 bit stack base pointer
	mov esp, ebp
	call BEGIN_PM


