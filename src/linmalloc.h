/*
 * linmalloc.h
 *
 *  Created on: Aug 6, 2019
 *      Author: Yevhen Kholomeniuk
 */

#ifndef SRC_LINMALLOC_H_
#define SRC_LINMALLOC_H_

#include <stddef.h>

typedef struct linalloc_t linalloc_t;

linalloc_t* linalloc_create(void);

void linalloc_free_all(linalloc_t *allocator);

__attribute__((malloc))
void* lincalloc(linalloc_t *allocator, size_t n, size_t size);

#endif /* SRC_LINMALLOC_H_ */
