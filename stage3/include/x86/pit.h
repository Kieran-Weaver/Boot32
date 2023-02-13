#ifndef PIT_H
#define PIT_H

#include <stdbool.h>
#include <stdint.h>

#define PIT_MODE_IRQ 0
#define PIT_MODE_ONESHOT 2

#ifdef __cplusplus
extern "C" {
#endif

void pit_arm( int mode, uint16_t period );
uint16_t pit_read( void );

#ifdef __cplusplus
}
#endif

#endif
