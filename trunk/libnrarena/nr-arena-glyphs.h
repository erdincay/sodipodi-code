#ifndef __NR_ARENA_GLYPHS_H__
#define __NR_ARENA_GLYPHS_H__

/*
 * RGBA display list system for sodipodi
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 2002 Lauris Kaplinski
 *
 * Released under GNU GPL
 *
 */

#define NR_TYPE_ARENA_GLYPHS (nr_arena_glyphs_get_type ())
#define NR_ARENA_GLYPHS(obj) (ARIKKEI_CHECK_INSTANCE_CAST ((obj), NR_TYPE_ARENA_GLYPHS, NRArenaGlyphs))
#define NR_IS_ARENA_GLYPHS(obj) (ARIKKEI_CHECK_INSTANCE_TYPE ((obj), NR_TYPE_ARENA_GLYPHS))

typedef struct _NRArenaGlyphsClass NRArenaGlyphsClass;

#include <libnr/nr-svp.h>
#include <libnr/nr-render.h>

#include <libnrtype/nr-rasterfont.h>

#include <libnrarena/nr-arena-item.h>

struct _NRArenaGlyphs {
	NRArenaItem item;

	/* Glyphs data */
	NRPath *path;
	NRArenaStyle *style;
	NRMatrixF transform;
	NRFont *font;
	int glyph;

	NRRasterFont *rfont;
	float x, y;

	NRSVP *stroke_svp;
};

struct _NRArenaGlyphsClass {
	NRArenaItemClass parent_class;
};

unsigned int nr_arena_glyphs_get_type (void);

void nr_arena_glyphs_set_path (NRArenaGlyphs *glyphs,
			       NRPath *path, unsigned int private,
			       NRFont *font, int glyph,
			       const NRMatrixF *transform);
void nr_arena_glyphs_set_style (NRArenaGlyphs *glyphs, NRArenaStyle *style);

/* Integrated group of component glyphss */

typedef struct _NRArenaGlyphsGroup NRArenaGlyphsGroup;
typedef struct _NRArenaGlyphsGroupClass NRArenaGlyphsGroupClass;

#include "nr-arena-group.h"

#define NR_TYPE_ARENA_GLYPHS_GROUP (nr_arena_glyphs_group_get_type ())
#define NR_ARENA_GLYPHS_GROUP(obj) (ARIKKEI_CHECK_INSTANCE_CAST ((obj), NR_TYPE_ARENA_GLYPHS_GROUP, NRArenaGlyphsGroup))
#define NR_IS_ARENA_GLYPHS_GROUP(obj) (ARIKKEI_CHECK_INSTANCE_TYPE ((obj), NR_TYPE_ARENA_GLYPHS_GROUP))

struct _NRArenaGlyphsGroup {
	NRArenaGroup group;
	NRArenaStyle *style;
	NRRectF paintbox;
	/* State data */
	NRRenderer *fill_renderer;
	NRRenderer *stroke_renderer;
};

struct _NRArenaGlyphsGroupClass {
	NRArenaGroupClass parent_class;
};

unsigned int nr_arena_glyphs_group_get_type (void);

/* Utility functions */

void nr_arena_glyphs_group_clear (NRArenaGlyphsGroup *group);

void nr_arena_glyphs_group_add_component (NRArenaGlyphsGroup *group, NRFont *font, int glyph, const NRMatrixF *transform);

void nr_arena_glyphs_group_set_style (NRArenaGlyphsGroup *group, NRArenaStyle *style);

void nr_arena_glyphs_group_set_paintbox (NRArenaGlyphsGroup *group, const NRRectF *pbox);

#endif
