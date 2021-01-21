#ifndef PAGETABLE_H
#define PAGETABLE_H

#include <stdint.h>

enum class PT {    // Set           / Unset
	PRESENT = 1,   // Present       / not present
	RW      = 2,   // Read          / Write
	USER    = 4,   // User          / Supervisor
	WTCACHE = 8,   // Write-through / Write-back
	CDIS    = 16,  // Cache disable / Cache enable
	ACCESS  = 32,  // Accessed      / Not accessed
	DIRTY   = 64,  // Dirty         / Clean
	SIZE    = 128, // 4MiB Page     / 4KiB Page
	GLOBAL  = 256  // Global        / Not Global
};

uint32_t get_paddr(uint32_t vaddr); // Get paddr from current page table

class PageTable {
public:
	void makePD(uint32_t* newPD);   // Create a new page directory
	void readPD(void);              // Read the page directory from the CPU
	void writePD(void);             // Load the page directory into the CPU
	void addPT(uint32_t* newPT, uint32_t vaddr); // Add a page table to the page directory
	bool map(uint32_t paddr, uint32_t vaddr, uint32_t flags); // Map paddr to vaddr 
private:
	uint32_t* PD = nullptr;
};

#endif
