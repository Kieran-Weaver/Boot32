#include <stdint.h>
#include "e820.h"
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
	uint8_t * screen = (uint8_t*)0xB8000;
	kprint(" Base       | Length     | Type", screen, 32);
	screen += 160;
	uint8_t str[] = " 0x00000000 | 0x0009FC00 | 1";
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
