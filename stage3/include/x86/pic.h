#ifndef PIC_H
#define PIC_H

#include <stdint.h>

#define PIC_IRR 0x0A
#define PIC_ISR 0x0B

#define PIC_PIT   0
#define PIC_KBD   1
#define PIC_CASCADE 2
#define PIC_COM2  3
#define PIC_COM1  4
#define PIC_LPT2  5
#define PIC_FLP   6
#define PIC_LPT1  7
#define PIC_CMOS  8
#define PIC_FREE1 9
#define PIC_FREE2 10
#define PIC_FREE3 11
#define PIC_MOUSE 12
#define PIC_FPU   13
#define PIC_ATA1  14
#define PIC_ATA2  15

#ifdef __cplusplus
extern "C" {
#endif

void pic_init(int offset);
void pic_disable(void);
void pic_irqmask(uint16_t mask);
void pic_eoi(int irq);
uint16_t pic_read(int reg);
/* Set the mask for the PICs individually */
void pic_irqmask1(uint8_t mask);
void pic_irqmask2(uint8_t mask);

#ifdef __cplusplus
}
#endif

#endif
