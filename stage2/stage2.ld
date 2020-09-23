ENTRY(_bstart)
OUTPUT_FORMAT(binary)
SEARCH_DIR(stage2)
STARTUP(crt0.o)
OUTPUT(stage2.bin)
MEMORY{
	RAM (wx) : org = 0x800, len = 0x6400
}
SECTIONS{
	.text.start (0x800):{
		crt0.o(.text)
	} > RAM
	.text : ALIGN(0x100){
	_TEXT_START_ = .;
		*(.text)
	_TEXT_END_ = .;
	} > RAM
	.data : ALIGN(0x100){
		*(.data)
	} > RAM
	.bss : ALIGN(0x100){
		*(.data)
	} > RAM
}
