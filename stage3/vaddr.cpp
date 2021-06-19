#include "vaddr.h"
#include "pagetable.h"
#include "intrinsics.h"

const static uint32_t* pd  = reinterpret_cast<uint32_t*>(0xFFFFF000);

struct ptinfo {
	uint16_t free;
	uint16_t largest;
	uint16_t lstart;
	uint16_t start;
	uint16_t end;
};

static ptinfo ptmeta[1024];

static void set_ptinfo(uint16_t i) {
	const uint32_t* pt = reinterpret_cast<const uint32_t*>(0xFFC00000) + (0x400 * i);

	if (i == 1023) {
		ptmeta[i].free    = 0;
		ptmeta[i].largest = 0;
		ptmeta[i].start   = 0;
		ptmeta[i].end     = 0;
	} else if ((pd[i] & PRESENT) != 0) {
		ptmeta[i].free = 0;
		ptmeta[i].largest = 0;
		ptmeta[i].start = 0;
		ptmeta[i].end = 0;
		uint16_t counter = 0;
		uint16_t curr = 0;
		
		for (counter = 0; pt[counter] & PRESENT == 0; counter++) {
			ptmeta[i].largest++;
			ptmeta[i].start++;
		}
		
		for (; counter < 1024; counter++) {
			if (pt[counter] & PRESENT == 0) {
				curr++;
			} else {
				if (curr > ptmeta[i].largest) {
					ptmeta[i].largest = curr;
				}
				curr = 0;
			}
		}
		
		ptmeta[i].end = curr;
	} else {
		ptmeta[i].free    = 1024;
		ptmeta[i].largest = 1024;
		ptmeta[i].start   = 1024;
		ptmeta[i].end     = 1024;
	}
}

paddr_t get_paddr(vaddr_t vaddr) {
	uint32_t  pdi = (reinterpret_cast<uint32_t>(vaddr) >> 22);
	uint32_t  pti = (reinterpret_cast<uint32_t>(vaddr) >> 12) & 0x3FF;
	uint32_t* pt = reinterpret_cast<uint32_t*>(0xFFC00000) + (0x400 * pdi);

	if (((pd[pdi] & PRESENT) == 0) || ((pt[pti] & PRESENT) == 0)) {
		return 0;
	}
 
	return (pt[pti] & ~0xFFF) + (reinterpret_cast<uint32_t>(vaddr) & 0xFFF);
}

paddr_t pt_map(paddr_t paddr, vaddr_t vaddr, uint32_t flags) {
	uint32_t  pdi = (reinterpret_cast<uint32_t>(vaddr) >> 22);
	uint32_t  pti = (reinterpret_cast<uint32_t>(vaddr) >> 12) & 0x3FF;	
	uint32_t* pagetable = ((uint32_t*)0xFFC00000) + (0x400 * pdi);
	paddr_t tmp;

	vaddr = reinterpret_cast<vaddr_t>(reinterpret_cast<uint32_t>(vaddr) & 0xFFFFF000);
	
	if ((pd[pdi] & PRESENT) == 0) { // Not present
		return 0xFFFFFFFF;
	}

	tmp = pagetable[pti];
	pagetable[pti] = paddr | (flags & 0xFFF) | 0x01;
	invlpg(reinterpret_cast<void*>(vaddr));
	
	return tmp;
}

uint32_t pt_unmap(vaddr_t vaddr) {
	uint32_t  pdi = (reinterpret_cast<uint32_t>(vaddr) >> 22);
	uint32_t  pti = (reinterpret_cast<uint32_t>(vaddr) >> 12) & 0x3FF;
	uint32_t* pagetable = ((uint32_t*)0xFFC00000) + (0x400 * pdi);
	uint32_t  paddr = 0;
	
	if ((pd[pdi] & PRESENT) != 0) { // Present
		paddr = pagetable[pti];
		pagetable[pti] = pagetable[pti] & ~(PRESENT);
		invlpg(reinterpret_cast<void*>(vaddr));
	}
	
	return paddr;
}

void vinit() {
	uint16_t i = 0;
	for (i = 0; i < 1024; i++) {
		set_ptinfo(i);
	}
}

vaddr_t valloc(uint32_t pages) {
	uint16_t pdi = 0;
	uint16_t pti = 0;
	uint32_t vaddr = 0;
	uint16_t i = 0;
	bool found = false;

	// Check every page except zero page
	for (i = 1; i < 1024; i++) {
		if (pages <= (ptmeta[i-1].end + ptmeta[i].start)) {
			pdi = i - 1;
			pti = 1024 - ptmeta[i-1].end;
			found = true;
			break;
		} else if (pages <= ptmeta[i].largest) {
			pdi = i;
			pti = ptmeta[i].lstart;
			found = true;
			break;
		}
	}
	
	// Allocate zero page
	if (!found) {
		pti = ptmeta[0].lstart;	
		if ((pages > ptmeta[0].start) && (pages > ptmeta[0].largest)) {
			return reinterpret_cast<vaddr_t>(-1);
		}
	}

	vaddr = (pdi << 22) + (pti << 12);
	
	// Write present to page table
	if (pages == 1) {
		pt_map(0, (vaddr_t)((pdi << 22) + (pti << 12)), PRESENT);
	} else {
		pt_map(0, (vaddr_t)((pdi << 22) + (pti << 12)), PRESENT | RANGE);
		pages--;
		pti++;
		for (; pages > 0; pages--) {
			pt_map(0, (vaddr_t)((pdi << 22) + (pti << 12)), PRESENT);
			pti++;
			if (pti == 1024) {
				set_ptinfo(pdi);
				pdi++;
				pti = 0;
			}
		}
		pt_map(0, (vaddr_t)((pdi << 22) + (pti << 12)), PRESENT | RANGE);
		pti++;
	}
	
	return reinterpret_cast<vaddr_t>(vaddr);
}

void vfree(vaddr_t vaddr) {
	// TODO: Implement
}
