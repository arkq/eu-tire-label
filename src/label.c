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


char *create_label(const struct eu_tire_label *label) {
	return NULL;
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
