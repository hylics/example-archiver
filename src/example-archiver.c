/*
 ============================================================================
 Name        : example-archiver.c
 Author      : Eugen
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

//#define NDEBUG

#ifndef NDEBUG
#define debug(...) printf(__VA_ARGS__)
#else
#define debug(...)
#endif

typedef enum action {
	UNDEFINED_ACTION=-1 ,
	COMPRESS,
	DECOMPRESS,
} action_t;

typedef enum alg {
	UNDEFINED_ALGORITHM=-1,
	LZW,
	LZMA,
} alg_t;

typedef void (*action_fp)(char *);

static void usage(char *app_name);
static void lzw_compress(char *a_file);
static void lzw_decompress(char *a_file);
static void lzma_compress(char *a_file);
static void lzma_decompress(char *a_file);

int main(int argc, char *argv[]) {
	int opt = 0;
	action_t action = UNDEFINED_ACTION;
	alg_t algorithm = LZW;
	char *file = NULL;
	action_fp fp_table[2][2] = {{lzw_compress, lzw_decompress}, {lzma_compress, lzma_decompress}};

	if (argc < 3) {
		usage(argv[0]);
	}

	while((opt = getopt(argc, argv, "a:c:d:h?")) != -1) {
		switch (opt) {
		case 'a':
			if (0 == strcasecmp(optarg, "LZW")) {
				algorithm = LZW;
			} else if (0 == strcasecmp(optarg, "LZMA")) {
				algorithm = LZMA;
			} else {
				usage(argv[0]);
			}
			break;
		case 'c':
			if (action != UNDEFINED_ACTION) {
				usage(argv[0]);
			}
			action = COMPRESS;
			file = optarg;
			break;
		case 'd':
			if (action != UNDEFINED_ACTION) {
				usage(argv[0]);
			}
			action = DECOMPRESS;
			file = optarg;
			break;
		case 'h':
		case '?':
		default:
			usage(argv[0]);
			break;
		}
	}

	fp_table[algorithm][action](file);

	return EXIT_SUCCESS;
}

static void usage(char *app_name)
{
	fprintf(stdout, "usage: %s [parameters]\n", app_name);
	fprintf(stdout, "parameters are:\n");
	fprintf(stdout, "\t-a lzw|lzma\t\tcompression or decompression algorithm. LZW is default\n");
	fprintf(stdout, "\t-c file\t\tfile to compress, output file will be 'file.[lzw|lzma]'\n");
	fprintf(stdout, "\t-d file.[lzw|lzma]\t\tfile to decompress, output will be 'file'\n");
	fprintf(stdout, "\t[-d] and [-c] are mutually exclusive\n");
	exit( EXIT_FAILURE );
}

static void lzw_compress(char *a_file)
{
	debug("%s %s\n", __func__, a_file);
}

static void lzw_decompress(char *a_file)
{
	debug("%s %s\n", __func__, a_file);
}

static void lzma_compress(char *a_file)
{
	debug("%s %s\n", __func__, a_file);
}

static void lzma_decompress(char *a_file)
{
	debug("%s %s\n", __func__, a_file);
}
