#ifndef SLAB_H
#define SLAB_H

#include <stdint.h>
#include <stddef.h>
#include "vaddr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SLABP_SIZE ((PAGE_SIZE / sizeof(uint32_t)) - 2)

struct __attribute__((aligned(PAGE_SIZE))) slab_page {
	uint16_t free; // Number of free elements
	uint16_t size; // Size of each element in uint32_ts
	uint32_t first_free; // Offset of first free element
	uint32_t data[SLABP_SIZE];
};

void  slabp_init(struct slab_page* slb, uint16_t size);
void* slabp_alloc(struct slab_page* slb);
void  slabp_free(struct slab_page* slb, void* data);

#ifdef __cplusplus
}

/* From OSDev wiki: operator new and delete */
inline void *operator new(size_t, void *p)     throw() { return p; }
inline void *operator new[](size_t, void *p)   throw() { return p; }
inline void  operator delete  (void *, void *) throw() { };
inline void  operator delete[](void *, void *) throw() { };
#endif

#endif
