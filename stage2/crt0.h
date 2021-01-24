#ifndef CRT0_H
#define CRT0_H

#include <stdint.h>
#include <stddef.h>
#include "e820.h"

typedef void (*kmain)(SMAP32_t*, size_t);
void setGdt(const uint64_t* gdt, uint16_t size, uint16_t cs, uint16_t ds);
void setDrive(uint16_t drive);
void readSector(uint32_t sector, uint16_t drive, uint8_t* buffer, uint32_t bufsize);
void loadPageDirectory(uint32_t*);
void enablePaging();
void enter(kmain entry, uint32_t stack, SMAP32_t* e820, size_t e820_size);
void bhang();

#endif
