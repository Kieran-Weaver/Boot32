#include <crt/header.h>
#include <crt/atomic.h>
#include <x86/cpuid.h>
#include <x86/intrinsics.h>

#define CPUID_FEATURES 1

EXTERN_C_BEGIN

static uint32_t _cpuid;
static const char cpuid_flags[] = "FPU VME DE  PSE TSC MSR PAE MCE CX8 APICRD1 SEP MTRRPGE MCA CMOVPAT PSE PSN CLFHRD2 DS  ACPIMMX FXSRSSE SSE2SS  HTT TM  IA64PBE ";

void cpuid_init(void) {
	_cpuid = get_cpuid();
}

uint32_t cpuid(void) {
	return _cpuid;
}

const char* cpuid_str(int flag) {
	return cpuid_flags + (4 * flag);
}

EXTERN_C_END
