#include <x86/pit.h>
#include <x86/intrinsics.h>

#define PIT_DATA 0x40
#define PIT_CMD  0x43

#define PIT_LATCH 0
#define PIT_HILO  0x30

#ifdef __cplusplus
extern "C" {
#endif

void pit_arm( int mode, uint16_t period ) {
	uint32_t eflags = save_intr();

	outb( PIT_CMD, PIT_HILO | mode );
	outb( PIT_DATA, period & 0xFF );
	outb( PIT_DATA, ( period & 0xFF00 ) >> 8 );

	restore_intr( eflags );
}

uint16_t pit_read( void ) {
	uint32_t eflags = save_intr();
	uint16_t ret;

	outb( PIT_CMD, PIT_LATCH );
	ret = inb( PIT_DATA );
	ret |= inb( PIT_DATA ) << 8;

	restore_intr( eflags );

	return ret;
}

#ifdef __cplusplus
}
#endif
