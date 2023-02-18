#include <mem/ringbuf.h>
#include <crt/header.h>

EXTERN_C_BEGIN

#define MAX_ELEM 0x7FFF
#define GRAB_HEAD_TAIL(hdr, size) \
	uint32_t r = atomic_load_explicit(hdr, memory_order_acquire); \
	uint32_t mask = size - 1; \
	uint32_t head = r & MAX_ELEM; \
	uint32_t tail = (r>>16) & MAX_ELEM

void buf_init(bufhdr_t* hdr) {
	atomic_store(hdr, 0);
}

uint32_t buf_elements(const bufhdr_t* hdr, uint32_t size) {
	GRAB_HEAD_TAIL(hdr, size);
	return (head - tail);
}

int32_t buf_write(bufhdr_t* hdr, uint32_t size, uint32_t num) {
	GRAB_HEAD_TAIL(hdr, size);
	int32_t capacity = (size - (head - tail)) - 1;
	if (r & 0x8000) atomic_fetch_and(hdr, ~0x8000);
	if (capacity < num) {
		return -capacity;
	} else {
		return head & mask;
	}
}

int32_t buf_read(bufhdr_t* hdr, uint32_t size, uint32_t num) {
	GRAB_HEAD_TAIL(hdr, size);
	int32_t elements = (head - tail);
	if (elements < num) {
		return -elements;
	} else {
		return tail & mask;
	}
}

void buf_write_commit(bufhdr_t* hdr, uint32_t num) {
	atomic_fetch_add_explicit(hdr, num, memory_order_release);
}

void buf_read_commit(bufhdr_t* hdr, uint32_t num) {
	atomic_fetch_add_explicit(hdr, num * 0x10000, memory_order_release);
}

EXTERN_C_END
