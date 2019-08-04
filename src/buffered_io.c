/*
 * buffered_io.c
 *
 *  Created on: Jul 28, 2019
 *      Author: md761
 */

#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <netinet/in.h>

#include "buffered_io.h"
#include "utils.h"
#include "log.h"

typedef union {
	uint16_t code;
	uint32_t holder;
	struct __attribute__((packed)) {
		uint8_t b1;
		uint8_t b2;
		uint8_t b3;
		uint8_t b4;
	} bytes;
} code_t;

struct bitstream_writer {
	uint8_t_array_t *buffer;
	int fd;
	uint8_t currBitIndex;
	size_t written_len;
};

struct bitstream_reader {
	uint8_t_array_t *buffer;
	uint8_t currBitIndex;
	uint8_t currByteIndex;
};

static bool write_buffer2file(bitstream_writer_t *a_buffer);


uint8_t_array_t* io_reader_get_array(bitstream_reader_t *a_stream) {
	return a_stream->buffer;
}

uint8_t_array_t* io_writer_get_array(bitstream_writer_t *a_stream) {
	return a_stream->buffer;
}

bitstream_writer_t* buffered_io_writer_create(const char *a_file)
{
	struct stat sb;
	bitstream_writer_t *ret = calloc(1, sizeof(*ret));
	ret->fd = open(a_file, O_CREAT|O_WRONLY|O_EXCL|O_APPEND/*|O_NONBLOCK*/
			, 0666);
	if (ret->fd < 0) {
		printf("[%s]: %s\n", a_file, strerror(errno));
		assert(ret->fd >= 0);
	}
	fstat(ret->fd, &sb);
	ret->buffer = uint8_t_array_create(sb.st_blksize * 128);
	ret->buffer->len = 0;

	return ret;
}

void buffered_io_writer_close(bitstream_writer_t *a_buffer)
{
	if (a_buffer) {
		write_buffer2file(a_buffer);
		fsync(a_buffer->fd);
		close(a_buffer->fd);
		printf("Output file has size \t%lu\n", a_buffer->written_len);
		uint8_t_array_destroy(a_buffer->buffer);
		free(a_buffer);
	}
}

static bool write_buffer2file(bitstream_writer_t *a_buffer)
{
	size_t len = a_buffer->buffer->len;
	void *ptr = a_buffer->buffer->data;
	int ret;

	if (len == 0) {
		printf("buffer is empty\n");
		return true;
	}

	while(len > 0) {
		ret = write(a_buffer->fd, ptr, len);
		if (unlikely(ret == -1)) {
			if (errno == EAGAIN) continue;
			if (errno == EINTR) continue;
			printf("Writing to file failed: %d\n", errno);
			return false;
		}
		len -= ret;
		ptr += ret;
	}
	a_buffer->written_len += a_buffer->buffer->len;
	debug("dump to file %zu bytes\n", a_buffer->buffer->len);
	return true;
}

void buffered_io_writer_write_vcode(bitstream_writer_t *a_stream
		, const uint32_t a_code, const uint8_t a_bits)
{
	uint8_t bits = a_bits;
	uint32_t code = a_code;

	while (bits > 0) {
		if (a_stream->currBitIndex == 0) {
			a_stream->buffer->data[++a_stream->buffer->len - 1] =
					(uint8_t)code;
			if (bits < 8) {
				a_stream->currBitIndex += bits;
				bits = 0;
			} else {
				bits -= 8;
				code >>= 8;
			}

			/* try dump buffer to file */
			if (unlikely(a_stream->buffer->len == a_stream->buffer->size)) {
				if (unlikely(false == write_buffer2file(a_stream))) {
					exit(EXIT_FAILURE);
				}
				a_stream->buffer->len =
						(a_stream->currBitIndex == 0) ? 0 : 1;
			}
		} else {
			const uint8_t b = (uint8_t)(code << a_stream->currBitIndex);
			a_stream->buffer->data[a_stream->buffer->len - 1] |= b;
			const uint32_t bitsWritten = 8 - a_stream->currBitIndex;

			if (bits < bitsWritten) {
				a_stream->currBitIndex += bits;
				bits = 0;
			} else {
				bits -= bitsWritten;
				code >>= bitsWritten;
				a_stream->currBitIndex = 0;
			}
		}
	}
}

