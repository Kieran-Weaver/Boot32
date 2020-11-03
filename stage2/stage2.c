#include <stdint.h>
#include <stddef.h>
#include "util.h"
#include "e820.h"
#include "crt0.h"
#include "pff3a/source/pff.h"
static uint32_t page_directory[1024] __attribute__((aligned(4096)));
static uint32_t page_table[1024] __attribute__((aligned(4096)));
static volatile uint8_t * screen;
extern const uint64_t GDT[];

uint32_t bmain(uint16_t e820, uint16_t dx){
	setGdt(GDT, 0x40, 0x20, 0x28);
	setDrive(dx);
	screen = (volatile uint8_t*)0xB8000;

	kprint(" Base       | Length     | Type", screen, 32);
	screen += 160;
	uint8_t str[] = " 0x00000000 | 0x0009FC00 | 1";
	return_t entries = validate((SMAP_entry_t*)0x7C00, e820);
	for (uint16_t i = 0; i < entries.n; i++){
		hextostr(entries.data[i].base, str + 3);
		hextostr(entries.data[i].length, str + 16);
		str[27] = '0' + entries.data[i].type;
		kprint(str, screen, 29);
		screen += 160;
	}

	FATFS fs;
	FRESULT res;
	
	res = pf_mount(&fs);
	assert(res == 0);
		
	for (uint16_t i = 0; i < 1024; i++) {
		page_directory[i] = 0x00000002;
		page_table[i] = 0x00000002;
	}
	
	page_directory[0] = 0x0000008B; // Identity mapped first 4 MB, no cache
	page_directory[1023] = 0x00000003 | (uint32_t)(page_table); // Page table, maps to last 4 MiB
	
	uint16_t entries_index = 0;
	while ((entries.data[entries_index].base + entries.data[entries_index].length) <= 0x400000) {
		entries_index++;
	}
	
	uint16_t page_table_index = 0;
	while (entries.data[entries_index].type == 1) {
		uint32_t page_base = (entries.data[entries_index].base + 0x0FFF) & 0xFFFFF000;
		if (page_base < 0x400000) {
			page_base = 0x400000;
		}
		uint32_t length = entries.data[entries_index].length - (page_base - entries.data[entries_index].base);

		for (uint32_t i = 0; (i < length) && (page_table_index < 1024); i += 0x1000) {
			page_table[page_table_index] = (page_base + i) | 0x00000003;
			page_table_index++;
		}
		entries_index++;
	}
	
	screen += 160;
	kprint("Hello Paging World!", screen, 19);
	
	res = pf_open("KERNEL.BIN");
	assert(res == 0);
	
	UINT bytesread = 0;
	int pt_index = 0;
	
	do {
		void* paddr = (void*)(page_table[pt_index] & 0xFFFFF000);
		res = pf_read(paddr, 4096, &bytesread);
		if (res) {
			hextostr(res, str + 3);
			kprint(str, screen, 16);
			screen += 160;
		}
		pt_index++;
	} while (bytesread == 4096);
	
	loadPageDirectory(page_directory);
	enablePaging();

	return 0xFFC00000;
}
