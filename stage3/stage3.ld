ENTRY(_kstart)
OUTPUT_FORMAT(elf32-i386)
SEARCH_DIR(stage3)
STARTUP(crt0.o)
OUTPUT(kernel.elf)
MEMORY{
	RAM (wx) : org = 0xFF000000, len = 0x400000
}
SECTIONS{
	BOOT_BEGIN = 0x600;
	
	_KERNEL_START_ = .;
	
	.text.start (0xFF000000):{
		crt0.o(.text)
	} > RAM

	.text : ALIGN(0x1000){
		*(.text)
	} > RAM

	.rodata : ALIGN(0x1000) {
		*(.rodata)
	} > RAM

	.data : ALIGN(0x1000){
		*(.data)
	} > RAM

	.bss : ALIGN(0x1000){
		*(.bss)
	} > RAM

	_KERNEL_END_ = .;
}
