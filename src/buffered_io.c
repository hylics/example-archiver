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
#include "arrays.h"
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
	uint8_t currBitIndex;
	int fd;
	struct stat sb;
	size_t written_len;
};

static bool write_buffer2file(bitstream_writer_t *a_buffer);

bitstream_writer_t* buffered_io_writer_create(const char *a_file)
{
	bitstream_writer_t *ret = calloc(1, sizeof(*ret));
	ret->fd = open(a_file, O_CREAT|O_WRONLY|O_EXCL|O_APPEND/*|O_NONBLOCK*/);
	if (ret->fd < 0) {
		printf("[%s]: %s\n", a_file, strerror(errno));
		assert(ret->fd >= 0);
	}
	fstat(ret->fd, &ret->sb);
	ret->buffer = uint8_t_array_create(ret->sb.st_blksize * 128);
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
