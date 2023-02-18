#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>
#include <x86/e820.h>
#include <crt/header.h>

EXTERN_C_BEGIN

void     pmm_init(SMAP32_t* memmap, size_t e820_size);
uint32_t pmm_alloc_ppage(void);
void     pmm_free_ppage(uint32_t);
uint32_t pmm_free_pages(void);

EXTERN_C_END

#endif
