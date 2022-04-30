#include <mem/kmalloc.h>
#include <mem/slab.h>
#include <crt/sort.h>
#include <mem/pmm.h>
#include <crt/assert.h>

const static uint32_t* kpd  = reinterpret_cast<uint32_t*>(0xFFFFFC00);

struct ptinfo {
	bool operator <(const struct ptinfo& b) const {
		return this->free < b.free;
	}
	uint16_t free;
	uint16_t pdi;
	uint16_t first_free;
};

// Index into ptmeta
static uint16_t ptmeta_first_free;
static ptinfo ptmeta[256];

static void set_ptinfo(uint16_t i) {
	const uint32_t* pt = reinterpret_cast<const uint32_t*>(0xFFF00000) + (0x400 * i);

	if ((i == 255) || (((uint32_t)pt) & SIZE)) {
		ptmeta[i].free = 0;
	} else if ((kpd[i] & PRESENT) != 0) {
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
	for (i = 0; i < 256; i++) {
		ptmeta[i].pdi = i;
		ptmeta[i].first_free = 0;
		set_ptinfo(i);
		assert(ptmeta[i].first_free < 1024);
		assert(ptmeta[i].free <= 1024);
	}
	sort(ptmeta, ptmeta + 256);
	for (i = 0; (ptmeta[i].free == 0) && (i < 256); i++) {}
	ptmeta_first_free = i;
}

static uint16_t get_idx(uint32_t pdi) {
	for (uint16_t i = 0; i < 256; i++) {
		if (ptmeta[i].pdi == pdi) return i;
	}
	
	assert(0);
}

static void update_first_free(uint16_t idx, uint16_t pdi) {
	uint32_t* pt = reinterpret_cast<uint32_t*>(0xFFF00000) + (0x400 * pdi);
	uint16_t pti = 0;

	if ((kpd[pdi] & PRESENT) == 0) {
		/* Allocate page directory */
		pt_map(pmm_alloc_ppage(), pt, 2);
	}
	
	/* Find next free page */
	do {
		pti++;
	} while ((pti < 1024) && ((pt[pti] & PRESENT) != 0));
	
	ptmeta[idx].first_free = pti;
}

vaddr_t valloc(void) {
	uint16_t idx = 0;
	uint16_t pdi = 0;
	uint16_t pti = 0;
	uint32_t vaddr = 0;
	bool found = false;

	if (ptmeta_first_free > 255) return (vaddr_t)0xFFFFFFFF;
	
	idx = ptmeta_first_free;
	assert(ptmeta[idx].free != 0);
	
	pdi = ptmeta[idx].pdi;
	pti = ptmeta[idx].first_free;
	assert(pti < 1024);
	if ((--ptmeta[idx].free) != 0) update_first_free(idx, pdi);
	else ptmeta_first_free++;

	vaddr = ((pdi + 768) << 22) + (pti << 12);
	pt_map(pmm_alloc_ppage(), reinterpret_cast<vaddr_t>(vaddr), 2);
	
	return reinterpret_cast<vaddr_t>(vaddr);
}

void vfree(vaddr_t vaddr) {
	uint32_t  pdi = (reinterpret_cast<uint32_t>(vaddr) >> 22) - 768;
	uint32_t  pti = (reinterpret_cast<uint32_t>(vaddr) >> 12) & 0x3FF;
	uint32_t* pt = reinterpret_cast<uint32_t*>(0xFFF00000) + (0x400 * pdi);
	uint16_t  idx = get_idx(pdi);
	
	if ((ptmeta[idx].free == 0) && (idx < ptmeta_first_free)) {
		ptmeta_first_free = pdi;
	}

	if ((ptmeta[idx].free == 0) || (pti < ptmeta[idx].first_free)) {
		ptmeta[idx].first_free = pti;
	}

	pmm_free_ppage(pt_unmap(vaddr));
	ptmeta[idx].free++;
	if (ptmeta[idx].free == 1024) {
		ptmeta[idx].first_free = 0;
		pmm_free_ppage(pt_unmap(pt));
	} else if (ptmeta[idx].free > 1024) {
		assert(0);
	}
	
	while ((idx + 1) < 256) {
		if (ptmeta[idx + 1] < ptmeta[idx]) {
			ptinfo tmp = ptmeta[idx + 1];
			ptmeta[idx + 1] = ptmeta[idx];
			ptmeta[idx++] = tmp;
		} else break;
	}
}

// Allocate block
void*   kmalloc(size_t size);
// Deallocate block
void    kfree(void* ptr);
