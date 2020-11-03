#ifndef CRT0_H
#define CRT0_H

#include <stdint.h>

void setGdt(const uint64_t* gdt, uint16_t size, uint16_t cs, uint16_t ds);
void setDrive(uint16_t drive);
void readSector(uint32_t sector, uint16_t drive, uint8_t* buffer, uint32_t bufsize);
void loadPageDirectory(uint32_t*);
void enablePaging();

#endif
