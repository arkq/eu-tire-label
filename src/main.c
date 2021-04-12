/*
 * EU-tire-label - main.c
 * Copyright (c) 2015-2021 Arkadiusz Bokowy
 *
 * This file is a part of EU-tire-label.
 *
 * This project is licensed under the terms of the MIT license.
 *
 */

#define _GNU_SOURCE
#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "label.h"
#if ENABLE_PNG
# include "raster.h"
#endif

enum output_format {
	FORMAT_SVG = 0,
	FORMAT_PNG,
};

/* Parse label dimensions according to the WIDTH[xHEIGHT] format. If parsing
 * fails passed variables are not modified. */
void parse_label_dimensions(const char *str, int *width, int *height) {

	int w = 0, h = 0;
	char *ptr, *tmp;

	if ((tmp = strdup(str)) == NULL)
		return;

	ptr = tmp;
	while ((*ptr = tolower(*ptr)) != '\0')
		ptr++;

	if ((ptr = strchr(tmp, 'x')) != NULL) {
		*ptr = '\0';
		ptr++;
	}

	w = atoi(tmp);
	if (ptr)
		h = atoi(ptr);

	if (w)
		*width = w;
	if (h)
		*height = h;

	free(tmp);
}

/* Decode URL-encoded string. Memory for decoded string is allocated with
 * malloc(3), and shall be freed with free(3). */
static char *urldecode(const char *str) {

	size_t len = strlen(str);
	char *decoded = malloc(len + 1);
	char *p = decoded;
	unsigned int ch;
	size_t i;

	for (i = 0; i < len; i++)
		switch (str[i]) {
		case '+':
			*p++ = ' ';
			break;
		case '%':
			if (i + 2 < len &&
					isxdigit(str[i + 1]) &&
					isxdigit(str[i + 2])) {
				sscanf(&str[i + 1], "%02x", &ch);
				*p++ = ch;
				i += 2;
				break;
			}
			/* fall-through */
		default:
			*p++ = str[i];
			break;
		}

	*p = '\0';
	return decoded;
}

