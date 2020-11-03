.section .text
.global _kstart
_kstart:
	call kmain
.hang:
	jmp .hang
