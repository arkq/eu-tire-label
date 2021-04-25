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

/* downloaded QR-code library */
#include "qrcode.h"

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

/* Create QR code with given version and text data. Memory for QR code is
 * allocated with malloc(3), and shall be freed with free(3). */
static char *create_qrcode(unsigned int version, const char *text) {

	const char template[] = "M%zu,%zuh1v%zuh-1z";
	unsigned char *modules = malloc(qrcode_getBufferSize(version));
	char *p, *qrcode = NULL;
	size_t x, y;
	QRCode qr;

	if (qrcode_initText(&qr, modules, version, ECC_LOW, text) != 0)
		goto fail;

	/* The worst case scenario is that the QR code is half empty, so the
	 * total max number of rectangles is: qr.size * qr.size / 2. */
	if ((qrcode = p = malloc(sizeof(template) * qr.size * qr.size / 2)) == NULL)
		goto fail;

	p += sprintf(p, "<path d=\"");

	for (x = 0; x < qr.size; x++) {
		size_t start = 0, length = 0;
		for (y = 0; y < qr.size; y++) {
			bool ok = qrcode_getModule(&qr, x, y);
			if (ok && length == 0)
				start = y;
			if (ok)
				length++;
			if (length != 0 && (!ok || y + 1 == qr.size)) {
				p += sprintf(p, template, x, start, length);
				length = 0;
			}
		}
	}

	p += sprintf(p, "\"/>");

fail:
	free(modules);
	return qrcode;
}

char *create_label_EC_1222_2009(const struct eu_tire_label *data) {

	static const char *classes[] = { "", "C1", "C2", "C3" };
	static const char *display[] = { "none", "", "", "", "", "", "", "" };
	static const char *letters[] = { "", "A", "B", "C", "D", "E", "F", "G" };
	static const char *y[] = { "0", "24.375", "29.875", "35.375", "40.875", "46.375", "51.875", "58.375" };
	char db[8] = "";
	char *label;

	label = strrep(label_EC_1222_2009_template, "[TIRE-CLASS]", classes[data->tire_class]);

	label = strrep_x(label, "[FUEL-EFFICIENCY-DISPLAY]", display[data->fuel_efficiency]);
	label = strrep_x(label, "[FUEL-EFFICIENCY-Y]", y[data->fuel_efficiency]);
	label = strrep_x(label, "[FUEL-EFFICIENCY]", letters[data->fuel_efficiency]);

	label = strrep_x(label, "[WET-GRIP-DISPLAY]", display[data->wet_grip]);
	label = strrep_x(label, "[WET-GRIP-Y]", y[data->wet_grip]);
	label = strrep_x(label, "[WET-GRIP]", letters[data->wet_grip]);

	switch (data->rolling_noise) {
	case RNC_3:
		label = strrep_x(label, "[ROLLING-NOISE-3-DISPLAY]", "none");
		/* fall-through */
	case RNC_2:
		label = strrep_x(label, "[ROLLING-NOISE-2-DISPLAY]", "none");
		/* fall-through */
	case RNC_1:
		label = strrep_x(label, "[ROLLING-NOISE-1-DISPLAY]", "none");
		/* fall-through */
	case RNC_NONE:
		label = strrep_x(label, "[ROLLING-NOISE-1-DISPLAY]", "");
		label = strrep_x(label, "[ROLLING-NOISE-2-DISPLAY]", "");
		label = strrep_x(label, "[ROLLING-NOISE-3-DISPLAY]", "");
	}

	if (data->rolling_noise_db)
		sprintf(db, "%d", data->rolling_noise_db);
	label = strrep_x(label, "[ROLLING-NOISE-DB-DISPLAY]", data->rolling_noise_db ? "" : "none");
	label = strrep_x(label, "[ROLLING-NOISE-DB]", db);

	return label;
}

