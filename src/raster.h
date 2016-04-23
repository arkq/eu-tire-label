/*
 * EU-tire-label - raster.h
 * Copyright (c) 2015 Arkadiusz Bokowy
 *
 * This file is a part of an EU-tire-label.
 *
 * This project is licensed under the terms of the MIT license.
 *
 */

#include <stddef.h>


struct raster_png {
	unsigned char *data;
	size_t length;
};


struct raster_png *raster_svg_to_png(const char *svg,
		unsigned int width, unsigned int height);
void raster_png_free(struct raster_png *png);
