/*
 * dict.h
 *
 *  Created on: Jul 21, 2019
 *      Author: md761
 */

#ifndef DICT_H_
#define DICT_H_

#include <stdint.h>
#include "arrays.h"

typedef struct symbols_chain {
	size_t prev_value_index;
	uint8_t value;
} symbols_chain;

typedef struct Dict Dict;

Dict* dict_create(size_t a_size);

void dict_destroy(Dict *a_dict);

bool dict_put(Dict *a_dict, const uint32_t a_key, const symbols_chain *a_value);

const size_t dict_get(Dict *a_dict, const uint32_t a_key);

void dict_reconstruct_string(Dict *a_dict, symbols_chain *a_index
		, uint8_t_array_t *a_out);

#ifndef NDEBUG
void dict_test(void);
#else
static inline void dict_test(void) {}
#endif

#endif /* DICT_H_ */
