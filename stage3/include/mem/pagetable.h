#ifndef PAGETABLE_H
#define PAGETABLE_H

#include <mem/vaddr.h>

class PageTable {
public:
	PageTable() = default;
	PageTable(vaddr_t newPD);   // Create a new page directory
	void clear(void);           // Clear the current page directory
	vaddr_t set(vaddr_t newPD); // Set a new page directory, return old one
	void activate(void);        // Load the page directory into the CPU
	void addPT(uint16_t idx, paddr_t paddr);  // Add a page table to the page directory
	bool remap(vaddr_t vptr, uint32_t flags); // Map the paddr to a new address
private:
	vaddr_t vaddr = 0;
	paddr_t paddr = 0;
};

#endif
