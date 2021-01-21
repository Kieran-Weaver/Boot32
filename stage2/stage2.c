#include <stdint.h>
#include <stddef.h>
#include "util.h"
#include "e820.h"
#include "crt0.h"
#include "elf.h"
#include "pff3a/source/pff.h"
static uint32_t page_directory[1024] __attribute__((aligned(4096)));
static uint32_t page_table[1024] __attribute__((aligned(4096)));
extern const uint64_t GDT[];
typedef void (*kmain)(SMAP32_t*, size_t);

void bmain(uint16_t e820size, uint16_t dx){
	struct Elf_Headers hdrs;
	uint8_t str[] = " 0x00000000 | 0x0009FC00 | 1";
	volatile uint8_t * screen = (volatile uint8_t*)0xB8000;
	SMAP32_t* e820entries = (SMAP32_t*)(0x7C00);
	
	kmain entry;
	FATFS fs;
	FRESULT res;

	setGdt(GDT, 0x40, 0x20, 0x28);
	setDrive(dx);

	kprint(" Base       | Length     | Type", screen, 32);
	screen += 160;
	e820size = validate((SMAP_entry_t*)e820entries, e820size);
	for (uint16_t i = 0; i < e820size; i++){
		hextostr(e820entries[i].base, str + 3);
		hextostr(e820entries[i].length, str + 16);
		str[27] = '0' + e820entries[i].type;
		kprint(str, screen, 29);
		screen += 160;
	}
	
	res = pf_mount(&fs);
	assert(res == 0);
		
	for (uint16_t i = 0; i < 1024; i++) {
		page_directory[i] = 0x00000002;
		page_table[i] = 0x00000002;
	}
	
	page_directory[0] = 0x0000008B; // Identity mapped first 4 MB, no cache
	page_directory[1023] = 0x00000003 | (uint32_t)(page_table); // Page table, maps to last 4 MiB
	
	uint16_t entries_index = 0;
	while ((e820entries[entries_index].base + e820entries[entries_index].length) <= 0x400000) {
		entries_index++;
	}
	
	uint16_t page_table_index = 0;
	while (e820entries[entries_index].type == 1) {
		uint32_t page_base = (e820entries[entries_index].base + 0x0FFF) & 0xFFFFF000;
		if (page_base < 0x400000) {
			page_base = 0x400000;
		}
		uint32_t length = e820entries[entries_index].length - (page_base - e820entries[entries_index].base);

		for (uint32_t i = 0; (i < length) && (page_table_index < 1024); i += 0x1000) {
			page_table[page_table_index] = (page_base + i) | 0x00000003;
			page_table_index++;
		}
		entries_index++;
	}
	
	loadPageDirectory(page_directory);
	enablePaging();
	
	entry = (kmain)elf_load_file("KERNEL.ELF", &hdrs, sizeof(hdrs));
	assert(entry != NULL);
	entry(e820entries, e820size);
}
