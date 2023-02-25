#ifndef VADDR_H
#define VADDR_H

#include <stdint.h>
#include <crt/header.h>

EXTERN_C_BEGIN

#define PAGE_SIZE 4096

typedef uint32_t* vaddr_t;
typedef uint32_t  paddr_t;

struct __attribute__((aligned(PAGE_SIZE))) page {
	struct page *addrs[PAGE_SIZE / sizeof(struct page*)];
};

#define PT_PAGE_MASK  0xFFFFF000
#define PT_FLAGS_MASK 0x00000FFF
#define PT_INVALID_PAGE 0xFFFFFFFF

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
//  Types of page, sets AVL bits
	RAM_FREE = 0,
	MMAP_PRESENT = 512,
	MMAP_NOT_PRESENT = (512 * 2),
	RAM_PRESENT = (512 * 3),
	RAM_ALLOC_ON_WRITE = (512 * 4),
	RAM_SWAP = (512 * 5),
	UNUSABLE = (512 * 6)
};

// Get paddr from current page table
paddr_t  get_paddr(vaddr_t vaddr);
// Get flags from current page table
uint32_t get_flags(vaddr_t vaddr);
// Map a single virtual page
paddr_t  pt_map(paddr_t paddr, vaddr_t vaddr, uint32_t flags);
// Map a page directory
paddr_t  pd_map(paddr_t paddr, vaddr_t vaddr, uint32_t flags);
// Unmap a single virtual page
paddr_t  pt_unmap(vaddr_t vaddr);
// Replace virtual page's flags with new flags
paddr_t  vmark(vaddr_t vaddr, uint32_t flags);

EXTERN_C_END

#endif
