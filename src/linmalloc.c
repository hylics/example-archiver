/*
 * linmalloc.c
 *
 *  Created on: Aug 6, 2019
 *      Author: Yevhen Kholomeniuk
 */

#include <stdlib.h>
#include "linmalloc.h"
#include "utils.h"

#define DEFAULT_BUFFER_SIZE (4 * 1024 * 1024)
#define MEMALIGN sizeof(size_t)

typedef struct linalloc_node {
	void *mem;
	size_t size;
	size_t offset;
	struct linalloc_node *next;
} linalloc_node_t;

struct linalloc_t {
	struct linalloc_node *head;
};

static linalloc_node_t* linalloc_node_create(size_t size)
{
	linalloc_node_t *ret = calloc(1, sizeof(*ret));
	ret->mem = malloc(size);
	ret->size = size;
	return ret;
}

linalloc_t* linalloc_create(void)
{
	linalloc_t *ret = calloc(1, sizeof(*ret));
	ret->head = linalloc_node_create(DEFAULT_BUFFER_SIZE);

	return ret;
}

void linalloc_free_all(linalloc_t *allocator)
{
	linalloc_node_t *iter, *del;
	iter = allocator->head;

	while (iter) {
		del = iter;
		iter = iter->next;
		free(del->mem);
		free(del);
	}

	free(allocator);
}

void* lincalloc(linalloc_t *allocator, size_t n, size_t size)
{
	void *ret;
	size_t desired_size = (n * size);
	desired_size = (desired_size + MEMALIGN-1) & ~(MEMALIGN-1);

	if (unlikely((allocator->head->size - allocator->head->offset)
			>= desired_size)) {
		linalloc_node_t *new = linalloc_node_create(DEFAULT_BUFFER_SIZE);
		new->next = allocator->head;
		allocator->head = new;
	}

	ret = allocator->head->mem + allocator->head->offset;
	allocator->head->offset += desired_size;
	return ret;
}
