/*
 * tst.c
 *
 *  Created on: Jul 23, 2019
 *      Author: md761
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tst.h"
#include "utils.h"
#include "linmalloc.h"

#define USE_LINMALLOC 0

#if USE_LINMALLOC == 1
#define M_CALLOC(a_n, a_size) lincalloc(_s_allocator, a_n, a_size)
#else
#define	M_CALLOC(a_n, a_size) calloc(a_n, a_size)
#endif

struct tst_node {
    uint8_t key;                /* value to split on */
    uint16_t value;
    struct tst_node *lt;        /* go here if target[index] < value */
    struct tst_node *eq;        /* go here if target[index] == value */
    struct tst_node *gt;        /* go here if target[index] > value */
};

struct ternary_search_tree {
	struct tst_node *root;
	size_t prefix_count;
};

static linalloc_t *_s_allocator = NULL;

static void destroy_node_recursive(struct tst_node *node)
{
	if (node) {
		destroy_node_recursive(node->lt);
		destroy_node_recursive(node->eq);
		destroy_node_recursive(node->gt);
		free(node);
	}
}

TST* tst_create(void)
{
	if (USE_LINMALLOC) {
		_s_allocator = linalloc_create();
	}

	TST *tst = M_CALLOC(1, sizeof(*tst));
	tst->prefix_count = 0;
	my_assert(tst);
	return tst;
}

/* free a TST */
void tst_destroy(TST *t)
{
	my_assert(t);
	if (USE_LINMALLOC) {
		linalloc_free_all(_s_allocator);
		_s_allocator = NULL;
	} else {
		destroy_node_recursive(t->root);
		free(t);
	}
}

TSTCursor tst_get_cursor(const TST *t)
{
	return (TSTCursor){
		.finger = (struct tst_node **)&t->root,
		.parent = (TST *)t};
}

uint16_t tst_cursor_get_value(TSTCursor *cursor)
{
	my_assert(*cursor->finger);
	return (*cursor->finger)->value;
}

size_t tst_get_size(const TST *t)
{
	return t->prefix_count;
}

bool tst_contains_r(TSTCursor *cursor, const uint8_t key, uint16_t *value_out)
{
	struct tst_node *p = *cursor->finger;
	cursor->parent->prefix_count++;

	while (p) {
		if (key < p->key) {
			cursor->finger = &p->lt;
			p = p->lt;
		} else if (key > p->key) {
			cursor->finger = &p->gt;
			p = p->gt;
		} else {
			cursor->finger = &p->eq;
			*value_out = p->value;
			return true;
		}
	}
	return false;
}

void tst_insert_r(TSTCursor *cursor, const uint8_t key, const uint16_t value)
{
	struct tst_node **pp = cursor->finger;
	struct tst_node *p = NULL;
	uint8_t node_key = 0;

	while (1) {
		if (unlikely(*pp == NULL)) {
			*pp = p = M_CALLOC(1, sizeof(*p));
			my_assert(p);
			p->key = key;
			p->value = value;
			if (USE_LINMALLOC) {
				p->eq = NULL;
				p->gt = NULL;
				p->lt = NULL;
			}
			return;
		}

		node_key = (*pp)->key;
		if (key < node_key) {
			pp = &(*pp)->lt;
		} else if (key > node_key) {
			pp = &(*pp)->gt;
		} else {
			pp = &(*pp)->eq;
		}
	}
}

void node_dump(struct tst_node *node) {
	if (node) {
		printf("node [%p] key %u value %u\n"
				, node, node->key, node->value);
		node_dump(node->lt);
		node_dump(node->eq);
		node_dump(node->gt);
	}
}

void tst_dump(TST *t)
{
	if (t) {
		node_dump(t->root);
	}
}
