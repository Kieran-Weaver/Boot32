#include <crt/assert.h>
#include <x86/intrinsics.h>
#include <x86/cpuid.h>
#include <x86/mtrr.h>

#define MTRR_NUM_FIXED 88
#define MTRRCAP 0xFE
#define MTRR_FIXED_64K 0x250
#define MTRR_FIXED_16K 0x258
#define MTRR_FIXED_4K  0x268
#define MTRR_BASE 0x200

EXTERN_C_BEGIN

static uint32_t mtrrcap;
static struct mtrr var_mtrrs[8];
static uint8_t  fix_mtrrs[MTRR_NUM_FIXED];

static void mtrr_set(int idx, uint64_t msr) {
	for (int i = 0; i < 8; i++) {
		fix_mtrrs[idx + i] = msr & 0xFF;
		msr = msr >> 8;
	}
}

void mtrr_init(void) {
	uint32_t mtrr_exists = cpuid() & CPUID_MTRR;
	uint64_t mtrr;
	if (mtrr_exists) {
		mtrrcap = (uint32_t)(rdmsr(MTRRCAP) & 0xFFFFFFFF);

		/* Fixed MTRRs */
		mtrr_set(0x00, rdmsr(MTRR_FIXED_64K));
		mtrr_set(0x08, rdmsr(MTRR_FIXED_16K + 0));
		mtrr_set(0x10, rdmsr(MTRR_FIXED_16K + 1));
		mtrr_set(0x18, rdmsr(MTRR_FIXED_4K + 0));
		mtrr_set(0x20, rdmsr(MTRR_FIXED_4K + 1));
		mtrr_set(0x28, rdmsr(MTRR_FIXED_4K + 2));
		mtrr_set(0x30, rdmsr(MTRR_FIXED_4K + 3));
		mtrr_set(0x38, rdmsr(MTRR_FIXED_4K + 4));
		mtrr_set(0x40, rdmsr(MTRR_FIXED_4K + 5));
		mtrr_set(0x48, rdmsr(MTRR_FIXED_4K + 6));
		mtrr_set(0x50, rdmsr(MTRR_FIXED_4K + 7));

		/* Variable MTRRs */
		for (int i = 0; i < mtrr_size(); i++) {
			/* MTRRphysBase */
			mtrr = rdmsr(MTRR_BASE + (2*i));
			var_mtrrs[i].base = (uint32_t)(mtrr & 0xFFFFF000);
			var_mtrrs[i].type = (uint8_t)(mtrr & 0xFF);

			/* MTRRphysMask */
			mtrr = rdmsr(MTRR_BASE + (2*i) + 1);
			var_mtrrs[i].mask = (uint32_t)(mtrr & 0xFFFFF000);
			var_mtrrs[i].enabled = (uint8_t)((mtrr >> 11) & 1);
		}
	} else {
		mtrrcap = 0;
		for (int i = 0; i < MTRR_NUM_FIXED; i++) {
			fix_mtrrs[i] = MTRR_NONE;
		}
	}
}

uint32_t mtrr_size(void) {
	return mtrrcap & 0xFF;
}

struct mtrr mtrr_read(uint32_t idx) {
	return var_mtrrs[idx];
}

uint8_t mtrr_fixed_type(uint8_t mtrr) {
	return fix_mtrrs[mtrr];
}

uint32_t mtrr_base(uint8_t mtrr) {
	if (mtrr < 8) {
		return 0x10000 * mtrr;
	} else if (mtrr < 24) {
		return 0x80000 + (mtrr - 8) * 0x4000;
	} else {
		return 0xC0000 + (mtrr - 24) * 0x1000;
	}
}

EXTERN_C_END
