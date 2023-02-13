#ifndef PIC_H
#define PIC_H

#include <stdint.h>

#define PIC_IRR 0x0A
#define PIC_ISR 0x0B

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
