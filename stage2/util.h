#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

#define assert(x) ((void)((x) || (__assert_fail(#x, __FILE__, __LINE__, __func__),0)))
void __assert_fail(const char *, const char *, int, const char *);
void hextostr(uint32_t in, uint8_t* out);
void kprint(const uint8_t* in, volatile uint8_t* screen, uint16_t n);
int  kputs(const uint8_t* in, volatile uint8_t* screen, uint16_t pos);
int  strlen(const uint8_t* str);

#endif
