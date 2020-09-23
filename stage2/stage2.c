#include <stdint.h>
typedef struct{
	uint64_t base;
	uint64_t length;
	uint32_t type;
	uint32_t acpi;
} SMAP_entry_t;
void kprint(uint8_t* in, uint8_t* screen, uint16_t n){
	for (uint16_t i = 0; i < n; i++){
		*(screen++) = in[i];
		*(screen++) = 0x0F;
	}
}
void hextostr(uint64_t in, uint8_t* out){
	uint8_t *bytes = "0123456789ABCDEF";
	for (uint8_t i = 0; i < 16; i++){
		out[15-i] = bytes[in & 0x0F];
		in >>= 4;
	}
}
void bmain(uint16_t e820, uint16_t dx){
	uint8_t * screen = (uint8_t*)0xB8000;
//	koutb('#', screen);
//	screen[0] = '#';
//	screen[1] = 0x0F;
//	screen[2] = '@';
//	screen[3] = 0x0F;
//	kprinthex(e820, screen + 2);
//	kprinthex(dx, screen + 10);
	kprint(" Base Address       | Length             | Type", screen, 48);
	screen += 160;
	uint8_t str[] = " 0x0000000000000000 | 0x000000000009FC00 | 1";
	SMAP_entry_t* entries = (SMAP_entry_t*)0x8000;
	for (uint16_t i = 0; i < e820; i++){
		hextostr(entries->base, str + 3);
		hextostr(entries->length, str + 24);
		str[43] = '0' + entries->type;
		kprint(str, screen, 45);
		screen += 160;
		entries++;
	}
}
