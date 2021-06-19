#ifndef VADDR_H
#define VADDR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef uint32_t* vaddr_t;
typedef uint32_t  paddr_t;

enum PT {          // Set           / Unset
	PRESENT = 1,   // Present       / not present
	RW      = 2,   // Read          / Write
	USER    = 4,   // User          / Supervisor
	WTCACHE = 8,   // Write-through / Write-back
	CDIS    = 16,  // Cache disable / Cache enable
	ACCESS  = 32,  // Accessed      / Not accessed
	DIRTY   = 64,  // Dirty         / Clean
	SIZE    = 128, // 4MiB Page     / 4KiB Page
	GLOBAL  = 256, // Global        / Not Global
	RANGE   = 512, // Start or end of range
	SWAP    = 1024, // Swapped out
	FULL    = 2048 // No more space
};

// Get paddr from current page table
paddr_t  get_paddr(vaddr_t vaddr);
// Initialize valloc and vfree
void     vinit();
// Allocate virtual pages
vaddr_t  valloc(uint32_t pages);
// Free virtual pages
uint32_t vfree(vaddr_t vaddr);
// Map a single virtual page
bool     pt_map(paddr_t paddr, vaddr_t vaddr, uint32_t flags);
// Unmap a single virtual page
uint32_t pt_unmap(vaddr_t vaddr);

#ifdef __cplusplus
}
#endif

#endif
