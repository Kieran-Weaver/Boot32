#ifndef SORT_H
#define SORT_H

#include <stdint.h>

/* Gapped insertion sort */
template<typename T>
void gap_sort(T* begin, uint32_t N, uint32_t gap) {
	for (uint32_t offset = 0; offset < gap; offset++) {
		for (uint32_t i = offset; i < N; i++) {
			uint32_t j;
			T tmp = begin[i];
			for (j = i; (j >= gap) && (tmp < begin[j - gap]); j -= gap) {
				begin[j] = begin[j - gap];
			}
			begin[j] = tmp;
		}
	}
}

/* Shell sort */
template<typename T>
void sort(T* begin, T* end) {
	/* Marcin Ciura's gap sequence, extended to 32 bits */
	constexpr uint32_t gaps[] = {3444003501, 1530668223, 680296988, 302354217, 134379652, 59724290, 26544129, 11797391, 5243285, 2330349, 1035711, 460316, 204585, 90927, 40412, 17961, 7983, 3548,
1577, 701, 301, 132, 57, 23, 10, 4, 1};
	uint16_t idx = 0;
	uint32_t size = end - begin;
	
	while (gaps[idx] > size) idx++;
	
	while (gaps[idx] != 1) {
		gap_sort(begin, size, gaps[idx++]);
	}

	gap_sort(begin, size, 1);
}

#endif
