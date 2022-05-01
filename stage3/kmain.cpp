#include <stdint.h>
#include <x86/e820.h>
#include <crt/util.h>
#include <mem/pagetable.h>
#include <mem/pmm.h>
#include <mem/vaddr.h>
#include <mem/kmalloc.h>
#include <crt/assert.h>
#include <x86/serial.h>
#include <x86/gdt.h>

static struct serial port;

void printscreen(const uint8_t* in, volatile uint8_t* screen, uint16_t n){
	for (uint16_t i = 0; i < n; i++){
		*(screen++) = in[i];
		*(screen++) = 0x0F;
	}
}

void kprint(const char* str, uint16_t n) {
	for (uint16_t i = 0; i < n; i++) {
		ser_write(&port, str[i]);
	}
}

void hextostr(uint32_t in, char* out){
	const char *bytes = "0123456789ABCDEF";
	for (uint8_t i = 0; i < 8; i++){
		out[7-i] = bytes[in & 0x0F];
		in >>= 4;
	}
}

void clear(volatile uint8_t* screen) {
	for (int i = 0; i < (160 * 25); i++) {
		screen[i] = 0;
	}
}

static volatile vaddr_t vaddrs[8192];
static struct TSS tss;

extern "C" void kmain(SMAP32_t* e820, size_t e820_size) {
	volatile uint8_t* screen = (uint8_t*)0xB8000;
	uint32_t temp_page = 0;
	uint32_t free_pages;
	const uint8_t* sErr = reinterpret_cast<const uint8_t*>("Serial port init failed!");
	const char* header = " Base       | Length     | Type\r\n";
	char str[] = " 0x00000000 | 0x0009FC00 | 1\r\n";
	char freestr[] = "Free pages: 0x00000000\r\n";
	char errorstr[] = "Error: invalid page 0x00000000\r\n";
	
	gdt_set_tss(&tss);
	
	PageTable pt((vaddr_t)0xFFFFF000);
	pt.activate();
	pmm_init(e820, e820_size);

	clear(screen);

	port.port = COM1;
	port.speed = B115200;
	
	if (!ser_init(&port)) {
		printscreen(sErr, screen, strlen((const char*)sErr));
		assert(0);
	}

	kprint(header, strlen(header));
	
	for (uint16_t i = 0; i < e820_size; i++){
		hextostr(e820[i].base, str + 3);
		hextostr(e820[i].length, str + 16);
		str[27] = '0' + e820[i].type;
		kprint(str, strlen(str));
	}
	
	vinit();
	free_pages = pmm_free_pages();
	
	for (uint32_t i = 0; i < 8192; i++) {
		vaddrs[i] = valloc();
	}
	
	for (uint32_t i = 0; i < 8192; i++) {
		vfree(vaddrs[i]);
	}

	assert(free_pages == pmm_free_pages());
//	free_pages = pmm_free_pages();
	hextostr(free_pages, freestr + 14);
	kprint(freestr, strlen(freestr));
	temp_page = pmm_alloc_ppage();
	for (uint32_t i = 0; i < free_pages; i++) {
		if ((temp_page & 1023) != 0) {
			hextostr(temp_page, errorstr + 22);
			kprint(errorstr, strlen(errorstr));
		}
		temp_page = pmm_alloc_ppage();
		pmm_free_ppage(temp_page);
	}

	free_pages = pmm_free_pages();
	hextostr(free_pages, freestr + 14);
	kprint(freestr, strlen(freestr));
	for (uint32_t i = 0; i < free_pages; i++) {
		temp_page = pmm_alloc_ppage();
		if ((temp_page & 1023) != 0) {
			hextostr(temp_page, errorstr + 22);
			kprint(errorstr, strlen(errorstr));
		}
	}

	free_pages = pmm_free_pages();
	hextostr(free_pages, freestr + 14);
	kprint(freestr, strlen(freestr));
	
	assert(0);
}
