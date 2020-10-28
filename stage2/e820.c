#include "e820.h"
uint32_t max(uint32_t a, uint32_t b){
	return (a > b) ? a : b;
}
uint16_t compressE820(SMAP_entry_t* in, SMAP32_t* out, uint16_t n){
	uint16_t j = 0;
	for (uint16_t i = 0; i < n; i++){
		if (in[i].base < 0xFFFFFFFF){
			SMAP32_t temp;
			temp.base = in[i].base;
			if ((in[i].base + in[i].length) > 0xFFFFFFFF){
				temp.length = (0xFFFFFFFF - temp.base) + 1;
			} else {
				temp.length = in[i].length;
			}
			temp.type = in[i].type & 0xFFFF;
			temp.acpi = in[i].acpi & 0xFFFF;
			out[j] = temp;
			j++;
		}
	}
	return j;
}
uint16_t mergeE820(SMAP32_t* e820, uint16_t len){
	for (uint16_t i = 1; i < len; i++){
		if ((e820[i-1].base + e820[i-1].length) > e820[i].base){
			e820[i-1].length = max(e820[i-1].length, e820[i].base + e820[i].length);
			for (uint16_t j = (i+1); j < len; j++){
				e820[j-1] = e820[j];
			}
			len--;
		}
	}
}
uint8_t compE820(SMAP32_t* e820a, SMAP32_t* e820b){
	return (e820a->base < e820b->base) || (e820a->type < e820b->type);
}
void sortE820(SMAP32_t* e820, uint16_t len){
	uint16_t j = 0;
	for (uint16_t i = 1; i < len; i++){
		SMAP32_t current = e820[i];
		for (j = (i-1); (j >= 0) && compE820(&current, e820 + j); j--){
			e820[j + 1] = e820[j];
		}
		e820[j+1] = current;
	}
}
return_t validate(SMAP_entry_t* data, uint16_t n){
	return_t ret;
	ret.data = (SMAP32_t*) data;
	ret.n = compressE820(data, ret.data, n);
	sortE820(ret.data, ret.n);
	uint16_t typelengths[5] = {};
	for (uint16_t i = 0; i < ret.n; i++){
		typelengths[ret.data[i].type - 1]++;
	}
	uint16_t length = 0;
	for (uint8_t i = 0; i < 5; i++){
		mergeE820(ret.data + length, typelengths[i]);
		length += typelengths[i];
	}
	return ret;
}