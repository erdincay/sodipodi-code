#define __SP_COLOR_C__

/*
 * Colors and colorspaces
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 2001-2002 Lauris Kaplinski
 * Copyright (C) 2001 Ximian, Inc.
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#include <math.h>

#include "color.h"

#ifndef MAX
#define MAX(a,b) ((b) > (a) ? (b) : (a))
#endif

#ifndef MIN
#define MIN(a,b) ((b) < (a) ? (b) : (a))
#endif

struct _SPColorSpace {
	unsigned char *name;
};

static const SPColorSpace RGB = {"RGB"};
static const SPColorSpace CMYK = {"CMYK"};

SPColorSpaceClass
sp_color_get_colorspace_class (const SPColor *color)
{
	if (!color) return SP_COLORSPACE_CLASS_INVALID;

	if (color->colorspace == &RGB) return SP_COLORSPACE_CLASS_PROCESS;
	if (color->colorspace == &CMYK) return SP_COLORSPACE_CLASS_PROCESS;

	return SP_COLORSPACE_CLASS_UNKNOWN;
}

SPColorSpaceType
sp_color_get_colorspace_type (const SPColor *color)
{
	if (!color) return SP_COLORSPACE_TYPE_INVALID;

	if (color->colorspace == &RGB) return SP_COLORSPACE_TYPE_RGB;
	if (color->colorspace == &CMYK) return SP_COLORSPACE_TYPE_CMYK;

	return SP_COLORSPACE_TYPE_UNKNOWN;
}

void
sp_color_copy (SPColor *dst, const SPColor *src)
{
	if (!dst || !src) return;

	*dst = *src;
}

unsigned int
sp_color_is_equal (SPColor *c0, SPColor *c1)
{
	if (!c0 || !c1) return 0;

	if (c0->colorspace != c1->colorspace) return 0;
	if (c0->v.c[0] != c1->v.c[0]) return 0;
	if (c0->v.c[1] != c1->v.c[1]) return 0;
	if (c0->v.c[2] != c1->v.c[2]) return 0;
	if ((c0->colorspace == &CMYK) && (c0->v.c[3] != c1->v.c[3])) return 0;

	return 1;
}

void
sp_color_set_rgb_float (SPColor *color, float r, float g, float b, unsigned int clamp, unsigned int denorm)
{
	if (!color) return;

	if (clamp) {
		if (r < 0) r = 0;
		if (r > 1) r = 1;
		if (g < 0) g = 0;
		if (g > 1) g = 1;
		if (b < 0) b = 0;
		if (b > 1) b = 1;
	} else if (!denorm) {
		if ((r < 0) || (r > 1) || (g < 0) || (g > 1) || (b < 0) || (b > 1)) return;
	}

	color->colorspace = &RGB;
	color->v.c[0] = r;
	color->v.c[1] = g;
	color->v.c[2] = b;
}

void
sp_color_set_rgb_rgba32 (SPColor *color, unsigned int value)
{
	if (!color) return;

	color->colorspace = &RGB;
	color->v.c[0] = (value >> 24) / 255.0F;
	color->v.c[1] = ((value >> 16) & 0xff) / 255.0F;
	color->v.c[2] = ((value >> 8) & 0xff) / 255.0F;
}

void
sp_color_set_cmyk_float (SPColor *color, float c, float m, float y, float k, unsigned int clamp, unsigned int denorm)
{
	if (!color) return;

	if (clamp) {
		if (c < 0) c = 0;
		if (c > 1) c = 1;
		if (m < 0) m = 0;
		if (m > 1) m = 1;
		if (y < 0) y = 0;
		if (y > 1) y = 1;
		if (k < 0) k = 0;
		if (k > 1) k = 1;
	} else if (!denorm) {
		if ((c < 0) || (c > 1) || (m < 0) || (m > 1) || (y < 0) || (y > 1) || (k < 0) || (k > 1)) return;
	}

	color->colorspace = &CMYK;
	color->v.c[0] = c;
	color->v.c[1] = m;
	color->v.c[2] = y;
	color->v.c[3] = k;
}

unsigned int
sp_color_get_rgba32_ualpha (const SPColor *color, unsigned int alpha)
{
	unsigned int rgba;

	if (!color) return 0;
	if (alpha > 0xff) return 0;

	if (color->colorspace == &RGB) {
		rgba = SP_RGBA32_U_COMPOSE (SP_COLOR_F_TO_U (color->v.c[0]), SP_COLOR_F_TO_U (color->v.c[1]), SP_COLOR_F_TO_U (color->v.c[2]), alpha);
	} else {
		float rgb[3];
		sp_color_get_rgb_floatv (color, rgb);
		rgba = SP_RGBA32_U_COMPOSE (SP_COLOR_F_TO_U (rgb[0]), SP_COLOR_F_TO_U (rgb[1]), SP_COLOR_F_TO_U (rgb[2]), alpha);
	}

	return rgba;
}

unsigned int
sp_color_get_rgba32_falpha (const SPColor *color, float alpha)
{
	if (!color) return 0;
	if ((alpha < 0) || (alpha > 1)) return 0;

	return sp_color_get_rgba32_ualpha (color, SP_COLOR_F_TO_U (alpha));
}

void
sp_color_get_rgb_floatv (const SPColor *color, float *rgb)
{
	if (!color) return;
	if (!rgb) return;

	if (color->colorspace == &RGB) {
		rgb[0] = color->v.c[0];
		rgb[1] = color->v.c[1];
		rgb[2] = color->v.c[2];
	} else if (color->colorspace == &CMYK) {
		sp_color_cmyk_to_rgb_floatv (rgb, color->v.c[0], color->v.c[1], color->v.c[2], color->v.c[3]);
	}
}

void
sp_color_get_cmyk_floatv (const SPColor *color, float *cmyk)
{
	if (!color) return;
	if (!cmyk) return;

	if (color->colorspace == &CMYK) {
		cmyk[0] = color->v.c[0];
		cmyk[1] = color->v.c[1];
		cmyk[2] = color->v.c[2];
		cmyk[3] = color->v.c[3];
	} else if (color->colorspace == &RGB) {
		sp_color_rgb_to_cmyk_floatv (cmyk, color->v.c[0], color->v.c[1], color->v.c[2]);
	}
}

/* Plain mode helpers */

