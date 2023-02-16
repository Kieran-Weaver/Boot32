#ifndef INTRINSICS_H
#define INTRINSICS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define EFLAGS_CF  (1 << 0)
#define EFLAGS_PF  (1 << 2)
#define EFLAGS_AF  (1 << 4)
#define EFLAGS_ZF  (1 << 6)
#define EFLAGS_SF  (1 << 7)
#define EFLAGS_TF  (1 << 8)
#define EFLAGS_IF  (1 << 9)
#define EFLAGS_DF  (1 << 10)
#define EFLAGS_OF  (1 << 11)
#define EFLAGS_IOPL ((1 << 12) | (1 << 13))
#define EFLAGS_NT  (1 << 14)
#define EFLAGS_RF  (1 << 16)
#define EFLAGS_VM  (1 << 17)
#define EFLAGS_AC  (1 << 18)
#define EFLAGS_VIF (1 << 19)
#define EFLAGS_VIP (1 << 20)
#define EFLAGS_ID  (1 << 21)

static inline uint32_t read_eflags(void) {
	uint32_t ret;
	asm volatile ( "pushfl; pop %0" : "=g"(ret) : );
	return ret;	
}

static inline void outb(uint16_t port, uint8_t val){
	asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port){
	uint8_t ret;
	asm volatile ( "inb %1, %0"
			: "=a"(ret)
			: "Nd"(port) );
	return ret;
}

static inline void rep_insw(uint16_t port, uint16_t* buf, uint32_t ecx){
	asm volatile ("cld ; rep ; insw":"=D" (buf),
			"=c" (ecx):"d" (port), "0" (buf), "1" (ecx));
}

static inline void cli(void) {
	asm volatile ("cli");
}

static inline void halt(void) {
	asm volatile ("hlt");
}

static inline void hang(void) {
	asm volatile ("cli; hlt");
}

static inline void sti(void) {
	asm volatile ("sti");
}

static inline uint32_t save_intr(void) {
	uint32_t ret;
	asm volatile ( "pushfl; cli; pop %0" : "=r"(ret) : : "memory" );
	return ret;
}

static inline void restore_intr(uint32_t eflags) {
	asm volatile ( "push %0; popf" : : "rm"(eflags) : "memory","cc" );
}

static inline uint32_t read_cr4(void)
{
	uint32_t val;
	asm volatile ( "mov %%cr4, %0" : "=r"(val) );
	return val;
}

static inline void invlpg(void* m)
{
	asm volatile ( "invlpg (%0)" : : "b"(m) : "memory" );
}

static inline void io_wait(void)
{
	outb(0x80, 0);
}

static inline uint16_t bswap16(uint16_t x)
{
#ifdef __GNUC__
	return __builtin_bswap16(x);
#else
	return (x << 8) | (x >> 8);
#endif
}

static inline uint16_t bswap32(uint16_t x)
{
#ifdef __GNUC__
	return __builtin_bswap32(x);
#else
	return (x << 24) | ((x & 0x00FF0000) >> 8) | ((x & 0xFF00) << 8) | (x >> 24);
#endif
}

static inline uint32_t popcount(uint32_t x)
{
#ifdef __GNUC__
	return __builtin_popcount(x);
#else
	uint32_t c;
	for (c = 0; x; c++) {
		x &= (x - 1);
	}
	return x;
#endif
}

static inline uint32_t parity(uint32_t x)
{
#ifdef __GNUC__
	return __builtin_parity(x);
#else
	x ^= x >> 1;
	x ^= x >> 2;
	x = (x & 0x11111111U) * 0x11111111U;
	return (x >> 28) & 1;
#endif
}

static inline uint32_t clz(uint32_t x)
{
	if (x == 0) {
		return 32;
	}
#ifdef __GNUC__
	return __builtin_clz(x);
#else
	uint32_t r;
	uint32_t shift;
	r     = (v > 0xFFFF) << 4; v >>= r;
	shift = (v > 0xFF  ) << 3; v >>= shift; r |= shift;
	shift = (v > 0xF   ) << 2; v >>= shift; r |= shift;
	shift = (v > 0x3   ) << 1; v >>= shift; r |= shift;
                                            r |= (v >> 1);
	return r;
#endif
}

static inline uint32_t ctz(uint32_t x)
{
	if (x == 0) {
		return 32;
	}
#ifdef __GNUC__
	return __builtin_ctz(x);
#else
	uint32_t c = 32;

	x &= -((int32_t)(x));
	if (x) c--;
	if (x & 0x0000FFFF) c -= 16;
	if (x & 0x00FF00FF) c -= 8;
	if (x & 0x0F0F0F0F) c -= 4;
	if (x & 0x33333333) c -= 2;
	if (x & 0x55555555) c -= 1;
	
	return c;
#endif
}

void loadPageDirectory(uint32_t*);

#ifdef __cplusplus
}
#endif

#endif
