#include <mem/ringbuf.h>

#define GRAB_HEAD_TAIL(hdr, size) \
	uint32_t r = atomic_load_explicit(hdr, memory_order_acquire); \
	uint32_t mask = size - 1; \
	uint32_t head = r & mask; \
	uint32_t tail = (r>>16) & mask

void buf_init(bufhdr* hdr) {
	atomic_store(hdr, 0);
}

uint32_t buf_elements(bufhdr* hdr, uint32_t size) {
	GRAB_HEAD_TAIL(hdr, size);
	return head - tail;
}

int32_t buf_write(bufhdr* hdr, uint32_t size, uint32_t num) {
	GRAB_HEAD_TAIL(hdr, size);
	int32_t capacity = size - (head - tail);
	if (capacity < num) {
		return -capacity;
	} else {
		return tail;
	}
}

int32_t buf_read(bufhdr* hdr, uint32_t size, uint32_t num) {
	GRAB_HEAD_TAIL(hdr, size);
	int32_t elements = head - tail;
	if (elements < num) {
		return -elements;
	} else {
		return tail;
	}
}

void buf_write_commit(bufhdr* hdr, uint32_t num) {
	*hdr += atomic_fetch_add_explicit(hdr, num, memory_order_release);
}

void buf_read_commit(bufhdr* hdr, uint32_t num) {
	*hdr += atomic_fetch_add_explicit(hdr, num * 0x10000, memory_order_release);
}
