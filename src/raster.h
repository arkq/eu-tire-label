/*
 * EU-tire-label - raster.h
 * Copyright (c) 2015-2021 Arkadiusz Bokowy
 *
 * This file is a part of EU-tire-label.
 *
 * This project is licensed under the terms of the MIT license.
 *
 */

#pragma once
#ifndef EUTIRELABEL_RASTER_H_
#define EUTIRELABEL_RASTER_H_

#include <stddef.h>

struct raster_png {
	unsigned char *data;
	size_t length;
};

struct raster_png *raster_svg_to_png(const char *svg, int width, int height);
void raster_png_free(struct raster_png *png);

#endif
