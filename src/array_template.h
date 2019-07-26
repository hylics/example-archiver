/*
 * array_template.h
 *
 *  Created on: Jul 21, 2019
 *      Author: md761
 */

#ifndef ARRAY_TEMPLATE_H_
#define ARRAY_TEMPLATE_H_

#define TEMPLATE_DECLARE_ARRAY(a_type)									\
typedef struct {														\
	size_t size; /* array capacity */									\
	size_t len;	/* used length */										\
	a_type *data;														\
} a_type ## _array_t;													\
																		\
__attribute__((warn_unused_result))										\
a_type ## _array_t* a_type ## _array_create(size_t a_size); 			\
																		\
void  a_type ## _array_destroy(a_type ## _array_t *a_arr); 				\
																		\
__attribute__((unused))													\
void  a_type ## _array_dump(a_type ## _array_t *a_arr); 				\


#define TEMPLATE_IMPLEMENT_ARRAY(a_type)								\
a_type ## _array_t* a_type ## _array_create(size_t a_size) { 			\
	a_type ## _array_t *ret = calloc(1, sizeof(*ret));					\
	ret->data = calloc(a_size, sizeof(a_type));							\
	ret->size = a_size;													\
	ret->len = 0;														\
	return ret;															\
}																		\
																		\
void  a_type ## _array_destroy(a_type ## _array_t *a_arr) {				\
	free(a_arr->data);													\
	free(a_arr);														\
}																		\
																		\
void  a_type ## _array_dump(a_type ## _array_t *a_arr) {				\
		debug("%s [%p] : ", __func__, a_arr);							\
		for (size_t i = 0; i < a_arr->len; ++i) {						\
			DUMP_ITEM(a_arr->data[i]);									\
		}																\
		debug("\n");													\
}

#endif /* ARRAY_TEMPLATE_H_ */
