# example-archiver
Simple archiver tool written for educational purposes

Build:
For Linux or MacOSX:
1) '$ cd Release' [or Debug] folder
2) '$ make'

Usage:
./example-archiver [parameters]
parameters are:
	-a lzw|lzma 		 compression or decompression algorithm. LZW is default
	-c 		 compress
	-d 		 decompress
	[-d] and [-c] are mutually exclusive
	-i file 		 input file
	-o file 		 output file
