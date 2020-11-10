ENTRY(_kstart)
OUTPUT_FORMAT(elf32-i386)
SEARCH_DIR(stage3)
STARTUP(crt0.o)
OUTPUT(kernel.elf)
MEMORY{
	RAM (wx) : org = 0xFFC00000, len = 0x400000
}
SECTIONS{
	.text.start (0xFFC00000):{
		crt0.o(.text)
	} > RAM
	.text : ALIGN(0x1000){
	_TEXT_START_ = .;
		*(.text)
	_TEXT_END_ = .;
	} > RAM
	.data : ALIGN(0x1000){
		*(.data)
	} > RAM
	.bss : ALIGN(0x1000){
		*(.data)
	} > RAM
}
