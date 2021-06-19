#ifndef PAGETABLE_H
#define PAGETABLE_H

#include "vaddr.h"

class PageTable {
public:
	PageTable() = default;
	PageTable(vaddr_t newPD);   // Create a new page directory
	void set(vaddr_t  newPD);   // Set a new page directory
	void activate(void);        // Load the page directory into the CPU
	void addPT(vaddr_t newPT, vaddr_t vptr);  // Add a page table to the page directory
	bool remap(vaddr_t vptr, uint32_t flags); // Map the paddr to a new address
private:
	vaddr_t vaddr = 0;
	paddr_t paddr = 0;
};

#endif
