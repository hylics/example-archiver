/*
 * hash.c
 *
 *  Created on: Jul 21, 2019
 *      Author: md761
 */

#include "hash.h"

#define mmix(h,k) { k *= m; k ^= k >> r; k *= m; h *= m; h ^= k; }

uint32_t MurmurHash2A(const void *a_key, size_t a_len, unsigned int a_seed)
{
	const unsigned int m = 0x5bd1e995;
	const int r = 24;
	unsigned int l = a_len;

	const unsigned char * data = (const unsigned char *)a_key;

	unsigned int h = a_seed;
	unsigned int k;

	while(a_len >= 4)
	{
		k = *(unsigned int*)data;

		mmix(h,k);

		data += 4;
		a_len -= 4;
	}

	unsigned int t = 0;

	switch(a_len)
	{
	case 3:
		t ^= data[2] << 16;
	case 2:
		t ^= data[1] << 8;
	case 1:
		t ^= data[0];
	};

	mmix(h,t);
	mmix(h,l);

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}