int main(int argc, char **argv) {

	int opt;
	const char *opts = "hVU:M:T:S:C:F:G:R:N:WI";
	struct option longopts[] = {
		{ "help", no_argument, NULL, 'h' },
		{ "version", no_argument, NULL, 'V' },
		{ "output-svg", no_argument, NULL, 's' },
#if ENABLE_PNG
		{ "output-png", required_argument, NULL, 'p' },
#endif
		{ "eprel-url", required_argument, NULL, 'U' },
		{ "trademark", required_argument, NULL, 'M' },
		{ "tire-type", required_argument, NULL, 'T' },
		{ "tire-size", required_argument, NULL, 'S' },
		{ "tire-class", required_argument, NULL, 'C' },
		{ "fuel-efficiency", required_argument, NULL, 'F' },
		{ "wet-grip", required_argument, NULL, 'G' },
		{ "rolling-noise", required_argument, NULL, 'R' },
		{ "rolling-noise-db", required_argument, NULL, 'N' },
		{ "snow-grip", no_argument, NULL, 'W' },
		{ "ice-grip", no_argument, NULL, 'I' },
		{ 0, 0, 0, 0 },
	};

	struct eu_tire_label data = { 0 };
	enum output_format format = FORMAT_SVG;
	bool label_EU_2020_740 = false;
	char *label = NULL;
	/* dimensions used for PNG output */
	int width = -1;
	int height = -1;

	/* parse options */
	while ((opt = getopt_long(argc, argv, opts, longopts, NULL)) != -1)
		switch (opt) {
		case 'h' /* --help */:
usage:
			printf("Usage:\n"
					"  %s [OPTION]...\n"
					"\nOptions:\n"
					"  -h, --help                   print this help and exit\n"
					"  -V, --version                print version and exit\n"
#if ENABLE_PNG
					"  --output-svg                 return label in the SVG format (default)\n"
					"  --output-png=WIDTH[xHEIGHT]  return label in the PNG format\n"
#endif
					"  -U, --eprel-url=URL          URL link to EPREL entry (for EU/2020/740)\n"
					"  -M, --trademark=NAME         trademark string (for EU/2020/740)\n"
					"  -T, --tire-type=NAME         tire type string (for EU/2020/740)\n"
					"  -S, --tire-size=NAME         tire size designation string (for EU/2020/740)\n"
					"  -C, --tire-class=CLASS       tire class value; one of: 1, 2 or 3\n"
					"  -F, --fuel-efficiency=CLASS  fuel efficiency class in the numerical or\n"
					"                               letter format; allowed values: 1-7 or A-G\n"
					"  -G, --wet-grip=CLASS         wet grip class in the numerical or letter\n"
					"                               format; allowed values: 1-7 or A-G\n"
					"  -R, --rolling-noise=CLASS    external rolling noise class in the numerical\n"
					"                               or letter format; allowed values: 1-3 or A-C\n"
					"  -N, --rolling-noise-db=DB    external rolling noise value expressed in dB\n"
					"  -W, --snow-grip              show snow grip pictogram (for EU/2020/740)\n"
					"  -I, --ice-grip               show ice grip pictogram (for EU/2020/740)\n",
					argv[0]);
			return EXIT_SUCCESS;

		case 'V' /* --version */:
			printf(VERSION "\n");
			return EXIT_SUCCESS;

		case 's' /* --output-svg */:
			format = FORMAT_SVG;
			break;
		case 'p' /* --output-png=WIDTH[xHEIGHT] */:
			format = FORMAT_PNG;
			parse_label_dimensions(optarg, &width, &height);
			break;

		case 'U' /* --eprel-url=URL */:
			strncpy(data.qrcode, optarg, sizeof(data.qrcode) - 1);
			/* If EPREL URL was given it must mean that someone is trying to render
			 * EU/2020/740 label, otherwise EC/1222/2009 label will be generated. */
			label_EU_2020_740 = true;
			break;
		case 'M' /* --trademark=NAME */:
			strncpy(data.trademark, optarg, sizeof(data.trademark) - 1);
			break;
		case 'T' /* --tire-type=NAME */:
			strncpy(data.tire_type, optarg, sizeof(data.tire_type) - 1);
			break;
		case 'S' /* --tire-size=NAME */:
			strncpy(data.tire_size, optarg, sizeof(data.tire_size) - 1);
			break;
		case 'C' /* --tire-class=CLASS */:
			data.tire_class = parse_tire_class(optarg);
			break;
		case 'F' /* --fuel-efficiency=CLASS */:
			data.fuel_efficiency = parse_fuel_efficiency_class(optarg);
			break;
		case 'G' /* --wet-grip=CLASS */:
			data.wet_grip = parse_wet_grip_class(optarg);
			break;
		case 'R' /* --rolling-noise=CLASS */:
			data.rolling_noise = parse_rolling_noise_class(optarg);
			break;
		case 'N' /* --rolling-noise-db=DB */:
			data.rolling_noise_db = parse_rolling_noise_db(optarg);
			break;
		case 'W' /* --snow-grip */:
			data.snow_grip = 1;
			break;
		case 'I' /* --ice-grip */:
			data.ice_grip = 1;
			break;

		default:
			fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
			return EXIT_FAILURE;
		}

	if (optind != argc)
		/* this program does not take any arguments */
		goto usage;

#if ENABLE_CGI
	/* detect whatever we are in the CGI environment, and if not, proceed
	 * as a normal console-based application */
	bool cgi = false;
	const char *tmp;
	if ((tmp = getenv("REQUEST_METHOD")) != NULL) {
		cgi = true;

		/* handle GET requests only */
		if (strcmp(tmp, "GET") != 0) {
			fprintf(stdout, "Status: 405 Method Not Allowed\r\n\r\n");
			return EXIT_FAILURE;
		}

		if ((tmp = getenv("QUERY_STRING")) != NULL) {

			char *query;
			if ((query = strdup(tmp)) == NULL) {
				perror("error: duplicate query string");
				return EXIT_FAILURE;
			}

			char *str = query;
			char *token;

			/* dissect and parse query string */
			while ((token = strtok(str, "&")) != NULL) {
				char *tmp = NULL;
				str = NULL;

#if ENABLE_PNG
				if (strcasestr(token, "PNG=") == token) {
					format = FORMAT_PNG;
					parse_label_dimensions(&token[4], &width, &height);
				}
#endif

				if (strcasestr(token, "U=") == token) {
					strncpy(data.qrcode, tmp = urldecode(&token[2]), sizeof(data.qrcode) - 1);
					label_EU_2020_740 = true;
				}
				else if (strcasestr(token, "M=") == token)
					strncpy(data.trademark, tmp = urldecode(&token[2]), sizeof(data.trademark) - 1);
				else if (strcasestr(token, "T=") == token)
					strncpy(data.tire_type, tmp = urldecode(&token[2]), sizeof(data.tire_type) - 1);
				else if (strcasestr(token, "S=") == token)
					strncpy(data.tire_size, tmp = urldecode(&token[2]), sizeof(data.tire_size) - 1);
				else if (strcasestr(token, "C=") == token)
					data.tire_class = parse_tire_class(&token[2]);
				else if (strcasestr(token, "F=") == token)
					data.fuel_efficiency = parse_fuel_efficiency_class(&token[2]);
				else if (strcasestr(token, "G=") == token)
					data.wet_grip = parse_wet_grip_class(&token[2]);
				else if (strcasestr(token, "R=") == token)
					data.rolling_noise = parse_rolling_noise_class(&token[2]);
				else if (strcasestr(token, "N=") == token)
					data.rolling_noise_db = parse_rolling_noise_db(&token[2]);
				else if (strcasestr(token, "W") == token)
					data.snow_grip = 1;
				else if (strcasestr(token, "I") == token)
					data.ice_grip = 1;

				free(tmp);
			}

			free(query);
		}

	}
#endif

	if (data.tire_class == TC_ERROR) {
		fprintf(stderr, "error: tire class option is required\n");
#if ENABLE_CGI
		if (cgi)
			fprintf(stdout, "Status: 400 Bad Request\r\n\r\n");
#endif
		return EXIT_FAILURE;
	}

	if (sanitize_plain_text(data.trademark) != 0)
		fprintf(stderr, "warning: found CDATA end sequence \"]]>\" in trademark string\n");
	if (sanitize_plain_text(data.tire_type) != 0)
		fprintf(stderr, "warning: found CDATA end sequence \"]]>\" in tire type string\n");
	if (sanitize_plain_text(data.tire_size) != 0)
		fprintf(stderr, "warning: found CDATA end sequence \"]]>\" in tire size string\n");

	if (label_EU_2020_740)
		label = create_label_EU_2020_740(&data);
	else
		label = create_label_EC_1222_2009(&data);
	if (label == NULL) {
		perror("error: create label");
		return EXIT_FAILURE;
	}

#if ENABLE_PNG
	struct raster_png *png = NULL;
	if (format == FORMAT_PNG &&
			(png = raster_svg_to_png(label, width, height)) == NULL) {
		perror("error: raster label to PNG");
		return EXIT_FAILURE;
	}
#endif

#if ENABLE_CGI
	if (cgi) {
		fprintf(stdout, "Status: 200 OK\r\n");
		switch (format) {
		case FORMAT_SVG:
			fprintf(stdout, "Content-Type: image/svg+xml\r\n");
			fprintf(stdout, "Content-Length: %zu\r\n", strlen(label));
			break;
#if ENABLE_PNG
		case FORMAT_PNG:
			fprintf(stdout, "Content-Type: image/png\r\n");
			fprintf(stdout, "Content-Length: %zu\r\n", png->length);
			break;
#endif
		}
		fprintf(stdout, "\r\n");
	}
#endif

	/* dump created label to the standard output */
	switch (format) {
	case FORMAT_SVG:
		fprintf(stdout, "%s\n", label);
		break;
#if ENABLE_PNG
	case FORMAT_PNG:
		fwrite(png->data, png->length, 1, stdout);
		break;
#endif
	}

#if ENABLE_PNG
	raster_png_free(png);
#endif
	free(label);
	return EXIT_SUCCESS;
}
