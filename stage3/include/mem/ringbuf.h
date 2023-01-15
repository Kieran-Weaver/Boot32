#ifndef RINGBUF_H
#define RINGBUF_H

#include <stdint.h>
#include <crt/atomic.h>

typedef _Atomic uint32_t bufhdr;

/* All buffers must use power-of-two sizes */
/* Adapted from http://nullprogram.com/blog/2022/05/14/ */

void buf_init(bufhdr* hdr);
uint32_t buf_elements(bufhdr* hdr, int size);
uint32_t buf_write(bufhdr* hdr, int size, int num);
void buf_write_commit(bufhdr* hdr, int num);
uint32_t buf_read(bufhdr* hdr, int size, int num);
void buf_read_commit(bufhdr* hdr, int num);

#endif
