/*
 * EU-tire-label - main.c
 * Copyright (c) 2015 Arkadiusz Bokowy
 *
 * This file is a part of an EU-tire-label.
 *
 * This project is licensed under the terms of the MIT license.
 *
 */

#if HAVE_CONFIG_H
#include "../config.h"
#endif

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "label.h"
#if ENABLE_RSVG
#include "raster.h"
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


int main(int argc, char **argv) {

	int opt;
	const char *opts = "hC:F:G:R:N:";
	struct option longopts[] = {
		{ "help", no_argument, NULL, 'h' },
		{ "output-svg", no_argument, NULL, 's' },
#if ENABLE_RSVG
		{ "output-png", required_argument, NULL, 'p' },
#endif
		{ "tire-class", required_argument, NULL, 'C' },
		{ "fuel-efficiency", required_argument, NULL, 'F' },
		{ "wet-grip", required_argument, NULL, 'G' },
		{ "rolling-noise", required_argument, NULL, 'R' },
		{ "rolling-noise-db", required_argument, NULL, 'N' },
		{ 0, 0, 0, 0 },
	};

	struct eu_tire_label label_data = { 0 };
	enum output_format label_format = FORMAT_SVG;
	char *label_svg_image = NULL;

	/* label dimensions used for rasterisation */
	int label_width = -1;
	int label_height = -1;

	/* parse options */
	while ((opt = getopt_long(argc, argv, opts, longopts, NULL)) != -1)
		switch (opt) {
		case 'h':
return_usage:
			printf("usage: %s [options]\n"
					"options:\n"
#if ENABLE_RSVG
					"  --output-svg                 return label in the SVG format (default)\n"
					"  --output-png=WIDTH[xHEIGHT]  return label in the PNG format\n"
#endif /* ENABLE_RSVG */
					"  -C, --tire-class=CLASS       tire class value; one of: 1, 2 or 3\n"
					"  -F, --fuel-efficiency=CLASS  fuel efficiency class in the numerical or\n"
					"                               letter format; allowed values: 1-7 or A-G\n"
					"  -G, --wet-grip=CLASS         wet grip class in the numerical or letter\n"
					"                               format; allowed values: 1-7 or A-G\n"
					"  -R, --rolling-noise=CLASS    external rolling noise class value;\n"
					"                               one of: 1, 2 or 3\n"
					"  -N, --rolling-noise-db=DB    external rolling noise value expressed in dB\n",
					argv[0]);
			return EXIT_SUCCESS;

		case 's':
			label_format = FORMAT_SVG;
			break;
		case 'p':
			label_format = FORMAT_PNG;
			parse_label_dimensions(optarg, &label_width, &label_height);
			break;

		case 'C':
			label_data.tire_class = parse_tire_class(optarg);
			break;
		case 'F':
			label_data.fuel_efficiency = parse_fuel_efficiency_class(optarg);
			break;
		case 'G':
			label_data.wet_grip = parse_wet_grip_class(optarg);
			break;
		case 'R':
			label_data.rolling_noise = parse_rolling_noise_class(optarg);
			break;
		case 'N':
			label_data.rolling_noise_db = parse_rolling_noise_db(optarg);
			break;

		default:
			fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
			return EXIT_FAILURE;
		}

	if (optind != argc)
		/* this program does not take any arguments */
		goto return_usage;

#if ENABLE_CGI
	/* detect whatever we are in the CGI environment, and if not, proceed
	 * as a normal console-based application */
	int cgi = 0;
	const char *tmp;
	if ((tmp = getenv("REQUEST_METHOD")) != NULL) {
		cgi = 1;

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

			/* make query string case-insensitive */
			char *ptr = query;
			while ((*ptr = toupper(*ptr)) != '\0')
				ptr++;

			char *str = query;
			char *token;

			/* dissect and parse query string */
			while ((token = strtok(str, "&")) != NULL) {
				str = NULL;

#if ENABLE_RSVG
				if (strstr(token, "PNG=") == token) {
					label_format = FORMAT_PNG;
					parse_label_dimensions(&token[4], &label_width, &label_height);
				}
#endif /* ENABLE_RSVG */

				if (strstr(token, "C=") == token)
					label_data.tire_class = parse_tire_class(&token[2]);
				else if (strstr(token, "F=") == token)
					label_data.fuel_efficiency = parse_fuel_efficiency_class(&token[2]);
				else if (strstr(token, "G=") == token)
					label_data.wet_grip = parse_wet_grip_class(&token[2]);
				else if (strstr(token, "R=") == token)
					label_data.rolling_noise = parse_rolling_noise_class(&token[2]);
				else if (strstr(token, "N=") == token)
					label_data.rolling_noise_db = parse_rolling_noise_db(&token[2]);

			}

			free(query);
		}

	}
#endif /* ENABLE_CGI */

	if (label_data.tire_class == TC_ERROR) {
		fprintf(stderr, "error: tire class option is required\n");
#if ENABLE_CGI
		if (cgi)
			fprintf(stdout, "Status: 400 Bad Request\r\n\r\n");
#endif /* ENABLE_CGI */
		return EXIT_FAILURE;
	}

	if ((label_svg_image = create_label(&label_data)) == NULL) {
		perror("error: create label");
		return EXIT_FAILURE;
	}

#if ENABLE_RSVG
	struct raster_png *png;
	if ((png = raster_svg_to_png(label_svg_image, label_width, label_height)) == NULL) {
		perror("error: raster label to PNG");
		return EXIT_FAILURE;
	}
#endif /* ENABLE_RSVG */

#if ENABLE_CGI
	if (cgi) {
		fprintf(stdout, "Status: 200 OK\r\n");
		switch (label_format) {
		case FORMAT_SVG:
			fprintf(stdout, "Content-Type: image/svg+xml\r\n");
			fprintf(stdout, "Content-Length: %zu\r\n", strlen(label_svg_image));
			break;
#if ENABLE_RSVG
		case FORMAT_PNG:
			fprintf(stdout, "Content-Type: image/png\r\n");
			fprintf(stdout, "Content-Length: %zu\r\n", png->length);
			break;
#endif /* ENABLE_RSVG */
		}
		fprintf(stdout, "\r\n");
	}
#endif /* ENABLE_CGI */

	/* dump created label to the standard output */
	switch (label_format) {
	case FORMAT_SVG:
		fprintf(stdout, "%s\n", label_svg_image);
		break;
#if ENABLE_RSVG
	case FORMAT_PNG:
		fwrite(png->data, png->length, 1, stdout);
		break;
#endif /* ENABLE_RSVG */
	}

#if ENABLE_RSVG
	raster_png_free(png);
#endif
	free(label_svg_image);
	return EXIT_SUCCESS;
}
