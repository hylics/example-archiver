/*
 * buffered_io.h
 *
 *  Created on: Jul 28, 2019
 *      Author: md761
 */

#ifndef BUFFERED_IO_H_
#define BUFFERED_IO_H_

#include <stdint.h>
#include "arrays.h"

typedef struct bitstream_writer bitstream_writer_t;
typedef struct bitstream_reader bitstream_reader_t;

bitstream_writer_t* buffered_io_writer_create(const char *a_file);

void buffered_io_writer_close(bitstream_writer_t *a_buffer);

void buffered_io_writer_write_vcode(bitstream_writer_t *a_stream
		, const uint16_t a_code, const uint8_t a_bits);

bitstream_reader_t* io_reader_create(const char *a_file);

void io_reader_destroy(bitstream_reader_t *a_stream);

uint16_t io_reader_read_vcode(bitstream_reader_t *a_stream
		, uint8_t codelength);

uint8_t_array_t* io_reader_get_array(bitstream_reader_t *a_stream);

uint8_t_array_t* io_writer_get_array(bitstream_writer_t *a_stream);

#endif /* BUFFERED_IO_H_ */
