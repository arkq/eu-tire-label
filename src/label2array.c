/*
 * EU-tire-label - bin2array.c
 * Copyright (c) 2015-2021 Arkadiusz Bokowy
 *
 * This file is a part of EU-tire-label.
 *
 * This project is licensed under the terms of the MIT license.
 *
 */

#include <stdio.h>

int label2array(const char *variable, const char *filename) {

	FILE *in;
	if ((in = fopen(filename, "r")) == NULL)
		return -1;

	printf("const char %s[] = \"", variable);

	unsigned char c;
	while (fread(&c, 1, 1, in) > 0)
		switch (c) {
		case '\t':
		case '\n':
			break;
		case '"':
			printf("\\\"");
			break;;
		default:
			printf("%c", c);
		}

	printf("\";\n");

	return 0;
}

int main(int argc, char *argv[]) {

	if (argc != 3) {
		fprintf(stderr, "usage: %s <var> <file>\n", argv[0]);
		return 1;
	}

	if (label2array(argv[1], argv[2]) == -1)
		perror("label2array");

	return 0;
}
