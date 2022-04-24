#include <mem/vaddr.h>
#include <mem/pagetable.h>
#include <x86/intrinsics.h>
#include <crt/sort.h>

/* Physical memory management */

const static uint32_t* pd  = reinterpret_cast<uint32_t*>(0xFFFFF000);

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
		paddr = pagetable[pti] & PT_PAGE_MASK;
		pagetable[pti] = pagetable[pti] & ~(PRESENT);
		invlpg(reinterpret_cast<void*>(vaddr));
	}
	
	return paddr;
}

uint32_t get_flags(vaddr_t vaddr) {
	const uint32_t  pdi = (reinterpret_cast<uint32_t>(vaddr) >> 22);
	const uint32_t  pti = (reinterpret_cast<uint32_t>(vaddr) >> 12) & 0x3FF;
	const uint32_t* pt = reinterpret_cast<const uint32_t*>(0xFFC00000) + (0x400 * pdi);

	return pt[pti] & 0xE00;
}

/* Virtual memory management */
struct ptinfo {
	bool operator <(const struct ptinfo& b) const {
		return this->free < b.free;
	}
	uint16_t free;
	uint16_t idx;
	uint16_t first_free;
};

static uint16_t ptmeta_first_free;
static ptinfo ptmeta[1024];

static void set_ptinfo(uint16_t i) {
	const uint32_t* pt = reinterpret_cast<const uint32_t*>(0xFFC00000) + (0x400 * i);

	if ((i == 1023) || (((uint32_t)pt) & SIZE)) {
		ptmeta[i].free = 0;
	} else if ((pd[i] & PRESENT) != 0) {
		uint16_t counter;

		ptmeta[i].free = 0;
		for (counter = 0; counter < 1024; counter++) {
			if ((pt[counter] & PRESENT) == 0) {
				ptmeta[i].free++;
				ptmeta[i].first_free = counter;
				break;
			}
		}
		
		for (; counter < 1024; counter++) {
			ptmeta[i].free += (pt[counter] & PRESENT) == 0;
		}
	} else {
		ptmeta[i].free = 1024;
	}
}

void vinit(void) {
	uint16_t i = 0;
	for (i = 0; i < 1024; i++) {
		ptmeta[i].idx = i;
		ptmeta[i].first_free = 0;
		set_ptinfo(i);
	}
	sort(ptmeta, ptmeta + 1024);
	for (i = 0; ptmeta[i].free == 0; i++) {}
	ptmeta_first_free = i;
}

static void update_first_free(uint16_t pdi) {
	const uint32_t* pt = reinterpret_cast<const uint32_t*>(0xFFC00000) + (0x400 * pdi);
	uint32_t pti = ptmeta[pdi].first_free;

	if ((pd[pdi] & PRESENT) == 0) {
		ptmeta[pdi].first_free++;
	} else {
		do {
			pti++;
		} while ((pti < 1024) && ((pt[pti] & PRESENT) != 0));
	
		ptmeta[pdi].first_free = pti;
	}
}

vaddr_t valloc(void) {
	uint16_t pdi = 0;
	uint16_t pti = 0;
	uint32_t vaddr = 0;
	bool found = false;

	if (ptmeta_first_free > 1023) return (vaddr_t)0xFFFFFFFF;
	
	pdi = ptmeta_first_free;
	pti = ptmeta[pdi].first_free;
	if ((--ptmeta[pdi].free) != 0) update_first_free(pdi);
	else ptmeta_first_free++;

	vaddr = (pdi << 22) + (pti << 12);
	
	return reinterpret_cast<vaddr_t>(vaddr);
}

void vfree(vaddr_t vaddr) {
	uint32_t  pdi = (reinterpret_cast<uint32_t>(vaddr) >> 22);
	uint32_t  pti = (reinterpret_cast<uint32_t>(vaddr) >> 12) & 0x3FF;
	
	if ((ptmeta[pdi].free == 0) && (pdi < ptmeta_first_free)) {
		ptmeta_first_free = pdi;
	}

	if ((ptmeta[pdi].free == 0) || (pti < ptmeta[pdi].first_free)) {
		ptmeta[pdi].first_free = pti;
	}

	ptmeta[pdi].free++;
	
	while ((pdi + 1) < 1024) {
		if (ptmeta[pdi + 1] < ptmeta[pdi]) {
			ptinfo tmp = ptmeta[pdi + 1];
			ptmeta[pdi + 1] = ptmeta[pdi];
			ptmeta[pdi++] = tmp;
		} else break;
	}
}

void vmark(vaddr_t vaddr, uint32_t flags) {
	uint32_t  pdi = (reinterpret_cast<uint32_t>(vaddr) >> 22);
	uint32_t  pti = (reinterpret_cast<uint32_t>(vaddr) >> 12) & 0x3FF;
	uint32_t* pagetable = ((uint32_t*)0xFFC00000) + (0x400 * pdi);
	uint32_t  paddr = 0;

	if ((pd[pdi] & PRESENT) != 0) {
		paddr = pagetable[pti] & PT_PAGE_MASK;
		pagetable[pti] = paddr | flags;
		invlpg(reinterpret_cast<void*>(vaddr));
	}
}
