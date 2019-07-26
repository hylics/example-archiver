/*
 ============================================================================
 Name        : example-archiver.c
 Author      : Eugen Kholomeniuk
 Version     :
 Copyright   : GPLV3
 Description : Simple archiver written for educational purposes
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "options.h"
#include "lzw_edu.h"
#include "log.h"


typedef void (*action_fp)(options_t *);


static void usage(char *app_name);

void lzma_compress(options_t *a_options);
void lzma_decompress(options_t *a_options);

int main(int argc, char *argv[]) {
	action_fp fp_table[2][2] = {{lzw_compress, lzw_decompress}
						, {lzma_compress, lzma_decompress}};
	options_t options = {.action = UNDEFINED_ACTION
			, .algorithm = LZW
			, .file_in = NULL
			, .file_out = NULL};

	if (argc < 3) {
		usage(argv[0]);
	}

	int opt = -1;
	while((opt = getopt(argc, argv, "a:cdi:o:h?")) != -1) {
		switch (opt) {
		case 'a':
			if (0 == strcasecmp(optarg, "LZW")) {
				options.algorithm = LZW;
			} else if (0 == strcasecmp(optarg, "LZMA")) {
				options.algorithm = LZMA;
			} else {
				usage(argv[0]);
			}
			break;
		case 'c':
			if (options.action != UNDEFINED_ACTION) {
				usage(argv[0]);
			}
			options.action = COMPRESS;
			break;
		case 'd':
			if (options.action != UNDEFINED_ACTION) {
				usage(argv[0]);
			}
			options.action = DECOMPRESS;
			break;
		case 'i':
			options.file_in = optarg;
			break;
		case 'o':
			options.file_out = optarg;
			break;
		case 'h':
		case '?':
		default:
			usage(argv[0]);
			break;
		}
	}

	if (!options.file_in || !options.file_out) {
		usage(argv[0]);
	}

	fp_table[options.algorithm][options.action](&options);

	return EXIT_SUCCESS;
}

static void usage(char *a_app_name)
{
	fprintf(stdout, "usage: %s [parameters]\n", a_app_name);
	fprintf(stdout, "parameters are:\n");
	fprintf(stdout, "\t-a lzw|lzma \t\t compression or decompression algorithm. "
			"LZW is default\n");
	fprintf(stdout, "\t-c \t\t compress\n");
	fprintf(stdout, "\t-d \t\t decompress\n");
	fprintf(stdout, "\t[-d] and [-c] are mutually exclusive\n");
	fprintf(stdout, "\t-i file \t\t input file\n");
	fprintf(stdout, "\t-o file \t\t output file\n");
	exit(EXIT_FAILURE);
}

void lzma_compress(options_t *a_options)
{
	debug("%s %s\n", __func__, a_options->file_in);
	fprintf(stderr, "not implemented\n");
	exit(EXIT_FAILURE);
}

void lzma_decompress(options_t *a_options)
{
	debug("%s %s\n", __func__, a_options->file_in);
	fprintf(stderr, "not implemented\n");
	exit(EXIT_FAILURE);
}
