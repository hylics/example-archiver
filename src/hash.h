/*
 * hash.h
 *
 *  Created on: Jul 21, 2019
 *      Author: md761
 */

#ifndef HASH_H_
#define HASH_H_

#include <stdlib.h>
#include <stdint.h>

uint32_t MurmurHash2A(const void *a_key, size_t a_len, unsigned int a_seed);

#endif /* HASH_H_ */
