#ifndef __NR_ARENA_STYLE_H__
#define __NR_ARENA_STYLE_H__

/*
 * RGBA display list system for sodipodi
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 2009 Lauris Kaplinski
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#include <libnr/nr-matrix.h>
#include <libnr/nr-rect.h>
#include <libnr/nr-render.h>

#include <libnrarena/nr-arena-forward.h>

struct _NRArenaColor {
	unsigned int rgba;
};

unsigned int nr_arena_color_get_rgba (NRArenaColor *color, float opacity);

enum { NR_ARENA_PAINT_NONE, NR_ARENA_PAINT_COLOR, NR_ARENA_PAINT_SERVER };


struct _NRArenaPaint {
	unsigned int type;
	union {
		NRArenaColor color;
		NRPaintServer *server;
	};
};

struct _NRArenaStyle {
	unsigned int refcount;
	NRArenaPaint fill;
	NRArenaPaint stroke;
	unsigned int fill_rule;
	float opacity;
	float stroke_width;
	unsigned int stroke_linejoin;
	unsigned int stroke_linecap;
	float stroke_miterlimit;
};

NRArenaStyle *nr_arena_style_new (void);

void nr_arena_style_ref (NRArenaStyle *style);
void nr_arena_style_unref (NRArenaStyle *style);

NRRenderer *nr_arena_style_get_renderer (NRArenaStyle *style, NRArenaPaint *paint, const NRMatrixF *transform, const NRRectF *paintbox);

#endif
