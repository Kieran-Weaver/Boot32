#include <stdint.h>
int strlen(const uint8_t* str){
	const uint8_t* s;
	for (s = str; *s; ++s){}
	return (s - str);
}

void kprint(const uint8_t* in, volatile uint8_t* screen, uint16_t n){
	for (uint16_t i = 0; i < n; i++){
		*(screen++) = in[i];
		*(screen++) = 0x0F;
	}
}

void clear(volatile uint8_t* screen) {
	for (int i = 0; i < (160 * 25); i++) {
		screen[i] = 0;
	}
}

extern "C" void kmain() {
	volatile uint8_t* screen = (uint8_t*)0xB8000;
	clear(screen);
	const uint8_t* str1 = reinterpret_cast<const uint8_t*>("Roses are #ff0000");
	const uint8_t* str2 = reinterpret_cast<const uint8_t*>("Violets are #0000ff");
	const uint8_t* str3 = reinterpret_cast<const uint8_t*>("All of my base");
	const uint8_t* str4 = reinterpret_cast<const uint8_t*>("Are belong to you");
	const uint8_t* str5 = reinterpret_cast<const uint8_t*>("Operating system not found");
	kprint(str1, screen, strlen(str1));
	kprint(str2, screen + 160, strlen(str2));
	kprint(str3, screen + 320, strlen(str3));
	kprint(str4, screen + 480, strlen(str4));
	kprint(str5, screen + 640, strlen(str5));
}
