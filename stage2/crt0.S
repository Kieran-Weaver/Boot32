.section .text
.global _bstart
_bstart:
	shl $10, %eax
	mov %eax, %esp
	pushl $0
	pushl $0
	pushl %edx
	pushl %ebp
	call bmain
.hang:
	jmp .hang
