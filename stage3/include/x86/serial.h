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
	B0 = 0,
	B50,
	B75,
	B110,
	B134,
	B150,
	B200,
	B300,
	B600,
	B1200,
	B1800,
	B2400,
	B4800,
	B9600,
	B19200,
	B38400,
	B57600,
	B76800,
	B115200
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
