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

.globl loadGDT
loadGDT:
	mov   4(%esp), %edx
	lgdt  (%edx)
	pushl 12(%esp)
	pushl 12(%esp)
	call setSegments
	add $8, %esp
	mov $0x28, %ax
	ltr %ax
	ret

.globl setSegments
setSegments:
	mov   8(%esp), %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %gs
	mov %ax, %fs
	mov %ax, %ss
	pushl 4(%esp)
	push $.setcs
	ljmp *(%esp)
.setcs:
	add $8, %esp
	ret

.align 16
.globl GDT
GDT:
	.quad 0x0
	.quad 0x00CF9A000000FFFF  // CS for Ring 0, offset 0x08
	.quad 0x00CF92000000FFFF  // DS for Ring 0, offset 0x10
	.quad 0x00CFFA000000FFFF  // CS for Ring 3, offset 0x18
	.quad 0x00CFF2000000FFFF  // DS for Ring 3, offset 0x20
.globl GDT_TSSEntry
GDT_TSSEntry:
	.quad 0x0  // TSS, offset 0x28

.align 16
.globl IDT
IDT:
	.quad 

.align 4
gdtr:
	.word 0 // for limit storage
	.word 0
	.word 0 // for base storage

.align 4
idtr:
	.word 0 // for limit storage
	.word 0
	.word 0 // for base storage
