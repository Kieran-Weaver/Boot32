#ifndef PIT_H
#define PIT_H

#include <stdbool.h>
#include <stdint.h>
#include <crt/header.h>

#define PIT_MODE_IRQ 0
#define PIT_MODE_ONESHOT 2

EXTERN_C_BEGIN

void pit_arm( int mode, uint16_t period );
uint16_t pit_read( void );

EXTERN_C_END

#endif
