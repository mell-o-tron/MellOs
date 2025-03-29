vbe_width:						dw 0
vbe_height:						dw 0
vbe_bpp:						dw 0
vbe_pitch:						dw 0
vbe_framebuffer:				dd 0
vbe_mode:						dw 0
vbe_offset:						dw 0
vbe_segment:					dw 0

struc VBE_Info_Struct
	.signature					resb 4
	.version					resw 1
	.oem						resd 1
	.capabilities				resd 1
	.video_modes				resd 1
	.video_memory				resw 1
	.software_rev				resw 1
	.vendor						resd 1
	.product_name				resd 1
	.product_rev				resd 1
	.reserved					resb 222
	.oem_data					resb 256
endstruc

struc VBE_Mode_Info_Struct
	.attributes					resw 1
	.window_a					resb 1
	.window_b					resb 1
	.granularity				resw 1
	.window_size				resw 1
	.segment_a					resw 1
	.segment_b					resw 1
	.win_func_ptr				resd 1
	.pitch						resw 1
	.width						resw 1
	.height						resw 1
	.w_char						resb 1
	.y_char						resb 1
	.planes						resb 1
	.bpp						resb 1
	.banks						resb 1
	.memory_model				resb 1
	.bank_size					resb 1
	.image_pages				resb 1
	.reserved0					resb 1
	.red_mask					resb 1
	.red_position				resb 1
	.green_mask					resb 1
	.green_position				resb 1
	.blue_mask					resb 1
	.blue_position				resb 1
	.reserved_mask				resb 1
	.reserved_position			resb 1
	.direct_color_attributes	resb 1
	.framebuffer				resd 1
	.off_screen_mem_off			resd 1
	.off_screen_mem_size		resw 1
	.reserved1					resb 206
endstruc

struc VBE_Screen
	.mode						resw 1
	.width						resw 1
	.height						resw 1
	.bpp						resb 1
	.pitch						resw 1
	.framebuffer				resd 1
endstruc

; vbe_info:
; 	db "VBE2"
; 	times VBE_Info_Struct_size - 4 db 0x00

; vbe_mode_info:
; 	times VBE_Mode_Info_Struct_size db 0x00

vbe_info			equ 0x5300
vbe_mode_info		equ 0x5300 + VBE_Info_Struct_size
vbe_screen			equ 0x5300 + VBE_Info_Struct_size + VBE_Mode_Info_Struct_size

%macro vbe_get_info 0
	push es
	mov ax, 0x4F00
	mov di, vbe_info
	int 0x10
	pop es

	; VBE returns 0x4F in ax on success
	cmp ax, 0x4F
	jne vbe_error
%endmacro

%macro vbe_get_mode_info 0

	; pusha
	; mov bx, [vbe_mode]
	; call print_dec
	; popa

	push es
	mov ax, 0x4F01
	mov cx, [vbe_mode]
	mov di, vbe_mode_info
	int 0x10
	pop es

	; VBE returns 0x4F in ax on success
	cmp ax, 0x4F
	jne vbe_error
%endmacro

%macro vbe_set_mode 0
	mov ax, 0x4F02
	mov bx, [vbe_mode]
	or bx, 0x4000		; Linear Frame Buffer mode
	mov di, 0
	int 0x10
%endmacro

