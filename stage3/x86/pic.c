#include <x86/intrinsics.h>
#include <x86/pic.h>
#include <crt/header.h>

#define PIC1_CMD	0x20
#define PIC1_DATA	0x21
#define PIC2_CMD	0xA0
#define PIC2_DATA	0xA1
#define ICW1_ICW4	0x01
#define ICW1_INIT	0x10
#define ICW4_8086	0x01
#define PIC_EOI		0x20

EXTERN_C_BEGIN

void pic_init(int offset) {
	/* ICW1: start init */
	outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
	io_wait();
	/* ICW2: set offset */
	outb(PIC1_DATA, offset);
	io_wait();
	outb(PIC2_DATA, offset + 8);
	io_wait();
	/* ICW3: configure cascade */
	outb(PIC1_DATA, 4);
	io_wait();
	outb(PIC2_DATA, 2);
	io_wait();
	/* ICW4: configure 8086 mode */
	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();
}

void pic_irqmask(uint16_t mask) {
	outb(PIC1_DATA, mask & 0xFF);
	outb(PIC2_DATA, (mask & 0xFF00) >> 8);
}

void pic_irqmask1(uint8_t mask) {
	outb(PIC1_DATA, mask);
}

void pic_irqmask2(uint8_t mask) {
	outb(PIC2_DATA, mask);
}

void pic_disable(void) {
	pic_irqmask(0xFFFF);
}

uint16_t pic_read(int reg) {
	outb(PIC1_CMD, reg);
	outb(PIC2_CMD, reg);
	return (inb(PIC2_CMD) << 8) | inb(PIC1_CMD);
}

void pic_eoi(int irq) {
	if(irq >= 8)
		outb(PIC2_CMD,PIC_EOI);
	outb(PIC1_CMD,PIC_EOI);
}

EXTERN_C_END
