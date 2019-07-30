/*
 * buffered_io.h
 *
 *  Created on: Jul 28, 2019
 *      Author: md761
 */

#ifndef BUFFERED_IO_H_
#define BUFFERED_IO_H_

typedef struct bitstream_writer bitstream_writer_t;

bitstream_writer_t* buffered_io_writer_create(const char *a_file);

void buffered_io_writer_close(bitstream_writer_t *a_buffer);

void buffered_io_writer_write_vcode(bitstream_writer_t *a_stream
		, const uint16_t a_code, const uint8_t a_bits);

#endif /* BUFFERED_IO_H_ */
