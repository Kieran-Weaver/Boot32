#include <x86/serial.h>
#include <x86/intrinsics.h>

/* Serial port registers */
#define DATA 0
#define IER 1
#define DIVL 0
#define DIVH 1
#define IIR 2
#define FCR 2
#define LCR 3
#define MCR 4
#define LSR 5
#define MSR 6
#define SR 7

/* FIFO Control Register */
/* FIFO Depth */
#define FIFO_1   0x00
#define FIFO_4   0x40
#define FIFO_8   0x80
#define FIFO_14  0xC0
/* Extra commands */
#define FIFO_DMA 0x08
#define CLEAR_TX 0x04
#define CLEAR_RX 0x02
#define FIFO_EN  0x01

/* Line Control Register */
/* Data bits */
#define DATA_5 0x00
#define DATA_6 0x01
#define DATA_7 0x02
#define DATA_8 0x03
/* Stop bits */
#define STOP_1 0x00
#define STOP_2 0x04
/* Parity bits */
#define PARITY_NONE  0x00
#define PARITY_ODD   0x08
#define PARITY_EVEN  0x18
#define PARITY_MARK  0x28
#define PARITY_SPACE 0x38
/* DLAB bit */
#define DLAB 0x80

/* Modem Control Register */
#define DTR 0x01
#define RTS 0x02
#define OUT1 0x04
#define IRQ 0x08
#define LOOPBACK 0x10

void ser_init_poll(void) {
	outb(COM1 + IER, 0x00); /* Disable interrupts */
	outb(COM1 + LCR, DLAB); /* Enable DLAB */
	/* Set divisor to 38400 */
	outb(COM1 + DIVL, 3);
	outb(COM1 + DIVH, 0);
	/* Set 8N1 mode */
	outb(COM1 + LCR, DATA_8 | STOP_1 | PARITY_NONE);
	/* Disable + clear FIFOs */
	outb(COM1 + FCR, 0);
	/* Disable IRQs, set RTS / DTR */	
	outb(COM1 + MCR, DTR | RTS);
}

bool ser_init(const struct serial* port) {
	const uint16_t bauds[] = {
		65535, // B0
		2304, // B50
		1536, // B75
		1047, // B110
		859,  // B134
		768,  // B150
		576,  // B200
		384,  // B300
		192,  // B600
		96,   // B1200
		64,   // B1800
		48,   // B2400
		24,   // B4800
		12,   // B9600
		6,    // B19200
		3,    // B38400
		2,    // B57600
		1     // B115200
	};

	outb(port->port + IER, 0x00); /* Disable interrupts */
	outb(port->port + LCR, DLAB); /* Enable DLAB */
	/* Set divisor */
	outb(port->port + DIVL, bauds[port->speed] & 0xFF);
	outb(port->port + DIVH, bauds[port->speed] >> 8);
	/* Set 8N1 mode */
	outb(port->port + LCR, DATA_8 | STOP_1 | PARITY_NONE);
	/* Enable + clear FIFOs, with maximum threshold */
	outb(port->port + FCR, FIFO_14 | CLEAR_RX | CLEAR_TX | FIFO_EN);
	/* Enable IRQs, set RTS / DTR */
	outb(port->port + MCR, RTS | DTR | IRQ);
	/* Set loopback mode, test chip */
	outb(port->port + MCR, LOOPBACK | IRQ | OUT1 | DTR);
	/* Test serial chip */
	outb(port->port + DATA, 0xAE);
	
	if (inb(port->port + DATA) != 0xAE) {
		return false;
	}
	
	outb(port->port + MCR, DTR | RTS | OUT1 | IRQ);
	return true;
}

bool ser_ready(const struct serial* port) {
	return (inb(port->port + LSR) & 0x20) == 0x20;
}

void ser_write(const struct serial* port, char c) {
	while (!ser_ready(port)) {};

	outb(port->port + DATA, c);
}

bool ser_avail(const struct serial* port) {
	return inb(port->port + LSR) & 1;
}

char ser_read(const struct serial* port) {
	while (!ser_avail(port)) {};
	
	return inb(port->port + DATA);
}
