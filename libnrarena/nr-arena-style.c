#define __NR_ARENA_STYLE_CPP__

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

#include <malloc.h>
#include <string.h>

#include "nr-arena-style.h"

unsigned int
nr_arena_color_get_rgba (NRArenaColor *color, float opacity)
{
	return (color->rgba & 0xffffff00) | (unsigned int) ((color->rgba & 0xff) * opacity);
}

NRArenaStyle *
nr_arena_style_new (void)
{
	NRArenaStyle *style = (NRArenaStyle *) malloc (sizeof (NRArenaStyle));
	memset (style, 0, sizeof (NRArenaStyle));
	style->refcount = 1;

	return style;
}

void
nr_arena_style_ref (NRArenaStyle *style)
{
	style->refcount += 1;
}

void
nr_arena_style_unref (NRArenaStyle *style)
{
	style->refcount -= 1;

	if (!style->refcount) {
		if (style->fill.type == NR_ARENA_PAINT_SERVER) {
			arikkei_object_unref (ARIKKEI_OBJECT(style->fill.server));
		}
		if (style->stroke.type == NR_ARENA_PAINT_SERVER) {
			arikkei_object_unref (ARIKKEI_OBJECT(style->stroke.server));
		}
		free (style);
	}
}

NRRenderer *
nr_arena_style_get_renderer (NRArenaStyle *style, NRArenaPaint *paint, const NRMatrixF *transform, const NRRectF *paintbox)
{
	if (paint->type != NR_ARENA_PAINT_SERVER) return NULL;
	return nr_paint_server_get_renderer (paint->server, transform, paintbox);
}
