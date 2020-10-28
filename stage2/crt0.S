.section .text
.global _bstart
_bstart:
	shl $10, %eax
	mov %eax, %esp
	pushl %edx
	pushl %ebp
	call bmain
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

.globl enablePaging
enablePaging:
	push %ebp
	mov %esp, %ebp
	mov %cr0, %eax
	or $0x80000000, %eax
	mov %eax, %cr0
	mov %ebp, %esp
	pop %ebp
	ret

	ret

.globl setGdt
setGdt:
	mov   4(%esp), %eax
	mov   gdtr, %edx
	mov   %eax, 2(%edx)
	mov   8(%esp), %ax
	mov   %ax, (%edx)
	lgdt  (%edx)
	
	mov   16(%esp), %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %gs
	mov %ax, %fs
	mov %ax, %ss
	
	pushl 12(%esp)
	push $.setcs
	ljmp *(%esp)
.setcs:
	add $8, %esp
	ret

gdtr:		.quad 0
