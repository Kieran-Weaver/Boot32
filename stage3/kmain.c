#include <stdint.h>
int strlen(const uint8_t* str){
	int i = 0;
	while (str[i] != '\0') {
		i++;
	}
	return 0;
}

void kprint(const uint8_t* in, uint8_t* screen, uint16_t n){
	for (uint16_t i = 0; i < n; i++){
		*(screen++) = in[i];
		*(screen++) = 0x0F;
	}
}

void kmain() {
	uint8_t* screen = (uint8_t*)0xB8000;
	const uint8_t* str1 = "Roses are #ff0000";
	const uint8_t* str2 = "Violets are #0000ff";
	const uint8_t* str3 = "All of my base";
	const uint8_t* str4 = "Are belong to you";
	kprint(str1, screen, strlen(str1));
	kprint(str2, screen + 160, strlen(str2));
	kprint(str3, screen + 320, strlen(str3));
	kprint(str4, screen + 480, strlen(str4));
}
