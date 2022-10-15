// From musl/include/assert.h

#ifdef NDEBUG
#define	assert(x) (void)0
#else
#define assert(x) ((void)((x) || (__assert_fail(#x, __FILE__, __LINE__, __func__),0)))
#endif

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((noreturn)) void __assert_fail (const char *, const char *, int, const char *);
__attribute__((noreturn)) void panic(const char* fmt, ...);

#ifdef __cplusplus
}
#endif
