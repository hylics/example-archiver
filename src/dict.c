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


TEMPLATE_DECLARE_ARRAY(symbols_chain)

struct Dict {
	size_t size;
	size_t len;
	uint32_t_array_t *keys;
	symbols_chain_array_t *values;
};


#define DUMP_ITEM(a_item) debug("%c %zu\n" \
			, a_item.value, a_item.prev_value_index)
TEMPLATE_IMPLEMENT_ARRAY(symbols_chain)
#undef DUMP_ITEM

static inline bool is_key_empty(const uint32_t a_key)
{
	return a_key == UINT32_MAX;
}

Dict* dict_create(size_t a_size)
{
	Dict *ret = calloc(1, sizeof(*ret));
	ret->size = a_size;
	ret->values = symbols_chain_array_create(a_size);
	ret->keys = uint32_t_array_create(a_size);

	/*UINT32_MAX will be empty key*/
	for (size_t i=0; i < a_size; ++i) {
		ret->keys->data[i] = UINT32_MAX;
	}

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

bool dict_put(Dict *a_dict, const uint32_t a_key, const symbols_chain *a_value)
{
	size_t index;

	if (unlikely(a_dict->len >= a_dict->size * 0.75)) {
		return false;
	}

	index = MurmurHash2A(&a_key, sizeof(uint32_t), 1) % a_dict->size;
	while (!is_key_empty(a_dict->keys->data[index])
			&& a_key != a_dict->keys->data[index]) {
		index=(index + 1) % a_dict->size;
	}
	a_dict->keys->data[index] = a_key;
	a_dict->values->data[index] = *a_value;
	++a_dict->len;

	return true;
}

const size_t dict_get(Dict *a_dict, const uint32_t a_key)
{
	size_t index = MurmurHash2A(&a_key, sizeof(uint32_t), 1) % a_dict->size;

	while (!is_key_empty(a_dict->keys->data[index])) {
		if (a_key == a_dict->keys->data[index]) {
			return index;
		}
		index=(index + 1) % a_dict->size;
	}

	return UINT32_MAX;
}


void dict_reconstruct_string(Dict *a_dict, symbols_chain *a_index
		, uint8_t_array_t *a_out)
{
	symbols_chain *iter = a_index;
	size_t index = 0;

	while (iter) {
		my_assert(index < a_out->size);
		a_out->data[index++] = iter->value;

		if (iter->prev_value_index != UINT32_MAX) {
			iter = &a_dict->values->data[iter->prev_value_index];
		} else {
			iter = NULL;
		}
	}

	a_out->len = index;

	/*Reverse string*/
	--index;
	const size_t middle = a_out->len / 2;
	for (size_t i=0; i < middle; ++i) {
		uint8_t tmp = a_out->data[i];
		a_out->data[i] = a_out->data[index - i];
		a_out->data[index - i] = tmp;
	}
}


#ifndef NDEBUG

static void test_put_get(void)
{
	Dict *test = dict_create(1000);
	symbols_chain chain;

	for (int i = 0; i < 750; ++i) {
		chain.value = i % 30;
		chain.prev_value_index = UINT32_MAX;
		assert(dict_put(test, i, &chain));
	}

	for (int i = 0; i < 750; ++i) {
		symbols_chain *val = (symbols_chain *)dict_get(test, i);
		assert(val);
		assert(val->value == i % 30);
	}

	dict_destroy(test);
}

static void reconstruct_string(void)
{
	Dict *test = dict_create(100);
	symbols_chain input, *output;
	uint8_t_array_t *string = uint8_t_array_create(10);
	uint32_t key = 0;

#define INSERT(a_value) do {\
		++key; \
		input.value = a_value; \
		input.prev_value_index = output->prev_value_index; \
		assert(dict_put(test, key, &input)); \
		assert((output=dict_get(test, key))); \
} while(0)

	input.value = 'a';
	input.prev_value_index = UINT32_MAX;
	assert(dict_put(test, key, &input));
	assert((output=dict_get(test, key)));

	INSERT('b');
	INSERT('c');
	INSERT('d');

	dict_reconstruct_string(test, output, string);

	uint8_t_array_dump(string);
	debug("str [%s]\n", (char *)string->data);

	dict_destroy(test);
}


void dict_test(void)
{
	test_put_get();
	reconstruct_string();
}
#endif
