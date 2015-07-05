/*
 * EU-tire-label - raster.c
 * Copyright (c) 2015 Arkadiusz Bokowy
 *
 * This file is a part of an EU-tire-label.
 *
 * This projected is licensed under the terms of the MIT license.
 *
 */

#include "raster.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <librsvg/rsvg.h>


/* Write PNG data into our raster structure. Note, that passed structure
 * should be initialized to 0, otherwise new data will be appended to end
 * of the data buffer. */
static cairo_status_t _png_write_callback(void *closure,
		const unsigned char *data, unsigned int length) {

	struct raster_png *png = (struct raster_png *)closure;
	size_t new_length = png->length + length;
	unsigned char *new_data;

	if ((new_data = realloc(png->data, new_length)) == NULL)
		return CAIRO_STATUS_WRITE_ERROR;

	memcpy(&new_data[png->length], data, length);

	png->data = new_data;
	png->length = new_length;

	return CAIRO_STATUS_SUCCESS;
}

/* Rasterise given SVG image into the PNG format. When dimensions (width and
 * height) are set to -1, than SVG view-box is used. If only height is set to
 * -1, then original aspect ratio is preserved and image is resized according
 * to the width parameter. Upon failure this function returns NULL. */
struct raster_png *raster_svg_to_png(const char *svg,
		unsigned int width, unsigned int height) {

	RsvgHandle *rsvg;
	RsvgDimensionData dimension;
	cairo_t *cr;
	cairo_surface_t *surface;
	cairo_matrix_t matrix;
	struct raster_png *png;

	if ((png = calloc(1, sizeof(*png))) == NULL)
		return NULL;

	if ((rsvg = rsvg_handle_new_from_data(svg, strlen(svg), NULL)) == NULL) {
		raster_png_free(png);
		return NULL;
	}

	/* initialize default dimensions based on the SVG view-box */
	rsvg_handle_get_dimensions(rsvg, &dimension);
	if (width == -1)
		width = dimension.width;
	if (height == -1)
		height = round((double)(width * dimension.height) / dimension.width);

	/* scale SVG image according to the given dimensions */
	cairo_matrix_init_scale(&matrix,
			(double)width / dimension.width, (double)height / dimension.height);

	surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height);
	cr = cairo_create(surface);
	cairo_set_matrix(cr, &matrix);

	/* draw our SVG data to the Cairo surface */
	if (rsvg_handle_render_cairo(rsvg, cr))
		cairo_surface_write_to_png_stream(surface, _png_write_callback, png);

	cairo_destroy(cr);
	cairo_surface_destroy(surface);
	g_object_unref(G_OBJECT(rsvg));

	return png;
}

/* Free memory allocated by the raster_svg_to_png function. */
void raster_png_free(struct raster_png *png) {
	if (png) {
		free(png->data);
		free(png);
	}
}
