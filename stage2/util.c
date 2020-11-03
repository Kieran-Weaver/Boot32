#include "util.h"
#include "crt0.h"

void kprint(const uint8_t* in, volatile uint8_t* screen, uint16_t n){
	for (uint16_t i = 0; i < n; i++){
		*(screen++) = in[i];
		*(screen++) = 0x0F;
	}
}

void hextostr(uint32_t in, uint8_t* out){
	uint8_t *bytes = "0123456789ABCDEF";
	for (uint8_t i = 0; i < 8; i++){
		out[7-i] = bytes[in & 0x0F];
		in >>= 4;
	}
}

int strlen(const uint8_t* str){
	const uint8_t* s;
	for (s = str; *s; ++s){}
	return (s - str);
}

int kputs(const uint8_t* in, volatile uint8_t* screen, uint16_t pos) {
	uint16_t offset = pos % 160;
	uint16_t len = strlen(in);
	if ((offset + len) > 160) {
		pos = (pos - offset) + 160;
	}
	kprint(in, screen + pos, len);
	return pos + len;
}

void __assert_fail(const char * x, const char * file, int line, const char * func) {
	volatile uint8_t* screen = (volatile uint8_t*)0xB8000;
	uint16_t pos = 0;
	uint8_t linestr[] = "0x00000000";
	hextostr(line, linestr + 2);
	pos = kputs("Panic: ", screen, pos);
	pos = kputs(x, screen, pos);
	pos = kputs(", File: ", screen, pos);
	pos = kputs(file, screen, pos);
	pos = kputs(", Line: ", screen, pos);
	pos = kputs(linestr, screen, pos);
	pos = kputs(", Func: ", screen, pos);
	pos = kputs(func, screen, pos);
	bhang();
}
