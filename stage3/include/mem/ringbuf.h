#ifndef RINGBUF_H
#define RINGBUF_H

#include <stdint.h>
#include <crt/atomic.h>
#include <crt/header.h>

EXTERN_C_BEGIN

typedef _Atomic uint32_t bufhdr_t;

/* All buffers must use power-of-two sizes */
/* Adapted from http://nullprogram.com/blog/2022/05/14/ */

void buf_init(bufhdr_t* hdr);
uint32_t buf_elements(const bufhdr_t* hdr, uint32_t size);
int32_t buf_write(bufhdr_t* hdr, uint32_t size, uint32_t num);
void buf_write_commit(bufhdr_t* hdr, uint32_t num);
int32_t buf_read(bufhdr_t* hdr, uint32_t size, uint32_t num);
void buf_read_commit(bufhdr_t* hdr, uint32_t num);

EXTERN_C_END

#endif