%macro vbe_find_mode 0
	; Initialise modes address (32 bit)
	mov ax, [vbe_info + VBE_Info_Struct.video_modes]
	mov [vbe_offset], ax
	mov ax, [vbe_info + VBE_Info_Struct.video_modes + 2]
	mov [vbe_segment], ax

	mov fs, ax
	mov si, [vbe_offset]

	.find_mode_loop:
		mov dx, [fs:si]
		add si, 2
		mov [vbe_offset], si
		mov [vbe_mode], dx
		xor ax, ax
		mov fs, ax

		; pusha
		; mov bx, [vbe_mode]
		; call print_dec
		; popa

		cmp word [vbe_mode], 0xFFFF	; End of mode list
		je vbe_error
		; jmp .find_mode_continue
		
		vbe_get_mode_info
		
		; jmp .mode_found

		mov ax, [vbe_width]
		cmp ax, [vbe_mode_info + VBE_Mode_Info_Struct.width]
		jne .find_mode_continue

		mov ax, [vbe_height]
		cmp ax, [vbe_mode_info + VBE_Mode_Info_Struct.height]
		jne .find_mode_continue

		mov al, [vbe_bpp]
		cmp al, [vbe_mode_info + VBE_Mode_Info_Struct.bpp]
		je .mode_found
		.find_mode_continue:
			; call nl
			mov ax, [vbe_segment]
			mov fs, ax
			mov si, [vbe_offset]
			jmp .find_mode_loop
	.mode_found:
			; These are the parameters of the search. There should be no need to write them in the vbe_ locations
			; mov ax, [vbe_mode_info + VBE_Mode_Info_Struct.width]
			; mov word [vbe_width], ax
			; mov ax, [vbe_mode_info + VBE_Mode_Info_Struct.height]
			; mov word [vbe_height], ax
			; mov ax, [vbe_mode_info + VBE_Mode_Info_Struct.bpp]
			; mov word [vbe_bpp], ax
			mov eax, [vbe_mode_info + VBE_Mode_Info_Struct.framebuffer]
			mov [vbe_framebuffer], eax
			mov ax, [vbe_mode_info + VBE_Mode_Info_Struct.pitch]
			mov [vbe_pitch], ax

			mov ax, [vbe_mode]
			mov [vbe_screen + VBE_Screen.mode], ax
			mov ax, [vbe_width]
			mov [vbe_screen + VBE_Screen.width], ax
			mov ax, [vbe_height]
			mov [vbe_screen + VBE_Screen.height], ax
			mov al, [vbe_bpp]
			mov [vbe_screen + VBE_Screen.bpp], al
			mov ax, [vbe_pitch]
			mov [vbe_screen + VBE_Screen.pitch], ax
			mov eax, [vbe_framebuffer]
			mov [vbe_screen + VBE_Screen.framebuffer], eax

			mov eax, [vbe_framebuffer]
			mov [vbe_info + VBE_Info_Struct.signature], eax

			; pusha
			; mov bx, [vbe_screen + VBE_Screen.framebuffer + 2]
			; call print_dec
			; popa
			; jmp $
%endmacro

%macro vbe_test_mode 0
	mov edx, [vbe_framebuffer]
	mov eax, 0x00
	mov ebx, 0x00
	.loop_x:
		.loop_y:
			mov ecx, eax
			shr ecx, 3
			mov byte [edx + 0], cl		; Blue
			mov ecx, ebx
			shr ecx, 3
			mov byte [edx + 1], cl		; Green
			mov byte [edx + 2], 0x00	; Red
			mov byte [edx + 3], 0x00	; Alpha
			add edx, 4
			inc ebx
			cmp bx, [vbe_width]
			jb .loop_y
			mov ecx, [vbe_pitch]
			imul ecx, eax
			mov edx, [vbe_framebuffer]
			add edx, ecx
			mov ebx, 0
		inc eax
		cmp ax, [vbe_height]
		jb .loop_x
%endmacro

vbe_setup:
	pusha
	
	vbe_get_info

	mov bx, vbe_info + VBE_Info_Struct.signature
	call print_string

	mov word [vbe_width], HRES
	mov word [vbe_height], VRES
	mov word [vbe_bpp], BPP

	vbe_find_mode

	; call nl
	; mov bx, [vbe_screen + VBE_Screen.pitch]
	; call print_dec
	; jmp $

	vbe_set_mode

	; vbe_test_mode

	call nl
	popa
	ret
; end vbe_setup


vbe_error:
	mov bx, ax
	call print_dec
	jmp $
	jmp vbe_error



	; ; mov bx, ax
	; ; call print_dec

	; ; mov bx, vbe_info + VBE_Info_Struct.signature
	; ; call print_string

	; ; mov bx, vbe_info + VBE_Info_Struct.version
	; ; call print_dec

	; popa

	; mov eax, [vbe_info + VBE_Info_Struct.video_modes]
	; ; add eax, 2000
	; mov eax, 0x4118
	; mov word [vbe_mode], ax
	; vbe_get_mode_info
	; vbe_set_mode

	; mov eax, 0x00
	; mov ebx, [vbe_mode_info + VBE_Mode_Info_Struct.framebuffer]
	; lop:
	; 	add ebx, eax
	; 	mov byte [ebx], 0xFF
	; 	; mov byte [ebx + 1], 0xFF
	; 	; mov byte [ebx + 2], 0x00
	; 	; mov byte [ebx + 3], 0x00
	; 	add eax, 0x01
	; 	jmp lop

	; jmp $
	; ret

NEW_LINE:
	db ": OK.", 0x0A, 0x0D, 0x00

nl:
	pusha
	mov bx, NEW_LINE
	call print_string
	popa
	; jmp $
	ret