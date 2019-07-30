/*
 * arrays.h
 *
 *  Created on: Jul 21, 2019
 *      Author: md761
 */

#ifndef ARRAYS_H_
#define ARRAYS_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "array_template.h"

TEMPLATE_DECLARE_ARRAY(uint32_t)

TEMPLATE_DECLARE_ARRAY(uint8_t)

typedef uint8_t_array_t octstr;

//TEMPLATE_DECLARE_ARRAY(octstr)

TEMPLATE_DECLARE_ARRAY(uintptr_t)

#endif /* ARRAYS_H_ */
