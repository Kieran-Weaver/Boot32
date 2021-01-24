.section .text
.global _bstart
_bstart:
	shl $10, %eax
	mov %esp, (backupESP)
	mov %eax, %esp
	mov %cr0, %eax
	and $0xF9, %al
	mov %eax, %cr0
	fninit
	fnstsw (FPUTest)
	cmpw $0, (FPUTest)
	jne bhang
	pushl %edx
	pushl %ebp
	call bmain
	jmp bhang

.globl bhang
bhang:
	jmp bhang

.globl enter
enter:
	movl 4(%esp),  %eax // entry
	movl 8(%esp),  %ebx // stack
	movl 12(%esp), %ecx // e820
	movl 16(%esp), %edx // e820_size
	subl $8, %ebx
	movl %ebx, %esp

	pushl %edx
	pushl %ecx
	call *%eax
	jmp bhang

.globl setGdt
setGdt:
	mov   4(%esp), %eax
	mov   $gdtr, %edx
	mov   %eax, 2(%edx)
	mov   8(%esp), %ax
	mov   %ax, (%edx)
	lgdt  (%edx)
	pushl 16(%esp)
	pushl 16(%esp)
	call setSegments
	add $8, %esp
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

.globl readSector
readSector:
	mov 4(%esp), %eax             // start sector
	mov %eax, (diskpacket + 8)
	mov 8(%esp), %edx             // drive number
	
	push %esi
	push %edi

	mov (backupESP), %eax         // Switch to 16-bit stack
	mov %esp, (backupESP)
	mov %eax, %esp

	mov $0x10, %eax                // Switch to 16-bit segments
	push %eax
	mov $0x18, %eax
	push %eax
	
	call setSegments
	
.code16
	mov %cr0, %eax
	mov %eax, (backupCR0)
	and $0x7FFFFFFE, %eax         // Disable protected mode
	mov %eax, %cr0
	
	jmp $0,$realmode              // Jump to real mode

realmode:
	mov $0, %ax                   // Set segment registers to 0
	call rsetsegs
	jmp .err

rsetsegs:
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	ret

.err:                             // Read disk sector
	mov $diskpacket, %si
	mov $0x42, %ah
	int $0x13
	jc .err
	
	mov %cr0, %eax                // Enable protected mode
	or $1, %al
	mov %eax, %cr0
	
	mov $0x28, %ax                // Switch to 32-bit segments
	call rsetsegs
	jmp $0x20,$pmode
	
.code32
pmode:
	mov (backupCR0), %eax
	mov %eax, %cr0
	mov %esp, %eax
	mov (backupESP), %esp         // Load 32-bit stack
	mov %eax, (backupESP)
	
	mov $diskdata, %esi
	mov 20(%esp), %edi            // Buffer
	mov 24(%esp), %ecx            // Bufsize (in bytes)
	
	cmp $512, %ecx
	jb .repmov
	mov $512, %ecx

.repmov:
	rep movsb
	pop %edi
	pop %esi

	ret

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

	mov %cr4, %eax
	or $0x00000010, %eax
	mov %eax, %cr4

	mov %cr0, %eax
	or $0x80000000, %eax
	mov %eax, %cr0
	
	mov %ebp, %esp
	pop %ebp
	ret

backupCR0:
	.int 0

backupESP:
	.int 0

gdtr:		.quad 0

diskpacket:
	.byte 16 // Size of packet
	.byte 0  // Always 0
	.word 1  // Sectors
	.word diskdata  // Buffer offset
	.word 0  // Buffer segment, always 0 since code is in segment 0000
	.int  0  // Lower 32-bits LBA
	.int  0  // Upper 32-bits LBA

FPUTest:
	.word 0x55AA

.align 16
diskdata:
	.dcb.b 0x800

.align 16
.globl GDT
GDT:
	.quad 0x0
	.quad 0x008F92000000FFFF  // flat ds, offset 0x08
	.quad 0x000092000000FFFF  // 16 bit legacy real mode ds, offset 0x10
	.quad 0x000098000000FFFF  // 16 bit legacy real mode cs, offset 0x18
	.quad 0x00CF9A000000FFFF  // CS for Ring 0, offset 0x20
	.quad 0x00CF92000000FFFF  // DS for Ring 0, offset 0x28
	.quad 0x00C089009F000100  // TSS, offset 0x30
	.quad 0					  // Blank, offset 0x38
