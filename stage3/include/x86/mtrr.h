#ifndef MTRR_H
#define MTRR_H

#include <crt/header.h>
#include <stdint.h>

#define MTRR_NONE 0xFF
#define MTRR_UC 0
#define MTRR_WC 1
#define MTRR_WT 4
#define MTRR_WP 5
#define MTRR_WB 6

EXTERN_C_BEGIN

struct mtrr {
	uint32_t base;
	uint32_t mask;
	uint8_t  type;
	uint8_t  enabled;
};

void        mtrr_init(void);
/* Variable MTRRs */
uint32_t    mtrr_size(void);
struct mtrr mtrr_read(uint32_t idx);
/* Fixed MTRRs */
uint8_t     mtrr_fixed_type(uint8_t mtrr);
uint32_t    mtrr_fixed_base(uint8_t mtrr);
uint32_t    mtrr_fixed_size(uint8_t mtrr);

EXTERN_C_END

#endif
