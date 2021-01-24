#include <stdint.h>
#include <stddef.h>
#include "util.h"
#include "e820.h"
#include "crt0.h"
#include "elf.h"
#include "pt.h"
#include "pff3a/source/pff.h"
#define NEW_STACK 0xFFC00000
extern const uint64_t GDT[];

void bmain(uint16_t e820size, uint16_t dx){
	struct Elf_Headers hdrs;
	SMAP32_t* e820entries = (SMAP32_t*)(0x7C00);
	
	kmain entry;
	FATFS fs;
	FRESULT res;

	setGdt(GDT, 0x40, 0x20, 0x28);
	setDrive(dx);

	e820size = validate((SMAP_entry_t*)e820entries, e820size);	
	res = pf_mount(&fs);
	assert(res == 0);
	
	pt_init(e820entries, e820size);
	entry = (kmain)elf_load_file("KERNEL.ELF", &hdrs, sizeof(hdrs));
	assert(entry != NULL);
	pt_map(NEW_STACK - 0x1000);
	enter(entry, NEW_STACK, e820entries, e820size);
}
