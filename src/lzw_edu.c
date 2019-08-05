/*
 * lzw.c
 *
 *  Created on: Jul 20, 2019
 *      Author: md761
 */

#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "lzw_edu.h"
#include "log.h"
#include "arrays.h"
#include "tst.h"
#include "dict.h"
#include "utils.h"
#include "buffered_io.h"

/*
СТРОКА = очередной символ из входного потока
WHILE входной поток не пуст DO
	СИМВОЛ = очередной символ из входного потока
	IF СТРОКА+СИМВОЛ в таблице строк THEN
		СТРОКА = СТРОКА+СИМВОЛ
	ELSE
		вывести в выходной поток код для СТРОКА
		добавить в таблицу строк СТРОКА+СИМВОЛ
		СТРОКА = СИМВОЛ
	END of IF
END of WHILE
вывести в выходной поток код для СТРОКА


Процедура LZW-распаковки:

читать СТАРЫЙ_КОД
вывести СТАРЫЙ_КОД
СИМВОЛ = СТАРЫЙ_КОД
WHILE входной поток не пуст DO
	читать НОВЫЙ_КОД
	IF NOT в таблице перевода НОВЫЙ_КОД THEN
		СТРОКА = перевести СТАРЫЙ_КОД
		СТРОКА = СТРОКА+СИМВОЛ
	ELSE
		СТРОКА = перевести НОВЫЙ_КОД
	END of IF
	вывести СТРОКУ
	СИМВОЛ = первый символ СТРОКИ
	добавить в таблицу перевода СТАРЫЙ_КОД+СИМВОЛ
	СТАРЫЙ_КОД = НОВЫЙ_КОД
END of WHILE
 *
 */

#define CODELEN_MIN 9
#define CODELEN_MAX 12

enum reserved_codes {
	RESERVED_DICT_RESET=0,
	RESERVED_CODELENGTH_INC,
	RESERVED_COUNT
};

__attribute__((unused))
static void test(void);
static void _prefill_dict_r(TST *t, int left, int right);
static void prefill_dict(TST *t);



__attribute__((unused))
static void test(void)
{
	TST *t = tst_create();

	prefill_dict(t);
	tst_dump(t);

	tst_destroy(t);
}

static void _prefill_dict_r(TST *t, int left, int right)
{
	if ((right - left) < 1) {
		return;
	}
	int middle = (left + right)/2;
	TSTCursor cursor = tst_get_cursor(t);
	/* 0 is reserved as stop code */
	tst_insert_r(&cursor, middle, middle + RESERVED_COUNT);
	_prefill_dict_r(t, left, middle);
	_prefill_dict_r(t, middle+1, right);
}

static void prefill_dict(TST *t)
{
	_prefill_dict_r(t, 0, 255);
}

void lzw_compress(options_t *a_options)
{
	debug("%s %s\n", __func__, a_options->file_in);

	bitstream_writer_t *wstream = buffered_io_writer_create(a_options->file_out);
	uint8_t_array_t *input = read_file(a_options->file_in);
	size_t pos = 0;
	TST *dict = NULL;

reset_dict:
	dict = tst_create();
	prefill_dict(dict);
	int code_cnt = UINT8_MAX + RESERVED_COUNT;
	int codelength = CODELEN_MIN;
	size_t codemax = pow_simple(2, codelength) - 1;
	debug("codemax %lu\n", codemax);

	uint16_t code = 0;
	TSTCursor cursor = tst_get_cursor(dict);

	do {
		/* if dict contains this WORD+K which is 'cursor prefix + key'
		 * we set 'code' to corresponding value. In any case we update cursor */
		if (unlikely(false == tst_contains_r(&cursor, input->data[pos], &code))) {
			if (code_cnt < codemax) {
				/* Out code for WORD, then add 'WORD+K:next code' into dict */
				buffered_io_writer_write_vcode(wstream, code, codelength);
				++code_cnt;
				tst_insert_r(&cursor, input->data[pos], code_cnt);
			} else if (codelength < CODELEN_MAX) {
				buffered_io_writer_write_vcode(wstream, RESERVED_CODELENGTH_INC
						, codelength);
				codelength++;
				codemax = pow_simple(2, codelength) - 1;
				buffered_io_writer_write_vcode(wstream, code, codelength);
				++code_cnt;
				tst_insert_r(&cursor, input->data[pos], code_cnt);
				debug("codemax %lu\n", codemax);
			} else {
				debug("reset dict, pos %zu\n", pos);
				buffered_io_writer_write_vcode(wstream, code, codelength);
				buffered_io_writer_write_vcode(wstream, RESERVED_DICT_RESET
										, codelength);
				tst_destroy(dict);
				goto reset_dict;
			}
			/* set 'cursor' and 'code' to position of 'K' */
			cursor = tst_get_cursor(dict);
			tst_contains_r(&cursor, input->data[pos], &code);
		}
		++pos;
	} while (pos != input->len);
	buffered_io_writer_write_vcode(wstream, code, codelength);

	tst_destroy(dict);
	printf("input has size \t\t%zu\n", input->size);
	uint8_t_array_destroy(input);
	buffered_io_writer_close(wstream);
}

void lzw_decompress(options_t *a_options)
{
	debug("%s %s\n", __func__, a_options->file_in);

//	bitstream_reader_t *reader = bitstream_reader_create(
//			read_file(a_options->file_in));
//	uint8_t_array_t *string = uint8_t_array_create(1000);
//	Dict *dict = NULL;
//	uint8_t codelength = CODELEN_MIN;
//	uint32_t code_old = io_reader_read_vcode(reader, codelength);
//	debug("%u\n", code_old);
//	uint8_t sym = code_old;
//	_code code;
//	__dict_get(dict, code_old, &code);
//
//	while (io_reader_is_posible_read_next(reader, codelength)) {
//		uint32_t code_new = io_reader_read_vcode(reader, codelength);
//		if (__dict_get(dict, code_new, &code)) {
//			__dict_translate(dict, code, string);
//		} else {
//			__dict_translate(dict, code, string);
//			string->data[string->len] = sym;
//		}
//		debug("%s\n", string->data);
//		sym = string->data[0];
//
//		code_old = code_new;
//	}
//
//
//	io_reader_destroy(reader);


	/*
	 * читать СТАРЫЙ_КОД
вывести СТАРЫЙ_КОД
СИМВОЛ = СТАРЫЙ_КОД
WHILE входной поток не пуст DO
	читать НОВЫЙ_КОД
	IF NOT в таблице перевода НОВЫЙ_КОД THEN
		СТРОКА = перевести СТАРЫЙ_КОД
		СТРОКА = СТРОКА+СИМВОЛ
	ELSE
		СТРОКА = перевести НОВЫЙ_КОД
	END of IF
	вывести СТРОКУ
	СИМВОЛ = первый символ СТРОКИ
	добавить в таблицу перевода СТАРЫЙ_КОД+СИМВОЛ
	СТАРЫЙ_КОД = НОВЫЙ_КОД
END of WHILE*/
}

