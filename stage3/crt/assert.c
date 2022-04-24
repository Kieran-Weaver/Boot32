#ifdef __cplusplus
extern "C" {
#endif

void __assert_fail (const char * x, const char * file, int line, const char * func) {
	while (1)
		asm("cli; hlt");
}

#ifdef __cplusplus
}
#endif
