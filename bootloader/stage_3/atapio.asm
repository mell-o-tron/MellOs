; NASM source: LBA28_read_sector and helper functions
; Defines for ATA status bits
%define STATUS_BSY 0x80
%define STATUS_RDY 0x40
%define STATUS_DRQ 0x08
%define STATUS_DF  0x20
%define STATUS_ERR 0x01

section .text
    global wait_BSY
    global wait_DRQ
    global check_ERR
    global LBA28_read_sector

;----------------------------------------------------------------
; void wait_BSY(void)
; Loops while port 0x1F7 returns a value with STATUS_BSY set.
;----------------------------------------------------------------
wait_BSY:
.wait_loop:
    mov dx, 0x1F7      ; ATA status register
    in  al, dx
    test al, STATUS_BSY
    jnz .wait_loop     ; if BSY is set, continue looping
    ret

;----------------------------------------------------------------
; void wait_DRQ(void)
; Loops until the status register indicates the drive is ready.
;----------------------------------------------------------------
wait_DRQ:
.drq_loop:
    mov dx, 0x1F7      ; ATA status register
    in  al, dx
    test al, STATUS_RDY
    jz  .drq_loop      ; if RDY bit is not set, loop
    ret

;----------------------------------------------------------------
; bool check_ERR(void)
; Returns true (1) if the STATUS_ERR bit is set.
;----------------------------------------------------------------
check_ERR:
    mov dx, 0x1F7      ; ATA status register
    in  al, dx
    and al, STATUS_ERR
    ; Set AL = 1 if error bit is nonzero; else AL = 0.
    cmp al, 0
    setnz al         ; set AL = 1 if not zero, 0 if zero
    ret

;----------------------------------------------------------------
; void LBA28_read_sector(uint8_t drive, uint32_t LBA, uint32_t sector, uint16_t *addr)
;
; This function reads a number of sectors from an ATA drive using LBA28 addressing.
; The parameters (on the stack, cdecl) are:
;   [ebp+8]  : drive (uint8_t)
;   [ebp+12] : LBA (uint32_t)
;   [ebp+16] : sector count (uint32_t)
;   [ebp+20] : pointer to destination memory (uint16_t*)
;
; The routine:
;   - Masks LBA to 28 bits.
;   - Waits for BSY to clear.
;   - Writes the drive/LBA register, sector count, LBA bytes, and issues the read command.
;   - For each sector, waits for BSY then DRQ, then reads 256 words (512 bytes) from port 0x1F0.
;----------------------------------------------------------------
LBA28_read_sector:
    push ebp
    mov  ebp, esp

    ; --- Parameters ---
    ; drive: byte at [ebp+8]
    ; LBA:   dword at [ebp+12]
    ; count: dword at [ebp+16] (number of sectors)
    ; addr:  pointer at [ebp+20]

    ; Load drive (into AL) and LBA (into ECX) and mask LBA to 28 bits.
    movzx eax, byte [ebp+8]   ; drive in AL (zero-extended)
    mov  ecx, [ebp+12]        ; load LBA into ECX
    and  ecx, 0x0FFFFFFF      ; mask LBA to 28 bits

    ; Wait for BSY to clear.
    call wait_BSY

    ; --- Set up drive/head register (port 0x1F6) ---
    ; Compute: drive | ((LBA >> 24) & 0xF)
    mov edx, ecx             ; copy LBA into EDX
    shr edx, 24              ; EDX = LBA >> 24
    and edx, 0x0F            ; keep only lower 4 bits
    or  al, dl               ; combine drive value with bits from LBA
    mov dx, 0x1F6
    out dx, al               ; send to port 0x1F6

    ; --- Write remaining ATA registers ---
    ; Clear the features register (port 0x1F1)
    mov al, 0x00
    mov dx, 0x1F1
    out dx, al

    ; Write sector count (port 0x1F2)
    mov eax, [ebp+16]        ; sector count in EAX (only lower 8 bits used)
    mov al, al               ; use lower 8 bits
    mov dx, 0x1F2
    out dx, al

    ; Write LBA low byte (port 0x1F3)
    mov al, cl             ; lower 8 bits of ECX (LBA)
    mov dx, 0x1F3
    out dx, al

    ; Write LBA mid byte (port 0x1F4) and high byte (port 0x1F5)
    ; Reload LBA into EBX for shifting.
    mov ebx, [ebp+12]
    and ebx, 0x0FFFFFFF     ; mask LBA to 28 bits (again)
    shr ebx, 8              ; EBX now contains LBA >> 8
    mov al, bl              ; low byte of (LBA >> 8)
    mov dx, 0x1F4
    out dx, al

    shr ebx, 8              ; now EBX contains LBA >> 16 in BL
    mov al, bl
    mov dx, 0x1F5
    out dx, al

    ; Issue the 'Read Sectors' command (0x20) to port 0x1F7.
    mov al, 0x20
    mov dx, 0x1F7
    out dx, al

    ; --- Begin reading sectors ---
    ; ECX will be reused to count sectors. Reload sector count:
    mov ecx, [ebp+16]       ; number of sectors to read
    ; Destination pointer is in [ebp+20]
    mov esi, [ebp+20]       ; ESI points to destination memory

.read_sector_loop:
    test ecx, ecx
    jz .done_sectors        ; if count == 0, done

    ; For each sector, wait for BSY and then DRQ.
    call wait_BSY
    call wait_DRQ

    ; Read 256 words (512 bytes) from the data port 0x1F0.
    mov edx, 256            ; loop counter for 256 words
.read_word_loop:
    cmp edx, 0
    je .after_read_word
    mov dx, 0x1F0
    in ax, dx               ; read a word from the data port
    mov [esi], ax
    add esi, 2              ; advance destination pointer by 2 bytes
    dec edx
    jmp .read_word_loop
.after_read_word:
    dec ecx               ; one sector read
    jmp .read_sector_loop

.done_sectors:
    pop ebp
    ret