char *create_label_EU_2020_740(const struct eu_tire_label *data) {

	static const char *classes[] = { "", "C1", "C2", "C3" };
	static const char *display[] = { "none", "", "", "", "", "", "", "" };
	static const char *letters[] = { "", "A", "B", "C", "D", "E", "E", "E" };
	static const char *y[] = { "0", "19.25", "27.25", "35.25", "43.25", "51.25", "51.25", "51.25" };
	static const char *footer[][3] = {
		{ "0", "0", "0" },     /* empty */
		{ "22.5", "0", "0" },  /* rolling noise */
		{ "0", "30", "0" },    /* snow grip */
		{ "14", "41", "0" },   /* rolling noise + snow grip */
		{ "0", "0", "30" },    /* ice grip */
		{ "14", "0", "41" },   /* rolling noise + ice grip */
		{ "0", "20", "40" },   /* snow grip + ice grip */
		{ "4", "31", "51" }};  /* rolling noise + snow grip + ice grip */
	unsigned int x = 0;
	char db[8] = "";
	char *qrcode;
	char *label;

	qrcode = create_qrcode(3 /* 29 x 29 */, data->qrcode);
	label = strrep(label_EU_2020_740_template, "[QR-CODE]", qrcode);
	free(qrcode);

	label = strrep_x(label, "[TRADEMARK]", data->trademark);
	label = strrep_x(label, "[TIRE-TYPE]", data->tire_type);
	label = strrep_x(label, "[TIRE-SIZE-DESIGNATION]", data->tire_size);
	label = strrep_x(label, "[TIRE-CLASS]", classes[data->tire_class]);

	label = strrep_x(label, "[FUEL-EFFICIENCY-DISPLAY]", display[data->fuel_efficiency]);
	label = strrep_x(label, "[FUEL-EFFICIENCY-Y]", y[data->fuel_efficiency]);
	label = strrep_x(label, "[FUEL-EFFICIENCY]", letters[data->fuel_efficiency]);

	label = strrep_x(label, "[WET-GRIP-DISPLAY]", display[data->wet_grip]);
	label = strrep_x(label, "[WET-GRIP-Y]", y[data->wet_grip]);
	label = strrep_x(label, "[WET-GRIP]", letters[data->wet_grip]);

	if (data->rolling_noise != RNC_NONE || data->rolling_noise_db)
		x |= 1 << 0;
	if (data->snow_grip)
		x |= 1 << 1;
	if (data->ice_grip)
		x |= 1 << 2;

	if (data->rolling_noise_db)
		sprintf(db, "%d", data->rolling_noise_db);

	label = strrep_x(label, "[ROLLING-NOISE-DISPLAY]", data->rolling_noise_db ? "" : "none");
	label = strrep_x(label, "[ROLLING-NOISE-X]", footer[x][0]);
	label = strrep_x(label, "[ROLLING-NOISE-DB]", db);

	switch (data->rolling_noise) {
	case RNC_1:
		label = strrep_x(label, "[ROLLING-NOISE-A]", "active");
		goto case_rnc_none;
	case RNC_2:
		label = strrep_x(label, "[ROLLING-NOISE-B]", "active");
		goto case_rnc_none;
	case RNC_3:
		label = strrep_x(label, "[ROLLING-NOISE-C]", "active");
		goto case_rnc_none;
	case RNC_NONE:
	case_rnc_none:
		label = strrep_x(label, "[ROLLING-NOISE-A]", "");
		label = strrep_x(label, "[ROLLING-NOISE-B]", "");
		label = strrep_x(label, "[ROLLING-NOISE-C]", "");
	}

	label = strrep_x(label, "[SNOW-GRIP-DISPLAY]", data->snow_grip ? "" : "none");
	label = strrep_x(label, "[SNOW-GRIP-X]", footer[x][1]);

	label = strrep_x(label, "[ICE-GRIP-DISPLAY]", data->ice_grip ? "" : "none");
	label = strrep_x(label, "[ICE-GRIP-X]", footer[x][2]);

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
		return value;

	if (strlen(str) != 1)
		goto fail;

	value = tolower(str[0]) - 'a' + 1;
	if (value >= 1 && value <= 3)
		return value;

fail:
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

unsigned int sanitize_plain_text(char *text) {

	unsigned int rv = 0;
	char *tmp;

	while ((tmp = strstr(text, "]]>")) != NULL) {
		tmp[1] = '|';
		rv = 1;
	}

	return rv;
}
