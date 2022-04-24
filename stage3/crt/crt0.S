.section .text
.global _kstart
_kstart:
	jmp kmain
.hang:
	jmp .hang

.globl loadPageDirectory
loadPageDirectory:
	push %ebp
	mov %esp, %ebp
	mov 8(%esp), %eax
	mov %eax, %cr3
	mov %ebp, %esp
	pop %ebp
	ret
