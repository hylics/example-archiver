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

typedef enum {
	DICT_INT_KEY=0,
	DICT_STR_KEY
} dict_key_type_t;

typedef struct Dict Dict;

#endif /* DICT_H_ */
