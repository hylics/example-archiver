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
	int fd;
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
	ret->fd = open(a_file, O_CREAT|O_WRONLY|O_EXCL|O_APPEND/*|O_NONBLOCK*/);
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
	return true;
}

void buffered_io_writer_write_vcode(bitstream_writer_t *a_stream
		, const uint16_t a_code, const uint8_t a_bits)
{
	/* LE|BE format -> BE format:
	 * 	 hi byte 0000 XXXX, lo byte XXXX XXXX for 12-bit code
	 * left shift -> XXXX XXXX XXXX 0000
	 */
	uint8_t bits = a_bits;
	code_t code = {.holder = htons(a_code) << (32-bits)};

	while (bits > 0) {
		if (a_stream->currBitIndex == 0) {
			a_stream->buffer->data[++a_stream->buffer->len - 1] = code.bytes.b1;
			if (bits < 8) {
				a_stream->currBitIndex += bits;
				bits = 0;
			} else {
				bits -= 8;
				code.holder <<= 8;
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
			const code_t tmp = {.holder = code.holder >> a_stream->currBitIndex};
			a_stream->buffer->data[a_stream->buffer->len - 1] |= tmp.bytes.b1;
			const uint_fast8_t bitsWritten = 8 - a_stream->currBitIndex;

			if (bits < bitsWritten) {
				a_stream->currBitIndex += bits;
				bits = 0;
			} else {
				bits -= bitsWritten;
				code.holder <<= bitsWritten;
				a_stream->currBitIndex = 0;
			}
		}
	}
}


bitstream_reader_t* io_reader_create(const char *a_file)
{
	struct stat sb;
	bitstream_reader_t *ret = calloc(1, sizeof(*ret));
	ret->fd = open(a_file, O_RDONLY);
	if (ret->fd < 0) {
		printf("[%s]: %s\n", a_file, strerror(errno));
		assert(ret->fd >= 0);
	}
#ifdef __APPLE__
	fcntl(ret->fd, F_RDAHEAD, 1);
#elif __linux__

#endif

	fstat(ret->fd, &sb);
	ret->buffer = uint8_t_array_create(sb.st_size);
	ret->buffer->len = sb.st_size;

	long n = 0;
	long left_bytes = sb.st_size;

	while (left_bytes) {
		n = read(ret->fd, ret->buffer->data + n, left_bytes);
		assert(n > 0 && "fread failed");
		left_bytes -= n;
	}
	printf("Input stream has size \t%zu\n", ret->buffer->size);
	return ret;
}

void io_reader_destroy(bitstream_reader_t *a_stream)
{
	if (a_stream) {
		printf("Input stream has size \t%zu\n", a_stream->buffer->size);
		uint8_t_array_destroy(a_stream->buffer);
		close(a_stream->fd);
		free(a_stream);
	}
}

uint16_t io_reader_read_vcode(bitstream_reader_t *a_stream
		, uint8_t codelength)
{
	return 0;
}
