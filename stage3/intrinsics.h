#ifndef INTRINSICS_H
#define INTRINSICS_H
#include <stdint.h>
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
#endif
