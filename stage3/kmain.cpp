#include <stdint.h>
#include "e820.h"
#include "util.h"

void kprint(const uint8_t* in, volatile uint8_t* screen, uint16_t n){
	for (uint16_t i = 0; i < n; i++){
		*(screen++) = in[i];
		*(screen++) = 0x0F;
	}
}

void hextostr(uint32_t in, uint8_t* out){
	const uint8_t *bytes = reinterpret_cast<const uint8_t*>("0123456789ABCDEF");
	for (uint8_t i = 0; i < 8; i++){
		out[7-i] = bytes[in & 0x0F];
		in >>= 4;
	}
}

void clear(volatile uint8_t* screen) {
	for (int i = 0; i < (160 * 25); i++) {
		screen[i] = 0;
	}
}

extern "C" void kmain(SMAP32_t* e820, size_t e820_size) {
	volatile uint8_t* screen = (uint8_t*)0xB8000;
	const uint8_t* header = reinterpret_cast<const uint8_t*>(" Base       | Length     | Type");
	uint8_t str[] = " 0x00000000 | 0x0009FC00 | 1";

	clear(screen);
	kprint(header, screen, strlen(header));
	screen += 160;
	
	for (uint16_t i = 0; i < e820_size; i++){
		hextostr(e820[i].base, str + 3);
		hextostr(e820[i].length, str + 16);
		str[27] = '0' + e820[i].type;
		kprint(str, screen, 29);
		screen += 160;
	}
}
