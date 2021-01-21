#ifndef PT_H
#define PT_H

#include <stdint.h>
#include <stddef.h>
#include "e820.h"

void pt_init(SMAP32_t* _e820, size_t _e820_size);
void pt_map(uint32_t vaddr);

#endif
