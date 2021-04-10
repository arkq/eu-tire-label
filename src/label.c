/*
 * EU-tire-label - label.c
 * Copyright (c) 2015-2021 Arkadiusz Bokowy
 *
 * This file is a part of EU-tire-label.
 *
 * This project is licensed under the terms of the MIT license.
 *
 */

#include "label.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* auto-generated SVG templates */
#include "label_EC_1222_2009-template.h"
#include "label_EU_2020_740-template.h"

/* Replace every occurrence of src in the data with dst. Memory for the new
 * string is obtained with malloc(3), and can be freed with free(3). */
static char *strrep(const char *data, const char *src, const char *dst) {

	char *newdata = strdup(data);
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

char *create_label_EC_1222_2009(const struct eu_tire_label *data) {

	static const char *classes[] = { NULL, "C1", "C2", "C3" };
	static const char *letters[] = { NULL, "A", "B", "C", "D", "E", "F", "G" };
	static const char *y[] = { NULL, "24.375", "29.875", "35.375", "40.875", "46.375", "51.875", "58.375" };
	char *label;

	label = strrep(label_EC_1222_2009_template, "[TIRE-CLASS]", classes[data->tire_class]);

	if (data->fuel_efficiency != FEC_NONE) {
		label = strrep_x(label, "[FUEL-EFFICIENCY-DISPLAY]", "");
		label = strrep_x(label, "[FUEL-EFFICIENCY-Y]", y[data->fuel_efficiency]);
		label = strrep_x(label, "[FUEL-EFFICIENCY]", letters[data->fuel_efficiency]);
	}
	label = strrep_x(label, "[FUEL-EFFICIENCY-DISPLAY]", "none");

	if (data->wet_grip != WGC_NONE) {
		label = strrep_x(label, "[WET-GRIP-DISPLAY]", "");
		label = strrep_x(label, "[WET-GRIP-Y]", y[data->wet_grip]);
		label = strrep_x(label, "[WET-GRIP]", letters[data->wet_grip]);
	}
	label = strrep_x(label, "[WET-GRIP-DISPLAY]", "none");

	switch (data->rolling_noise) {
	case RNC_3:
		label = strrep_x(label, "[ROLLING-NOISE-3-DISPLAY]", "none");
		/* fall-through */
	case RNC_2:
		label = strrep_x(label, "[ROLLING-NOISE-2-DISPLAY]", "none");
		/* fall-through */
	case RNC_1:
		label = strrep_x(label, "[ROLLING-NOISE-1-DISPLAY]", "none");
		break;
	case RNC_NONE:
		label = strrep_x(label, "[ROLLING-NOISE-DISPLAY]", "none");
		break;
	}
	label = strrep_x(label, "[ROLLING-NOISE-DISPLAY]", "");
	label = strrep_x(label, "[ROLLING-NOISE-1-DISPLAY]", "");
	label = strrep_x(label, "[ROLLING-NOISE-2-DISPLAY]", "");
	label = strrep_x(label, "[ROLLING-NOISE-3-DISPLAY]", "");

	if (data->rolling_noise_db) {
		char tmp[8];
		sprintf(tmp, "%d", data->rolling_noise_db);
		label = strrep_x(label, "[ROLLING-NOISE-DB-DISPLAY]", "");
		label = strrep_x(label, "[ROLLING-NOISE-DB]", tmp);
	}
	label = strrep_x(label, "[ROLLING-NOISE-DB-DISPLAY]", "none");

	return label;
}

char *create_label_EU_2020_740(const struct eu_tire_label *data) {

	static const char *classes[] = { NULL, "C1", "C2", "C3" };
	static const char *letters[] = { NULL, "A", "B", "C", "D", "E", "E", "E" };
	static const char *y[] = { NULL, "19.25", "27.25", "35.25", "43.25", "51.25", "51.25", "51.25" };
	char *label;

	label = strrep(label_EU_2020_740_template, "[TRADEMARK]", data->trademark);
	label = strrep_x(label, "[TIRE-TYPE]", data->tire_type);
	label = strrep_x(label, "[TIRE-SIZE-DESIGNATION]", data->tire_size);
	label = strrep_x(label, "[TIRE-CLASS]", classes[data->tire_class]);

	if (data->fuel_efficiency != FEC_NONE) {
		label = strrep_x(label, "[FUEL-EFFICIENCY-DISPLAY]", "");
		label = strrep_x(label, "[FUEL-EFFICIENCY-Y]", y[data->fuel_efficiency]);
		label = strrep_x(label, "[FUEL-EFFICIENCY]", letters[data->fuel_efficiency]);
	}
	label = strrep_x(label, "[FUEL-EFFICIENCY-DISPLAY]", "none");

	if (data->wet_grip != WGC_NONE) {
		label = strrep_x(label, "[WET-GRIP-DISPLAY]", "");
		label = strrep_x(label, "[WET-GRIP-Y]", y[data->wet_grip]);
		label = strrep_x(label, "[WET-GRIP]", letters[data->wet_grip]);
	}
	label = strrep_x(label, "[WET-GRIP-DISPLAY]", "none");

	return label;
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
		goto fail;

	value = tolower(str[0]) - 'a' + 1;
	if (value >= 1 && value <= 7)
		return (enum fuel_efficiency_class)value;

fail:
	fprintf(stderr, "warning: invalid fuel efficiency class: %s\n", str);
	return FEC_NONE;
}

enum wet_grip_class parse_wet_grip_class(const char *str) {

	int value = atoi(str);

	if (value >= 1 && value <= 7)
		return (enum wet_grip_class)value;

	if (strlen(str) != 1)
		goto fail;

	value = tolower(str[0]) - 'a' + 1;
	if (value >= 1 && value <= 7)
		return (enum wet_grip_class)value;

fail:
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

	if (value >= 10 && value <= 120)
		return value;

	fprintf(stderr, "warning: invalid rolling noise dB value: %s\n", str);
	return 0;
}
