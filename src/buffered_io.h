/*
 * buffered_io.h
 *
 *  Created on: Jul 28, 2019
 *      Author: md761
 */

#ifndef BUFFERED_IO_H_
#define BUFFERED_IO_H_

#include <stdint.h>
#include <stdbool.h>
#include "arrays.h"

typedef struct bitstream_writer bitstream_writer_t;
typedef struct bitstream_reader bitstream_reader_t;

bitstream_writer_t* buffered_io_writer_create(const char *a_file);

void buffered_io_writer_close(bitstream_writer_t *a_buffer);

void buffered_io_writer_write_vcode(bitstream_writer_t *a_stream
		, const uint32_t a_code, const uint8_t a_bits);

uint8_t_array_t* read_file(const char *a_file);

bitstream_reader_t* bitstream_reader_create(uint8_t_array_t *a_vector);

void io_reader_destroy(bitstream_reader_t *a_stream);

bool io_reader_is_posible_read_next(bitstream_reader_t *a_stream
		, uint8_t a_codelength);

uint32_t io_reader_read_vcode(bitstream_reader_t *a_stream
		, uint8_t a_bits);

#ifndef NDEBUG
void bitstreams_test(const char *a_file);
#else
static inline void bitstreams_test(const char *a_file) {}
#endif

#endif /* BUFFERED_IO_H_ */
