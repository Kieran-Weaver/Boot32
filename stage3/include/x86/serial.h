#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <stdbool.h>
#include <mem/ringbuf.h>
#include <crt/header.h>

EXTERN_C_BEGIN

extern struct serial* COM1;
extern struct serial* COM2;

#define SERIAL_BUFSIZE 64
#define SERIAL_FIFO 16
#define SERIAL_COM1 0x3F8
#define SERIAL_COM2 0x2F8

typedef enum {
	B0 = 65535,
	B50 = 2304,
	B75 = 1536,
	B110 = 1047,
	B134 = 859,
	B150 = 768,
	B200 = 576,
	B300 = 384,
	B600 = 192,
	B1200 = 96,
	B1800 = 64,
	B2400 = 48,
	B4800 = 24,
	B9600 = 12,
	B19200 = 6,
	B38400 = 3,
	B57600 = 2,
	B76800 = 1,
	B115200 = 0
} speed_t;

typedef enum {
	VINTR = 0,
	VQUIT,
	VERASE,
	VKILL,
	VEOF,
	VEOL,
	VEOL2,
	VMIN,
	VTIME,
	NCCS
} cc_t;

struct serial {
	uint16_t port;
	uint8_t  speed;
	uint8_t  fifo_empty;
	bufhdr_t hdr;
	uint8_t  buffer[SERIAL_BUFSIZE];
};

/* COM1, 38400 baud, no interrupts or FIFO */
void ser_init_poll(void);
void ser_subsystem_init(void);
bool ser_init(struct serial* port, int base, speed_t speed);
/* Write functions */
int  ser_ready(const struct serial* port);
int  ser_write(struct serial* port, const void* c, int n);
/* Read functions */
bool ser_avail(const struct serial* port);
char ser_read(const struct serial* port);
/* Internals */
void ser_flush(struct serial* port);

EXTERN_C_END

#endif
