#ifndef PMM_H
#define PMM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <x86/e820.h>

void     pmm_init(SMAP32_t* memmap, size_t e820_size);
uint32_t pmm_alloc_ppage(void);
void     pmm_free_ppage(uint32_t);
uint32_t pmm_free_pages(void);

#ifdef __cplusplus
}
#endif

#endif
