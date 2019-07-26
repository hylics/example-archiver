/*
 * lzw.c
 *
 *  Created on: Jul 20, 2019
 *      Author: md761
 */

#include "lzw_edu.h"
#include "log.h"
#include "tst.h"

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

void lzw_compress(options_t *a_options)
{
	debug("%s %s\n", __func__, a_options->file_in);

	TST *t = tst_create();
	TSTCursor cursor = tst_get_cursor(t);

	uint16_t sym = 0;
	debug("1 tst contains '5'? %d %d\n", tst_contains_r(&cursor, 5, &sym), sym);
	for (int i = 0; i < 40; ++i) {
		cursor = tst_get_cursor(t);
		tst_insert_r(&cursor, i, i*i);
	}
	cursor = tst_get_cursor(t);
	debug("2 tst contains '5'? %d %d\n", tst_contains_r(&cursor, 5, &sym), sym);

//	tst_dump(t);

	tst_destroy(t);
}

void lzw_decompress(options_t *a_options)
{
	debug("%s %s\n", __func__, a_options->file_in);
}

