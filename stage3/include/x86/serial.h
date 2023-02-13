#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define COM1 0x3F8
#define COM2 0x2F8

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
};

/* COM1, 38400 baud, no interrupts or FIFO */
void ser_init_poll(void);
bool ser_init(const struct serial* port);
/* Write functions */
bool ser_ready(const struct serial* port);
void ser_write(const struct serial* port, char c);
/* Read functions */
bool ser_avail(const struct serial* port);
char ser_read(const struct serial* port);

#ifdef __cplusplus
}
#endif

#endif
