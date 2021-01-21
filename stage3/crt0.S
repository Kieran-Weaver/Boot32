.section .text
.global _kstart
_kstart:
	jmp kmain
.hang:
	jmp .hang
