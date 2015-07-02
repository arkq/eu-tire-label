/*
 * EU-tire-label - label.c
 * Copyright (c) 2015 Arkadiusz Bokowy
 *
 * This file is a part of an EU-tire-label.
 *
 * This projected is licensed under the terms of the MIT license.
 *
 */

#include "label.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* auto-generated label SVG template */
#include "label-template.h"


/* Replace every occurrence of src in the data with dst. Memory for the new
 * string is obtained with malloc(3), and can be freed with free(3). */
static char *strrep(const char *data, const char *src, const char *dst) {

	char *newdata = strdup(data);
	char *olddata = NULL;
	char *tok = NULL;

	size_t srclen = strlen(src);
	size_t dstlen = strlen(dst);
	size_t len = 0;

	while ((tok = strstr(&newdata[len], src)) != NULL) {

		int newdatalen = strlen(newdata);
		len += tok - &newdata[len];

		if (srclen < dstlen) {
			char *tmp;
			if ((tmp = realloc(newdata, newdatalen - srclen + dstlen + 1)) == NULL) {
				free(newdata);
				return NULL;
			}
			newdata = tmp;
		}

		memmove(&newdata[len + dstlen], &newdata[len + srclen], newdatalen - len - srclen + 1);
		memcpy(&newdata[len], dst, dstlen);

		len += dstlen;
	}

	return newdata;
}

/* Helper function related to the strrep which frees passed source memory.
 * Note, that the passed source string HAS TO be allocated with malloc(3)
 * functions family. */
static char *strrep_x(char *data, const char *src, const char *dst) {
	char *tmp = strrep(data, src, dst);
	free(data);
	return tmp;
}

char *create_label(const struct eu_tire_label *label) {

	const char *classes[] = { NULL, "C1", "C2", "C3" };
	const char *letters[] = { NULL, "A", "B", "C", "D", "E", "F", "G" };
	const char *y[] = { NULL, "0", "0", "0", "0", "0", "0", "0" };
	char *image;

	image = strrep(label_template, "[TIRE-CLASS]", classes[label->tire_class]);

	if (label->fuel_efficiency != FEC_NONE) {
		image = strrep_x(image, "[FUEL-EFFICIENCY-DISPLAY]", "");
		image = strrep_x(image, "[FUEL-EFFICIENCY-Y]", y[label->fuel_efficiency]);
		image = strrep_x(image, "[FUEL-EFFICIENCY]", letters[label->fuel_efficiency]);
	}
	image = strrep_x(image, "[FUEL-EFFICIENCY-DISPLAY]", "none");

	if (label->wet_grip != WGC_NONE) {
		image = strrep_x(image, "[WET-GRIP-DISPLAY]", "");
		image = strrep_x(image, "[WET-GRIP-Y]", y[label->wet_grip]);
		image = strrep_x(image, "[WET-GRIP]", letters[label->wet_grip]);
	}
	image = strrep_x(image, "[WET-GRIP-DISPLAY]", "none");

	switch (label->rolling_noise) {
	case RNC_3:
		image = strrep_x(image, "[ROLLING-NOISE-3-DISPLAY]", "none");
	case RNC_2:
		image = strrep_x(image, "[ROLLING-NOISE-2-DISPLAY]", "none");
	case RNC_1:
		image = strrep_x(image, "[ROLLING-NOISE-1-DISPLAY]", "none");
		break;
	case RNC_NONE:
		image = strrep_x(image, "[ROLLING-NOISE-DISPLAY]", "none");
	}
	image = strrep_x(image, "[ROLLING-NOISE-DISPLAY]", "");
	image = strrep_x(image, "[ROLLING-NOISE-1-DISPLAY]", "");
	image = strrep_x(image, "[ROLLING-NOISE-2-DISPLAY]", "");
	image = strrep_x(image, "[ROLLING-NOISE-3-DISPLAY]", "");

	if (label->rolling_noise_db) {
		char tmp[8];
		sprintf(tmp, "%d", label->rolling_noise_db);
		image = strrep_x(image, "[ROLLING-NOISE-DB-DISPLAY]", "");
		image = strrep_x(image, "[ROLLING-NOISE-DB]", tmp);
	}
	image = strrep_x(image, "[ROLLING-NOISE-DB-DISPLAY]", "none");

	return image;
}

enum tire_class parse_tire_class(const char *str) {

	int value = atoi(str);

	if (value >= 1 && value <= 3)
		return (enum tire_class)value;

	fprintf(stderr, "warning: invalid tire class: %s\n", str);
	return TC_ERROR;
}

enum fuel_efficiency_class parse_fuel_efficiency_class(const char *str) {

	int value = atoi(str);

	if (value >= 1 && value <= 7)
		return (enum fuel_efficiency_class)value;

	if (strlen(str) != 1)
		goto return_failure;

	value = tolower(str[0]) - 'a' + 1;
	if (value >= 1 && value <= 7)
		return (enum fuel_efficiency_class)value;

return_failure:
	fprintf(stderr, "warning: invalid fuel efficiency class: %s\n", str);
	return FEC_NONE;
}

enum wet_grip_class parse_wet_grip_class(const char *str) {

	int value = atoi(str);

	if (value >= 1 && value <= 7)
		return (enum wet_grip_class)value;

	if (strlen(str) != 1)
		goto return_failure;

	value = tolower(str[0]) - 'a' + 1;
	if (value >= 1 && value <= 7)
		return (enum wet_grip_class)value;

return_failure:
	fprintf(stderr, "warning: invalid wet grip class: %s\n", str);
	return WGC_NONE;
}

enum rolling_noise_class parse_rolling_noise_class(const char *str) {

	int value = atoi(str);

	if (value >= 1 && value <= 3)
		return (enum rolling_noise_class)value;

	fprintf(stderr, "warning: invalid rolling noise class: %s\n", str);
	return RNC_NONE;
}

unsigned int parse_rolling_noise_db(const char *str) {

	int value = atoi(str);

	if (value < 10 || value > 120)
		goto return_failure;

	return value;

return_failure:
	fprintf(stderr, "warning: invalid rolling noise dB value: %s\n", str);
	return 0;
}
