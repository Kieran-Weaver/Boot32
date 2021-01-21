#include "pagetable.h"
#include "intrinsics.h"

void PageTable::makePD(uint32_t* newPD) {
	uint32_t paddr = get_paddr(reinterpret_cast<uint32_t>(newPD));
	this->PD = newPD;
	
	for (uint16_t i = 0; i < 1023; i++) {
		PD[i] = 0x00000002;
	}
	
	PD[1023] = 0x00000003 | paddr; // Page Directory
}

void PageTable::readPD(void) {
	this->PD = reinterpret_cast<uint32_t*>(0xFFFFF000);
}

void PageTable::writePD(void) {
	loadPageDirectory(reinterpret_cast<uint32_t*>(this->PD[1023] & 0xFFFFF000));
	this->readPD();
}

void PageTable::addPT(uint32_t* newPT, uint32_t vaddr) {
	uint32_t  pdi = (vaddr >> 22);
	uint32_t* pagetable = reinterpret_cast<uint32_t*>(0xFFC00000) + (0x400 * pdi);

	this->PD[pdi] = reinterpret_cast<uint32_t>(newPT) | 0x03;
	if (this->PD == reinterpret_cast<uint32_t*>(0xFFFFF000)) {
		invlpg(pagetable);
	}
}

bool PageTable::map(uint32_t paddr, uint32_t vaddr, uint32_t flags) {
	uint32_t  pdi = (vaddr >> 22);
	uint32_t  pti = (vaddr >> 12) & 0x3FF;
	uint32_t* pagetable = ((uint32_t*)0xFFC00000) + (0x400 * pdi);

	vaddr &= 0xFFFFF000;
	
	if ((this->PD[pdi] & 1U) == 0) { // Not present
		return false;
	}
	
	pagetable[pti] = paddr | (flags & 0xFFF) | 0x01;
	if (this->PD == reinterpret_cast<uint32_t*>(0xFFFFF000)) {
		invlpg(reinterpret_cast<void*>(vaddr));
	}
	
	return true;
}

uint32_t get_paddr(uint32_t vaddr) {
	uint32_t pdi = vaddr >> 22;
	uint32_t pti = vaddr >> 12 & 0x03FF;
	uint32_t* pd = reinterpret_cast<uint32_t*>(0xFFFFF000);
	uint32_t* pt = reinterpret_cast<uint32_t*>(0xFFC00000) + (0x400 * pdi);

	if (((pd[pdi] & 1U) == 0) || ((pt[pti] & 1U) == 0)) {
		return 0;
	}
 
	return (pt[pti] & ~0xFFF) + (vaddr & 0xFFF);
}
