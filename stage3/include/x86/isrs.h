#ifndef ISRS_H
#define ISRS_H

#include <x86/gdt.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((interrupt)) void timer_isr(struct interrupt_frame* frame);

#ifdef __cplusplus
}
#endif

#endif
