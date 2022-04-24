#include <crt/util.h>

// If any of the bytes in X have a value less than N
#define HASLESS(x,n) (((x)-~0UL/255*(n))&~(x)&~0UL/255*128)
#define MASK(x) ((1 << x) - 1)

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

int memcmp(const void* s1, const void* s2, size_t n) {
	const uint8_t* u1 = (const uint8_t*)s1;
	const uint8_t* u2 = (const uint8_t*)s2;

	for (; n > 0; n--) {
		if (*(u1++) != *(u2++)) {
			return *(u1 - 1) - *(u2 - 1);
		}
	}
	
	return 0;
}
