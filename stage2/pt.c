#include "pt.h"
#include "crt0.h"

static uint32_t page_directory[1024] __attribute__((aligned(4096)));
static uint32_t* page_dir = (uint32_t*)0xFFFFF000;
static SMAP32_t* e820;
static size_t    e820_size;
static uint16_t entries_index = 0;
static uint16_t page_table_index = 0;
static uint32_t entry_addr = 0;

static inline void invlpg(void* m) {
    asm volatile ( "invlpg (%0)" : : "b"(m) : "memory" );
}

static void* palloc(void) {
	void* paddr = (void*)(entry_addr - 0x1000);
	
	entry_addr -= 0x1000;
	if (e820[entries_index].length >= 0x1000) {
		e820[entries_index].length -= 0x1000;
	} else {
		e820[entries_index].length = 0;
	}
	
	if (entry_addr <= e820[entries_index].base) {
		entries_index--;
		entry_addr = (e820[entries_index].base + e820[entries_index].length + 0x0FFF) & 0xFFFFF000;
	}
	
	return paddr;
}

void pt_init(SMAP32_t* _e820, size_t _e820_size) {
	e820 = _e820;
	e820_size = _e820_size;
	
	// Initialize Page Table and Page Directory
	for (uint16_t i = 0; i < 1024; i++) {
		page_directory[i] = 0x00000002;
	}

	page_directory[0] = 0x0000008B; // Identity mapped first 4 MB, no cache
	page_directory[1023] = 0x00000003 | (uint32_t)(page_directory); // Page Directory
	
	while (e820[entries_index + 1].type == 1) {
		entries_index++;
	}
	
	entry_addr = (e820[entries_index].base + e820[entries_index].length + 0x0FFF) & 0xFFFFF000;

	loadPageDirectory(page_directory);
	enablePaging();
}

void pt_map(uint32_t vaddr) {
	uint32_t  pdi = (vaddr >> 22);
	uint32_t  pti = (vaddr >> 12) & 0x3FF;
	uint32_t* pagetable = ((uint32_t*)0xFFC00000) + (0x400 * pdi);
	uint32_t* temp = 0;

	vaddr &= 0xFFFFF000;

	if ((page_dir[pdi] & 1U) == 0) { // Not present, allocate PT
		temp = palloc();
		page_dir[pdi] = (uint32_t)temp | 0x03;
		invlpg(pagetable);
		for (uint16_t i = 0; i < 1024; i++) {
			pagetable[i] = 0x00000002;
		}
	}
	
	pagetable[pti] = ((uint32_t)palloc()) | 0x03;
	invlpg((void*)vaddr);
}
