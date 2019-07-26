/*
 * tst.h
 *
 *  Created on: Jul 23, 2019
 *      Author: md761
 */

#ifndef TST_H_
#define TST_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct ternary_search_tree TST;

typedef struct tst_cursor {
	struct tst_node **finger;
	struct ternary_search_tree *parent;
} TSTCursor;

TST* tst_create(void);

/* free a TST */
void tst_destroy(TST *t);

void tst_dump(TST *t);

TSTCursor tst_get_cursor(const TST *t);

uint16_t tst_cursor_get_value(TSTCursor *cursor);

size_t tst_get_size(const TST *t);

bool tst_contains_r(TSTCursor *cursor, const uint8_t key, uint16_t *value_out);

void tst_insert_r(TSTCursor *cursor, const uint8_t key, const uint16_t value);

#endif /* TST_H_ */
