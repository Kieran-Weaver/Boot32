[ORG 0x0800]
BITS 32
PModeMain:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov ebx, 0xb8000
	mov al, '!'
	mov ah, 0x0F
	mov [ebx], ax
.hang:
	jmp .hang

times 0x6400-($-$$) db 0xba
