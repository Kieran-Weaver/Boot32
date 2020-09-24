; Memory Layout:
; 0 - 600h: Reserved for BIOS
; 600h - 800h:   Stage 1 (MBR)
; 800h - 6C00h:  Stage 2
; 6C00h - 7C00h: Stack
; 7C00h - AX:    E820 Entries (24 bytes each)
; 9F00h - 10000h: TSS
; Stack layout:
;   SP
;   Number of E820 Entries
;   DX register (drive number)

; CHS and LBA sizes of the first partition
%define MBR_CHS 0, 0, 0
%define MBR_LBA 0
BITS 16
[ORG 0x0600]
main:
	cli ; disable interrupts
	in al, 0x70
	and al, 0x7F
	out 0x70, al ; disable NMI
	mov bp, 7C00h
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, bp
	call .addr
.addr:
	pop si
	sub si, .addr-main
	mov di, 600h
	mov cx, 100h
	repnz movsw
	jmp 0:.start

.start:
	push dx ; save drive number
	mov [drivenumber], dl
	call do_e820
	push bp ; save number of e820 entries
.doA20:
	call check_a20
	or ax, ax
	jnz .diskread
	mov ax, 2401h
	int 15h
	call check_a20
	or ax, ax
	jnz .diskread
	mov ax, 0xBEEF
.hang:
	jmp .hang
.diskread:
	mov si, diskpacket
	mov ah, 42h
	mov dl, [drivenumber]
	or dl, 80h
	int 13h
	clc
.err:
	int 12h
	jc .err
;	push bx
	push ax
	call setGdt
	mov eax, cr0
	or al, 1
	mov cr0, eax
	xor eax, eax
	xor ebp, ebp
	call flush_gdt
	pop ax
;	pop bx
	pop bp
	pop dx
;	mov ss, dx
	jmp 08h:PModeMain

do_e820:
	mov di, 0x7C00
	xor ebx, ebx		; ebx must be 0 to start
	xor bp, bp		; keep an entry count in bp
	mov edx, 0x0534D4150	; Place "SMAP" into edx
	mov eax, 0xe820
	mov [es:di + 20], dword 1	; force a valid ACPI 3.X entry
	mov ecx, 24		; ask for 24 bytes
	int 0x15
	jc short .failed	; carry set on first call means "unsupported function"
	mov edx, 0x0534D4150	; Some BIOSes apparently trash this register?
	cmp eax, edx		; on success, eax must have been reset to "SMAP"
	jne short .failed
	test ebx, ebx		; ebx = 0 implies list is only 1 entry long (worthless)
	je short .failed
	jmp short .jmpin
.e820lp:
	mov eax, 0xe820		; eax, ecx get trashed on every int 0x15 call
	mov [es:di + 20], dword 1	; force a valid ACPI 3.X entry
	mov ecx, 24		; ask for 24 bytes again
	int 0x15
	jc short .e820f		; carry set means "end of list already reached"
	mov edx, 0x0534D4150	; repair potentially trashed register
.jmpin:
	jcxz .skipent		; skip any 0 length entries
	cmp cl, 20		; got a 24 byte ACPI 3.X response?
	jbe short .notext
	test byte [es:di + 20], 1	; if so: is the "ignore this data" bit clear?
	je short .skipent
.notext:
	mov ecx, [es:di + 8]	; get lower uint32_t of memory region length
	or ecx, [es:di + 12]	; "or" it with upper uint32_t to test for zero
	jz .skipent		; if length uint64_t is 0, skip entry
	inc bp			; got a good entry: ++count, move to next storage spot
	add di, 24
.skipent:
	test ebx, ebx		; if ebx resets to 0, list is complete
	jne short .e820lp
.e820f:
	clc			; there is "jc" on end of list to this point, so the carry must be cleared
	ret
.failed:
	stc			; "function unsupported" error exit
	ret

check_a20:
	push ds
	push es
	push di
	push si
	
	xor ax, ax
	mov es, ax
	not ax
	mov ds, ax
	mov di, 0x0500
	mov si, 0x0510
	
	mov al, byte [es:di]
	push ax
	mov al, byte [ds:si]
	push ax
	
    mov byte [es:di], 0x00
    mov byte [ds:si], 0xFF
    cmp byte [es:di], 0xFF
    
    pop ax
    mov byte [ds:si], al
    
    pop ax
    mov byte [es:di], al
    
    mov ax, 0
    je check_a20__exit
    mov ax, 1
check_a20__exit:
	pop si
	pop di
	pop es
	pop ds
	ret

setGdt:
	xor eax, eax
	mov ax, ds
	shl eax, 4
	add eax, GDT_table
	mov [gdtr + 2], eax
	mov eax, GDT_end
	sub eax, GDT_table
	mov [gdtr], ax
	lgdt [gdtr]
	ret

flush_gdt:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov ss, ax
	ret

drivenumber:
		db 0
diskpacket:
		db 10h
		db 0
blkcnt: dw 32h
db_off:	dw 800h
		dw 0
d_lba:	dd 1
		dd 0

gdtr:		dw 0
			dd 0
align 16
GDT_table:  dq 0
			dq 00CF9A000000FFFFh ; CS for Ring 0
			dq 00CF92000000FFFFh ; DS for Ring 0
			dq 00CFFA000000FFFFh ; CS for Ring 3
			dq 00CFF2000000FFFFh ; DS for Ring 3
			dq 00C089009F000100h ; TSS
GDT_end:	dd 0
align 4
		times 440-($-$$) db 0
		db 'bruh'
mbr:	dw 0
		db 80h       ; Bootable
		db 0, 33h, 0 ; CHS After bootloader
		db 83h       ; Type Linux
		db MBR_CHS   ; Last sector of partition
		dd 32h       ; After bootloader
		dd MBR_LBA   ; LBA size
		times 510-($-$$) db 0
		db 0x55
		db 0xAA

PModeMain:
;print_hex: ; stack
;	mov bp, sp
;	mov dx, [bp + 2]
;	mov cx, 4
;.L1:
;	mov al, dl
;	and al, 0x0F
;	call get_hex
;	shr dx, 4
;	call print_char
;	loop .L1
;	ret
;
;print_char:
;	mov ah, 0x0E
;	mov bh, 0
;	int 0x10
;	ret
;
;get_hex:
;	cmp al, 10
;	jl .addnum
;	add al, 0x7
;.addnum:
;	add al, 0x30
;	ret
