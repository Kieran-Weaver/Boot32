#include "pmm.h"
#include "util.h"
#include "vaddr.h"

extern char   BOOT_BEGIN[];
static uint32_t pcount; // Number of total pages
static uint32_t pfree;  // Number of free pages
static uint32_t pidx;   // Current page index
static uint32_t curr_pages[1024] __attribute__((aligned(4096))) = {};

void pmm_init(SMAP32_t* e820, size_t e820_size) {
	uint32_t addr;
	uint32_t initial_paddr = get_paddr(curr_pages);
	pcount = 0;
	pidx = 0;
	
	for (uint16_t idx = 0; (idx < e820_size) && (e820[idx].type == 1); idx++){
		for (addr = e820[idx].base; addr < (e820[idx].base + e820[idx].length); addr += 0x1000) {
			curr_pages[pcount & 1023] = addr;
			if ((pcount & 1023) == 1023) {
				pt_map(addr, curr_pages, PRESENT | RW);
			}
			pcount++;
		}
	}
	
	pt_map(initial_paddr, curr_pages, PRESENT | RW);
	pcount++; // Add initial page
	pfree = pcount;
}

uint32_t pmm_alloc_ppage(void) {
	uint32_t paddr = 0xFFFFFFFF;

	if (pfree != 0) {
		if ((pidx & 1023) == 1023) {
			paddr = pt_map(curr_pages[1023], curr_pages, PRESENT | RW) & PT_PAGE_MASK;
		} else {
			paddr = curr_pages[pidx & 1023];
		}
		pidx++;
		pfree--;
	}
	
	return paddr;
}

void pmm_free_ppage(uint32_t paddr) {
	pidx--;
	if ((pidx & 1023) == 1023) {
		paddr = pt_map(paddr, curr_pages, PRESENT | RW) & PT_PAGE_MASK;
		curr_pages[1023] = paddr;
	} else {
		curr_pages[pidx & 1023] = paddr;
	}
	pfree++;
}

uint32_t pmm_free_pages(void) {
	return pfree;
}
