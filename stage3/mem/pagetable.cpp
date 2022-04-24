#include <mem/pagetable.h>
#include <x86/intrinsics.h>
#include <mem/vaddr.h>

PageTable::PageTable(vaddr_t newPD) {
	this->paddr = get_paddr(newPD);
	this->vaddr = newPD;
}

void PageTable::clear(void) {
	for (uint16_t i = 0; i < 1023; i++) {
		this->vaddr[i] = 0x00000002;
	}
	
	this->vaddr[1023] = 0x00000003 | this->paddr; // Page Directory recursively mapped
}

vaddr_t PageTable::set(vaddr_t newPD) {
	vaddr_t oldvaddr = this->vaddr;
	this->paddr = get_paddr(newPD);
	this->vaddr = newPD;
	return oldvaddr;
}

void PageTable::activate(void) {
	loadPageDirectory(reinterpret_cast<uint32_t*>(this->vaddr[1023] & 0xFFFFF000));
	this->vaddr = reinterpret_cast<vaddr_t>(0xFFFFF000);
}

void PageTable::addPT(uint16_t idx, paddr_t paddr) {
	this->vaddr[idx] = paddr | 0x03;
	
	if (this->vaddr == reinterpret_cast<vaddr_t>(0xFFFFF000)) { // PD is active
		invlpg(this->vaddr);
	}
}

bool PageTable::remap(vaddr_t vptr, uint32_t flags) {
	if (pt_map(this->paddr, vptr, flags)) {
		this->vaddr = vptr;
		return true;
	}
	
	return false;
}
