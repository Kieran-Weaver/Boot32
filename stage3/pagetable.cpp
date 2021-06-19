#include "pagetable.h"
#include "intrinsics.h"
#include "vaddr.h"

PageTable::PageTable(vaddr_t newPD) {
	this->paddr = get_paddr(newPD);
	this->vaddr = newPD;
	
	for (uint16_t i = 0; i < 1023; i++) {
		this->vaddr[i] = 0x00000002;
	}
	
	this->vaddr[1023] = 0x00000003 | this->paddr; // Page Directory
}

void PageTable::set(vaddr_t newPD) {
	this->paddr = get_paddr(newPD);
	this->vaddr = newPD;
}

void PageTable::activate(void) {
	loadPageDirectory(reinterpret_cast<uint32_t*>(this->vaddr[1023] & 0xFFFFF000));
	this->vaddr = reinterpret_cast<vaddr_t>(0xFFFFF000);
}

void PageTable::addPT(vaddr_t newPT, vaddr_t vptr) {
	uint32_t pdi   = (reinterpret_cast<uint32_t>(vaddr) >> 22);
	uint32_t paddr = get_paddr(newPT);
	this->vaddr[pdi] = paddr | 0x03;
	
	if (this->vaddr == reinterpret_cast<vaddr_t>(0xFFFFF000)) { // PD is active
		invlpg(newPT);
	}
}

bool PageTable::remap(vaddr_t vptr, uint32_t flags) {
	if (pt_map(this->paddr, vptr, flags)) {
		this->vaddr = vptr;
		return true;
	}
	
	return false;
}
