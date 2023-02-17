#include <x86/serial.h>
#include <x86/intrinsics.h>
#include <crt/util.h>

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

/* Interrupt Enable Register */
#define IER_RX  0x01
#define IER_TX  0x02
#define IER_ERR 0x04
#define IER_STS 0x08

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

#define COM1_ 0x3F8
#define COM2_ 0x2F8

static struct serial _COM1;
static struct serial _COM2;

struct serial* COM1;
struct serial* COM2;

void ser_subsystem_init(void) {
	COM1 = &_COM1;
	COM2 = &_COM2;
}

void ser_init_poll(void) {
	COM1 = &_COM1;
	COM2 = &_COM2;

	outb(SERIAL_COM1 + IER, 0x00); /* Disable interrupts */
	outb(SERIAL_COM1 + LCR, DLAB); /* Enable DLAB */
	/* Set divisor to 38400 */
	outb(SERIAL_COM1 + DIVL, 3);
	outb(SERIAL_COM1 + DIVH, 0);
	/* Set 8N1 mode */
	outb(SERIAL_COM1 + LCR, DATA_8 | STOP_1 | PARITY_NONE);
	/* Disable + clear FIFOs */
	outb(SERIAL_COM1 + FCR, 0);
	/* Disable IRQs, set RTS / DTR */	
	outb(SERIAL_COM1 + MCR, DTR | RTS);
}

bool ser_init(struct serial* port, int base, speed_t speed) {
	port->port = base;
	port->speed = speed;
	port->fifo_empty = 0;
	buf_init(&port->hdr);

	outb(port->port + IER, IER_TX); /* Enable TX interrupt */
	outb(port->port + LCR, DLAB);   /* Enable DLAB */
	/* Set divisor */
	outb(port->port + DIVL, port->speed & 0xFF);
	outb(port->port + DIVH, port->speed >> 8);
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

int  ser_ready(const struct serial* port) {
	return buf_elements(&port->hdr, SERIAL_BUFSIZE);
}

int ser_write(struct serial* port, const void* c, int n) {
	uint32_t buflen = buf_elements(&port->hdr, SERIAL_BUFSIZE);
	const uint8_t* data = (const uint8_t*)c;

	/* Critical section: append to serial, flush if necessary */
	uint32_t eflags = save_intr();

	int32_t written = SERIAL_BUFSIZE - buflen;
	if (n <= written) written = n;
	int32_t head = buf_write(&port->hdr, SERIAL_BUFSIZE, written);

	if ((head + written) >= SERIAL_BUFSIZE) {
		int32_t space_left = SERIAL_BUFSIZE - head;
		memcpy(port->buffer + head, data, space_left);
		memcpy(port->buffer, data + space_left, written - space_left);
	} else {
		memcpy(port->buffer + head, data, written);
	}

	buf_write_commit(&port->hdr, written);
	if (port->fifo_empty) ser_flush(port);

	restore_intr(eflags);

	return written;
}

void ser_flush(struct serial* port) {
	int32_t buflen = buf_elements(&port->hdr, SERIAL_BUFSIZE);
	
	if (buflen > 0) {
		if (buflen > SERIAL_FIFO) buflen = SERIAL_FIFO;
		int32_t tail = buf_read(&port->hdr, SERIAL_BUFSIZE, buflen);

		port->fifo_empty = 0;
		if ((tail + buflen) > SERIAL_BUFSIZE) {
			int32_t space_left = SERIAL_BUFSIZE - tail;
			rep_outsb(port->port + DATA, port->buffer + tail, space_left);
			rep_outsb(port->port + DATA, port->buffer, buflen - space_left);
		} else {
			rep_outsb(port->port + DATA, port->buffer + tail, buflen);
		}
		buf_read_commit(&port->hdr, buflen);
	} else {
		port->fifo_empty = 1;
	}
}

bool ser_avail(const struct serial* port) {
	return inb(port->port + LSR) & 1;
}

char ser_read(const struct serial* port) {
	while (!ser_avail(port)) {};
	
	return inb(port->port + DATA);
}
