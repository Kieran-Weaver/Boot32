#include <stdint.h>
#include "e820.h"
void setGdt(const uint64_t* gdt, uint16_t size, uint16_t cs, uint16_t ds);
void kprint(uint8_t* in, uint8_t* screen, uint16_t n){
	for (uint16_t i = 0; i < n; i++){
		*(screen++) = in[i];
		*(screen++) = 0x0F;
	}
}

void hextostr(uint32_t in, uint8_t* out){
	uint8_t *bytes = "0123456789ABCDEF";
	for (uint8_t i = 0; i < 8; i++){
		out[8-i] = bytes[in & 0x0F];
		in >>= 4;
	}
}

void bmain(uint16_t e820, uint16_t dx){
	const uint64_t GDT[8] __attribute__((aligned(16))) = {
		0x0,
		0x008F92000000FFFF,  // flat ds, offset 0x08
		0x000092000000FFFF,  // 16 bit legacy real mode ds, offset 0x10
		0x000098000000FFFF,  // 16 bit legacy real mode cs, offset 0x18
		0x00CF9A000000FFFF,  // CS for Ring 0, offset 0x20
		0x00CF92000000FFFF,  // DS for Ring 0, offset 0x28
		0x00C089009F000100,  // TSS, offset 0x30
		0					 // Blank, offset 0x38
	};

	setGdt(GDT, 0x40, 0x20, 0x28);
	uint8_t * screen = (uint8_t*)0xB8000;
	kprint(" Base       | Length     | Type", screen, 32);
	screen += 160;
	uint8_t str[] = " 0x00000000 | 0x0009FC00 | 1";
	uint16_t buf[256];
	return_t entries = validate((SMAP_entry_t*)0x7C00, e820);
	for (uint16_t i = 0; i < entries.n; i++){
		hextostr(entries.data->base, str + 2);
		hextostr(entries.data->length, str + 15);
		str[27] = '0' + entries.data->type;
		kprint(str, screen, 29);
		screen += 160;
		(entries.data)++;
	}
}
