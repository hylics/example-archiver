///*
// * dict.c
// *
// *  Created on: Jul 21, 2019
// *      Author: md761
// */
//
//#include "dict.h"
//#include "log.h"
//#include "hash.h"
//#include "utils.h"
//
//
//struct Dict {
//	size_t size;
//	size_t len;
//	octstr_array_t *keys;
//	dict_key_type_t type;
//	union {
//		uint32_t_array_t *keys_f; //fixed key length
//		octstr_array_t *keys_v; //variable key length
//	};
//	uintptr_t_array_t *values;
//};
//
//static inline bool octstr_cmp(const octstr *a, const octstr *b)
//{
//	return a->len == b->len && 0 == memcmp(a->data, b->data, a->len);
//}
//
//Dict* dict_create(size_t a_size, dict_key_type_t a_type)
//{
//	Dict *ret = calloc(1, sizeof(*ret));
//	ret->size = a_size;
//	ret->values = uintptr_t_array_create(a_size);
//	ret->type = a_type;
//
//	if (a_type == DICT_INT_KEY) {
//		ret->keys_f = uint32_t_array_create(a_size);
//	} else if (a_type == DICT_STR_KEY) {
//		ret->keys_v = octstr_array_create(a_size);
//	}
//
//	return ret;
//}
//
//void dict_destroy(Dict *a_dict)
//{
//	if (a_dict) {
//		if (a_dict->type == DICT_INT_KEY) {
//			uint32_t_array_destroy(a_dict->keys_f);
//		} else if (a_dict->type == DICT_STR_KEY) {
//			for (size_t i = 0; i < a_dict->keys_v->len; ++i) {
//				uint8_t_array_destroy(a_dict->keys_v->data[i]);
//			}
//			octstr_array_destroy(a_dict->keys_v);
//		}
//
//		uintptr_t_array_destroy(a_dict->values);
//		free(a_dict);
//	}
//}
//
//bool dict_put_v(Dict a_dict, const octstr *a_key, const void *a_value)
//{
//	bool ret = false;
//	size_t index = MurmurHash2A(a_key->data, a_key->len, 1) % a_dict->size;
//
//	while (a_dict->keys[index] && !octstr_cmp(a_key, a_dict->keys[index])) {
//		index=(index + 1) % a_dict->size;
//	}
//	a_dict->keys[index] = a_key;
//	a_dict->values[index] = a_value;
//
//	return ret;
//}
//
//bool dict_put_f(Dict a_dict, const uint32_t *a_key, const void *a_value)
//{
//	bool ret = false;
//	size_t index = MurmurHash2A(a_key, sizeof(uint32_t), 1) % a_dict->size;
//	while (a_dict->keys[index] && a_key != a_dict->keys[index]) {
//		index=(index + 1) % a_dict->size;
//	}
//	a_dict->keys[index] = a_key;
//	a_dict->values[index] = a_value;
//
//	return ret;
//}
//
//const void* dict_get_v(Dict a_dict, const octstr *a_key)
//{
//	size_t index = MurmurHash2A(a_key->data, a_key->len, 1) % a_dict->size;
//
//	while (a_dict->keys[index] && !octstr_cmp(a_key, a_dict->keys[index])) {
//		index=(index + 1) % a_dict->size;
//	}
//}
//
//const void* dict_get_f(Dict a_dict, const octstr *a_key)
//{
//
//}
