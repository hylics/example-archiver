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
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "lzw_edu.h"
#include "log.h"
#include "arrays.h"
#include "tst.h"
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

__attribute__((unused))
static void test(void);
static void prefill_dict(TST *t, int left, int right);



__attribute__((unused))
static void test(void)
{
	TST *t = tst_create();

	prefill_dict(t, 0, 255);
//	TSTCursor cursor = tst_get_cursor(t);
//
//	uint16_t sym = 0;
//	debug("1 tst contains '5'? %d %d\n", tst_contains_r(&cursor, 5, &sym), sym);
//	for (int i = 0; i < 40; ++i) {
//		cursor = tst_get_cursor(t);
//		tst_insert_r(&cursor, i, i*i);
//	}
//	cursor = tst_get_cursor(t);
//	debug("2 tst contains '5'? %d %d\n", tst_contains_r(&cursor, 5, &sym), sym);

	tst_dump(t);

	tst_destroy(t);
}

static void prefill_dict(TST *t, int left, int right)
{
	if ((right - left) < 1) {
		return;
	}
	int middle = (left + right)/2;
	TSTCursor cursor = tst_get_cursor(t);
	tst_insert_r(&cursor, middle, middle + 1);
	prefill_dict(t, left, middle);
	prefill_dict(t, middle+1, right);
}

void lzw_compress(options_t *a_options)
{
	struct stat sb;
	debug("%s %s\n", __func__, a_options->file_in);

	long n = 0;
	long left_bytes = 0;
	int in_fd = open(a_options->file_in, O_RDONLY);
	assert(in_fd >= 0);
#ifdef __APPLE__
	fcntl(in_fd, F_RDAHEAD, 1);
#endif
	fstat(in_fd, &sb);

	left_bytes = sb.st_size;
	uint8_t_array_t *input = uint8_t_array_create(left_bytes);
	debug("file size %lld, IO block size %d\n"
			, sb.st_size, sb.st_blksize);

	while (left_bytes) {
		n = read(in_fd, input->data + n, left_bytes);
		assert(n > 0 && "fread failed");
		left_bytes -= n;
	}
	input->len = sb.st_size;

	bitstream_writer_t *wstream = buffered_io_writer_create(a_options->file_out);

	size_t pos = 0;
	int codelength = 16;
	size_t codemax = pow_simple(2, codelength);
	debug("codemax %lu\n", codemax);

	TST *dict = NULL;
reset_dict:
	dict = tst_create();
	prefill_dict(dict, 0, 255);
	int code_cnt = 257;
	uint16_t code = 0;
	TSTCursor cursor = tst_get_cursor(dict);

	do {
		/* if dict contains this WORD+K which is 'cursor prefix + key'
		 * we set 'code' to corresponding value. In any case we update cursor */
		if (false == tst_contains_r(&cursor, input->data[pos], &code)) {
			if (code_cnt < codemax) {
				/* Out code for WORD, then add 'WORD+K:next code' into dict */
//				debug("code %d\n", code);
				buffered_io_writer_write_vcode(wstream, code, codelength);
				++code_cnt;
				tst_insert_r(&cursor, input->data[pos], code_cnt);
			} else {
				debug("reset dict, pos %zu\n", pos);
				buffered_io_writer_write_vcode(wstream, code, codelength);
				tst_destroy(dict);
				goto reset_dict;
//				exit(1);
			}
			/* set 'cursor' and 'code' to position of 'K' */
			cursor = tst_get_cursor(dict);
			tst_contains_r(&cursor, input->data[pos], &code);
		}
		++pos;
	} while (pos != input->len);
	buffered_io_writer_write_vcode(wstream, code, codelength);
//	debug("code %d\n", code);

	tst_destroy(dict);
	uint8_t_array_destroy(input);
	close(in_fd);
	debug("\nInput file has size \t%lld\n", sb.st_size);
	buffered_io_writer_close(wstream);
}

void lzw_decompress(options_t *a_options)
{
	debug("%s %s\n", __func__, a_options->file_in);
}

