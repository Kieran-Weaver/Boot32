#ifndef VADDR_H
#define VADDR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef uint32_t* vaddr_t;
typedef uint32_t  paddr_t;

struct __attribute__((aligned(4096))) page {
	struct page *addrs[1024 / sizeof(struct page*)];
};

#define PT_PAGE_MASK  0xFFFFF000
#define PT_FLAGS_MASK 0x00000FFF

enum PT {          // Set           / Unset
	PRESENT = 1,   // Present       / not present
	RW      = 2,   // Read+Write    / Read only
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
void vfree(vaddr_t vaddr);
// Map a single virtual page
paddr_t  pt_map(paddr_t paddr, vaddr_t vaddr, uint32_t flags);
// Unmap a single virtual page
uint32_t pt_unmap(vaddr_t vaddr);

#ifdef __cplusplus
}
#endif

#endif