void
sp_color_rgb_to_hsv_floatv (float *hsv, float r, float g, float b)
{
	float max, min, delta;

	max = MAX (MAX (r, g), b);
	min = MIN (MIN (r, g), b);
	delta = max - min;

	hsv[2] = max;

	if (max > 0) {
		hsv[1] = delta / max;
	} else {
		hsv[1] = 0.0;
	}

	if (hsv[1] != 0.0) {
		if (r == max) {
			hsv[0] = (g - b) / delta;
		} else if (g == max) {
			hsv[0] = 2.0f + (b - r) / delta;
		} else {
			hsv[0] = 4.0f + (r - g) / delta;
		}

		hsv[0] = hsv[0] / 6.0f;

		if (hsv[0] < 0) hsv[0] += 1.0f;
	}
}

void
sp_color_hsv_to_rgb_floatv (float *rgb, float h, float s, float v)
{
	float f, w, q, t, d;

	d = h * 5.99999999f;
	f = d - (float) floor (d);
	w = v * (1.0f - s);
	q = v * (1.0f - (s * f));
	t = v * (1.0f - (s * (1.0f - f)));

	if (d < 1.0f) {
		*rgb++ = v;
		*rgb++ = t;
		*rgb++ = w;
	} else if (d < 2.0f) {
		*rgb++ = q;
		*rgb++ = v;
		*rgb++ = w;
	} else if (d < 3.0f) {
		*rgb++ = w;
		*rgb++ = v;
		*rgb++ = t;
	} else if (d < 4.0f) {
		*rgb++ = w;
		*rgb++ = q;
		*rgb++ = v;
	} else if (d < 5.0f) {
		*rgb++ = t;
		*rgb++ = w;
		*rgb++ = v;
	} else {
		*rgb++ = v;
		*rgb++ = w;
		*rgb++ = q;
	}
}

void
sp_color_rgb_to_cmyk_floatv (float *cmyk, float r, float g, float b)
{
	float c, m, y, k, kd;

	c = 1.0f - r;
	m = 1.0f - g;
	y = 1.0f - b;
	k = MIN (MIN (c, m), y);

	c = c - k;
	m = m - k;
	y = y - k;

	kd = 1.0f - k;

	if (kd > 1e-9f) {
		c = c / kd;
		m = m / kd;
		y = y / kd;
	}

	cmyk[0] = c;
	cmyk[1] = m;
	cmyk[2] = y;
	cmyk[3] = k;
}

void
sp_color_cmyk_to_rgb_floatv (float *rgb, float c, float m, float y, float k)
{
	float kd;

	kd = 1.0f - k;

	c = c * kd;
	m = m * kd;
	y = y * kd;

	c = c + k;
	m = m + k;
	y = y + k;

	rgb[0] = 1.0f - c;
	rgb[1] = 1.0f - m;
	rgb[2] = 1.0f - y;
}




