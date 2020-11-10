#include "util.h"
#include "crt0.h"

// If any of the bytes in X have a value less than N
#define HASLESS(x,n) (((x)-~0UL/255*(n))&~(x)&~0UL/255*128)
#define MASK(x) ((1 << x) - 1)

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

int strlen(const uint8_t* str) {
	const uint8_t* s;
	const uint32_t* w;

	for (s = str; ((uintptr_t)s) & MASK(sizeof(uintptr_t)); s++) {
		if (*s == 0) {
			return s - str;
		}
	}

	for (w = (const uint32_t*)s; !HASLESS(*w, 1); ++w){}
	
	for (s = (const uint8_t*)w; *s; s++) {}

	return (s - str);
}

int memcmp(const void* _s1, const void* _s2, size_t n) {
	const uint8_t* s1 = _s1;
	const uint8_t* s2 = _s2;
	int i;
	
	for (i = 0; i < n; i++) {
		if (*s1 == *s2) {
			s1++;
			s2++;
		} else {
			return (*s1 - *s2);
		}
	}
	
	return 0;
}

int kputs(const uint8_t* in, volatile uint8_t* screen, uint16_t pos) {
	uint16_t offset = pos % 160;
	uint16_t len = strlen(in);
	if ((offset + len) > 160) {
		pos = (pos - offset) + 160;
	}
	kprint(in, screen + pos, len);
	return pos + (len * 2);
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

uint32_t min(uint32_t a, uint32_t b){
	return (a < b) ? a : b;
}

uint32_t max(uint32_t a, uint32_t b){
	return (a > b) ? a : b;
}

void* memset(void* s, int c, size_t n) {
	uint8_t* dst = s;
	uint32_t i;
	for (i = 0; i < n; i++) {
		dst[i] = c;
	}
	return s;
}
