/*
 * dict.c
 *
 *  Created on: Jul 21, 2019
 *      Author: md761
 */

#include "dict.h"
#include "log.h"
#include "hash.h"
#include "utils.h"

typedef struct symbols_chain {
	size_t prev_value_index;
	uint8_t value;
} symbols_chain;

TEMPLATE_DECLARE_ARRAY(symbols_chain)

struct Dict {
	size_t size;
	size_t len;
	uint32_t_array_t *keys;
	symbols_chain_array_t *values;
};

static inline bool octstr_cmp(const octstr *a, const octstr *b)
{
	return a->len == b->len && 0 == memcmp(a->data, b->data, a->len);
}

#define DUMP_ITEM(a_item) debug("%c %zu\n" \
			, a_item->value, a_item->prev_value_index)
TEMPLATE_IMPLEMENT_ARRAY(symbols_chain)
#undef DUMP_ITEM

Dict* dict_create(size_t a_size)
{
	Dict *ret = calloc(1, sizeof(*ret));
	ret->size = a_size;
	ret->values = symbols_chain_array_create(a_size);
	ret->keys = uint32_t_array_create(a_size);

	return ret;
}

void dict_destroy(Dict *a_dict)
{
	if (a_dict) {
		uint32_t_array_destroy(a_dict->keys);
		symbols_chain_array_destroy(a_dict->values);
		free(a_dict);
	}
}


bool dict_put_f(Dict a_dict, const uint32_t a_key, const void *a_value)
{
	bool ret = false;
	size_t index = MurmurHash2A(a_key, sizeof(uint32_t), 1) % a_dict->size;

	while (a_dict->keys[index] && a_key != a_dict->keys[index]) {
		index=(index + 1) % a_dict->size;
	}
	a_dict->keys[index] = a_key;
	a_dict->values[index] = a_value;

	return ret;
}



const void* dict_get_f(Dict a_dict, const octstr *a_key)
{

}
