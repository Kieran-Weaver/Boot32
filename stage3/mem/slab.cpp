#include <mem/slab.h>
#include <crt/assert.h>

bool  slabp_full(const struct slab_page* slb) {
	return slb->first_free == SLAB_SENTINEL;
}

void  slabp_init(struct slab_page* slb, uint16_t size) {
	assert((size >= 1) && (size <= SLABP_SIZE));
	
	slb->size = size;
	slb->first_free = 0;
	
	for (slb->free = 0; (slb->first_free + size) < SLABP_SIZE; slb->free++) {
		slb->data[slb->first_free] = slb->first_free + size;
		slb->first_free += size;
	}
	
	slb->data[slb->first_free] = SLAB_SENTINEL; // No next block
	slb->first_free = 0;
}

void* slabp_alloc(struct slab_page* slb) {
	uint32_t offset = slb->first_free;
	slb->first_free = slb->data[offset];
	return &(slb->data[offset]);
}

void  slabp_free(struct slab_page* slb, void* data) {
	uint32_t* block = (uint32_t*)data;
	uint32_t offset = block - (uint32_t*)(&(slb->data[0]));

	assert((offset >= 0) && (offset < SLABP_SIZE));
	
	slb->data[offset] = slb->first_free;
	slb->first_free = offset;
}
