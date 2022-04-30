#ifndef UTIL_H
#define UTIL_H
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int   strlen(const char* str);
int   memcmp(const void* s1, const void* s2, size_t n);
void* memset(void* s, int c, size_t n);
void* memcpy(void* __restrict dst, const void* __restrict src, size_t n);
void* memmove(void* dst, const void* src, size_t n);

#ifdef __cplusplus
}
#endif


#endif