uint8_t_array_t* read_file(const char *a_file)
{
	int fd = open(a_file, O_RDONLY);
	if (fd < 0) {
		printf("[%s]: %s\n", a_file, strerror(errno));
		assert(fd >= 0);
	}
#ifdef __APPLE__
	fcntl(fd, F_RDAHEAD, 1);
#elif __linux__

#endif

	struct stat sb;
	fstat(fd, &sb);
	uint8_t_array_t *buffer = uint8_t_array_create(sb.st_size);
	buffer->len = sb.st_size;

	long n = 0;
	long left_bytes = sb.st_size;

	while (left_bytes) {
		n = read(fd, buffer->data + n, left_bytes);
		assert(n > 0 && "fread failed");
		left_bytes -= n;
	}
	close(fd);
	printf("Input file has size \t%zu\n", buffer->size);
	return buffer;
}

bitstream_reader_t* bitstream_reader_create(uint8_t_array_t *a_vector)
{
	bitstream_reader_t *ret = calloc(1, sizeof(*ret));
	ret->buffer = a_vector;
	return ret;
}

void io_reader_destroy(bitstream_reader_t *a_stream)
{
	if (a_stream) {
		printf("Input stream has size \t%zu\n", a_stream->buffer->size);
		uint8_t_array_destroy(a_stream->buffer);
		free(a_stream);
	}
}

bool io_reader_is_posible_read_next(bitstream_reader_t *a_stream
		, uint8_t a_codelength)
{
	const uint8_t len = (a_codelength + a_stream->currBitIndex + 7)/8;

	if (unlikely(a_stream->currByteIndex + len > a_stream->buffer->size)) {
		return false;
	}
	return true;
}

uint32_t io_reader_read_vcode(bitstream_reader_t *a_stream
		, uint8_t a_bits)
{
	uint32_t ret = 0;
	uint32_t resBitIndex = 0;
	uint8_t bits = a_bits;

	while (bits > 0) {
		uint32_t tmp = a_stream->buffer->data[a_stream->currByteIndex];
		debug("read byte %u\n", tmp);

		if (a_stream->currBitIndex == 0) {
			if (bits < 8) {
				const uint32_t mask = ~(~0U << bits);
				ret |= (tmp & mask) << resBitIndex;
				a_stream->currBitIndex += bits;
				bits = 0;
			} else {
				ret |= tmp << resBitIndex;
				resBitIndex += 8;
				bits -= 8;
				++a_stream->currByteIndex;
			}
		} else {
			const uint32_t mask = ~(~0U << bits);
			ret |= ((tmp >> a_stream->currBitIndex)
					& mask) << resBitIndex;
			const uint32_t bitsLeft = 8 - a_stream->currBitIndex;
			if (bits < bitsLeft) {
				a_stream->currBitIndex += bits;
				bits = 0;
			} else {
				a_stream->currBitIndex = 0;
				++a_stream->currByteIndex;
				bits -= bitsLeft;
				resBitIndex += bitsLeft;
			}
		}
	}

	return ret;
}

#ifndef NDEBUG
void bitstreams_test(const char *a_file)
{
	size_t max = 511;
	assert(a_file);
	remove(a_file);
	bitstream_writer_t *writer = buffered_io_writer_create(a_file);
	assert(writer);

	for (size_t i = 0; i < max; ++i) {
		buffered_io_writer_write_vcode(writer, 1, 9);
	}
	buffered_io_writer_close(writer);

	uint8_t_array_t *input = read_file(a_file);
	assert(input);
	bitstream_reader_t *reader = bitstream_reader_create(input);

	for (uint32_t i = 0; i < max; ++i) {
		assert(io_reader_is_posible_read_next(reader, 9));
		uint32_t code = io_reader_read_vcode(reader, 9);
		if (code != 1) {
			printf("code %u i %u\n", code, i);
//			assert(code == i);
		}
	}

	io_reader_destroy(reader);
}
#endif
