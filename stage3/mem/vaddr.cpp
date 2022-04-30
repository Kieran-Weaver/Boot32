#include <mem/vaddr.h>
#include <mem/pagetable.h>
#include <x86/intrinsics.h>

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
