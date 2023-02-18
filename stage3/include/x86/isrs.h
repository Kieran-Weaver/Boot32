#ifndef ISRS_H
#define ISRS_H

#include <x86/gdt.h>
#include <crt/header.h>

EXTERN_C_BEGIN

__attribute__((interrupt)) void timer_isr(struct interrupt_frame* frame);
__attribute__((interrupt)) void com1_isr(struct interrupt_frame* frame);

EXTERN_C_END

#endif
