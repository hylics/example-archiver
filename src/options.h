/*
 * options.h
 *
 *  Created on: Jul 20, 2019
 *      Author: md761
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

typedef enum action {
	UNDEFINED_ACTION = -1 ,
	COMPRESS,
	DECOMPRESS,
} action_t;

typedef enum alg {
	UNDEFINED_ALGORITHM = -1,
	LZW,
	LZMA,
} alg_t;

typedef struct _options_t {
	action_t action;
	alg_t algorithm;
	char *file_in;
	char *file_out;
} options_t;

#endif /* OPTIONS_H_ */
