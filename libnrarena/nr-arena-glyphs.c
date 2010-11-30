#define __NR_ARENA_GLYPHS_C__

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


#include <math.h>
#include <string.h>

#include <libnr/nr-rect.h>
#include <libnr/nr-matrix.h>
#include <libnr/nr-svp.h>
#include <libnr/nr-stroke.h>
#include <libnr/nr-blit.h>
#include <libnr/nr-svp-render.h>

#include <libnr/nr-svp-private.h>

#include "nr-arena.h"
#include "nr-arena-style.h"
#include "nr-arena-painter.h"

#include "nr-arena-glyphs.h"

#define noAG_DEBUG_BBOX			/* Need to set stroke-style if you use this */

static void nr_arena_glyphs_class_init (NRArenaGlyphsClass *klass);
static void nr_arena_glyphs_init (NRArenaGlyphs *glyphs);
static void nr_arena_glyphs_finalize (ArikkeiObject *object);

static unsigned int nr_arena_glyphs_update (NRArenaItem *item, const NRRectL *area, const NRGC *gc, unsigned int state, unsigned int reset);
static unsigned int nr_arena_glyphs_clip (NRArenaItem *item, const NRRectL *area, NRPixBlock *pb);
static NRArenaItem *nr_arena_glyphs_pick (NRArenaItem *item, float x, float y, float delta, unsigned int sticky);

static NRArenaItemClass *glyphs_parent_class;

unsigned int
nr_arena_glyphs_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		type = arikkei_object_register_type (NR_TYPE_ARENA_ITEM,
						"NRArenaGlyphs",
						sizeof (NRArenaGlyphsClass),
						sizeof (NRArenaGlyphs),
						(void (*) (ArikkeiObjectClass *)) nr_arena_glyphs_class_init,
						(void (*) (ArikkeiObject *)) nr_arena_glyphs_init);
	}
	return type;
}

static void
nr_arena_glyphs_class_init (NRArenaGlyphsClass *klass)
{
	ArikkeiObjectClass *object_class;
	NRArenaItemClass *item_class;

	object_class = (ArikkeiObjectClass *) klass;
	item_class = (NRArenaItemClass *) klass;

	glyphs_parent_class = (NRArenaItemClass *) ((ArikkeiObjectClass *) klass)->parent;

	object_class->finalize = nr_arena_glyphs_finalize;

	item_class->update = nr_arena_glyphs_update;
	item_class->clip = nr_arena_glyphs_clip;
	item_class->pick = nr_arena_glyphs_pick;
}

static void
nr_arena_glyphs_init (NRArenaGlyphs *glyphs)
{
	/* Nothing here */
}

static void
nr_arena_glyphs_finalize (ArikkeiObject *object)
{
	NRArenaGlyphs *glyphs;

	glyphs = NR_ARENA_GLYPHS (object);

	if (glyphs->stroke_svp) {
		nr_svp_free (glyphs->stroke_svp);
	}

	if (glyphs->rfont) {
		glyphs->rfont = nr_rasterfont_unref (glyphs->rfont);
	}

	if (glyphs->font) {
		glyphs->font = nr_font_unref (glyphs->font);
	}

	if (glyphs->style) {
		nr_arena_style_unref (glyphs->style);
		glyphs->style = NULL;
	}

	if (glyphs->path) {
		free (glyphs->path);
	}

	((ArikkeiObjectClass *) glyphs_parent_class)->finalize (object);
}

static unsigned int
nr_arena_glyphs_update (NRArenaItem *item, const NRRectL *area, const NRGC *gc, unsigned int state, unsigned int reset)
{
	NRArenaGlyphs *glyphs;
	NRRasterFont *rfont;
	NRMatrixF t;
	NRRectF bbox;

	glyphs = NR_ARENA_GLYPHS (item);

	/* Request repaint old area if needed */
	/* fixme: Think about it a bit (Lauris) */
	if (!nr_rect_l_test_empty (&item->bbox)) {
		nr_arena_request_render_rect (item->arena, &item->bbox);
		nr_rect_l_set_empty (&item->bbox);
	}

	/* Release state data */
	if (glyphs->stroke_svp) {
		nr_svp_free (glyphs->stroke_svp);
		glyphs->stroke_svp = NULL;
	}

	if (!glyphs->font || !glyphs->path || !glyphs->style) return NR_ARENA_ITEM_STATE_ALL;
	if ((glyphs->style->fill.type == NR_ARENA_PAINT_NONE) && (glyphs->style->stroke.type == NR_ARENA_PAINT_NONE)) return NR_ARENA_ITEM_STATE_ALL;

	bbox.x0 = bbox.y0 = NR_HUGE_F;
	bbox.x1 = bbox.y1 = -NR_HUGE_F;

	if (glyphs->style->fill.type != NR_ARENA_PAINT_NONE) {
		NRRectF area;
		nr_matrix_multiply_fff (&t, &glyphs->transform, &gc->transform);
		rfont = nr_rasterfont_new (glyphs->font, &t);
		if (glyphs->rfont) glyphs->rfont = nr_rasterfont_unref (glyphs->rfont);
		glyphs->rfont = rfont;
		glyphs->x = t.c[4];
		glyphs->y = t.c[5];
		nr_rasterfont_glyph_area_get (rfont, glyphs->glyph, &area);
		bbox.x0 = area.x0 + glyphs->x;
		bbox.y0 = area.y0 + glyphs->y;
		bbox.x1 = area.x1 + glyphs->x;
		bbox.y1 = area.y1 + glyphs->y;
	}

	if (glyphs->style->stroke.type != NR_ARENA_PAINT_NONE) {
		float width, scale;
		NRSVL *svl;
		/* Build state data */
		scale = (float) NR_MATRIX_DF_EXPANSION (&gc->transform);
		width = MAX (0.125f, glyphs->style->stroke_width * scale);
		svl = nr_path_stroke (glyphs->path, &gc->transform, width,
				      glyphs->style->stroke_linejoin,
				      glyphs->style->stroke_linecap,
				      glyphs->style->stroke_miterlimit, 0.25);
		glyphs->stroke_svp = nr_svp_from_svl (svl, NULL);
		nr_svl_free_list (svl);
	}

	if (glyphs->stroke_svp) nr_svp_bbox (glyphs->stroke_svp, &bbox, FALSE);
	if (nr_rect_f_test_empty (&bbox)) return NR_ARENA_ITEM_STATE_ALL;
	item->bbox.x0 = (int) (bbox.x0 - 1.0f);
	item->bbox.y0 = (int) (bbox.y0 - 1.0f);
	item->bbox.x1 = (int) (bbox.x1 + 1.0f);
	item->bbox.y1 = (int) (bbox.y1 + 1.0f);
	nr_arena_request_render_rect (item->arena, &item->bbox);

	return NR_ARENA_ITEM_STATE_ALL;
}

static unsigned int
nr_arena_glyphs_clip (NRArenaItem *item, const NRRectL *area, NRPixBlock *pb)
{
	NRArenaGlyphs *glyphs;

	glyphs = NR_ARENA_GLYPHS (item);

	if (!glyphs->font || !glyphs->path) return item->state;

#if 0
	/* fixme: Implement */
	if (glyphs->fill_svp) {
		art_gray_svp_aa (glyphs->fill_svp, area->x0, area->y0, area->x1, area->y1, b->px, b->rs);
		b->empty = FALSE;
	}
#endif

	return item->state;
}

static NRArenaItem *
nr_arena_glyphs_pick (NRArenaItem *item, float x, float y, float delta, unsigned int sticky)
{
	NRArenaGlyphs *glyphs;

	glyphs = NR_ARENA_GLYPHS (item);

	if (!glyphs->font || !glyphs->path) return NULL;
	if (!glyphs->style) return NULL;

	/* fixme: */
	if ((x >= item->bbox.x0) && (y >= item->bbox.y0) && (x < item->bbox.x1) && (y < item->bbox.y1)) return item;

	if (glyphs->stroke_svp && (glyphs->style->stroke.type != NR_ARENA_PAINT_NONE)) {
		if (nr_svp_point_wind (glyphs->stroke_svp, (float) x, (float) y)) return item;
	}
	if (delta > 1e-3) {
		if (glyphs->stroke_svp && (glyphs->style->stroke.type != NR_ARENA_PAINT_NONE)) {
			if (nr_svp_point_distance (glyphs->stroke_svp, (float) x, (float) y) <= delta) return item;
		}
	}

	return NULL;
}

void
nr_arena_glyphs_set_path (NRArenaGlyphs *glyphs, NRPath *path, unsigned int private, NRFont *font, int glyph, const NRMatrixF *transform)
{
	arikkei_return_if_fail (glyphs != NULL);
	arikkei_return_if_fail (NR_IS_ARENA_GLYPHS (glyphs));

	nr_arena_item_request_render (NR_ARENA_ITEM (glyphs));

	if (glyphs->path) {
		free (glyphs->path);
		glyphs->path = NULL;
	}

	if (path) {
		if (transform) {
			glyphs->transform = *transform;
		} else {
			nr_matrix_f_set_identity (&glyphs->transform);
		}
		glyphs->path = nr_path_duplicate_transform (path, transform);
	}

	if (glyphs->font) glyphs->font = nr_font_unref (glyphs->font);
	if (font) glyphs->font = nr_font_ref (font);
	glyphs->glyph = glyph;

	nr_arena_item_request_update (NR_ARENA_ITEM (glyphs), NR_ARENA_ITEM_STATE_ALL, FALSE);
}

void
nr_arena_glyphs_set_style (NRArenaGlyphs *glyphs, NRArenaStyle *style)
{
	arikkei_return_if_fail (glyphs != NULL);
	arikkei_return_if_fail (NR_IS_ARENA_GLYPHS (glyphs));

	if (style) nr_arena_style_ref (style);
	if (glyphs->style) nr_arena_style_unref (glyphs->style);
	glyphs->style = style;

	nr_arena_item_request_update (NR_ARENA_ITEM (glyphs), NR_ARENA_ITEM_STATE_ALL, FALSE);
}

static unsigned int
nr_arena_glyphs_fill_mask (NRArenaGlyphs *glyphs, const NRRectL *area, NRPixBlock *m)
{
	NRArenaItem *item;

	/* fixme: area == m->area, so merge these */

	item = NR_ARENA_ITEM (glyphs);

	if (glyphs->rfont && nr_rect_l_test_intersect (area, &item->bbox)) {
		nr_rasterfont_glyph_mask_render (glyphs->rfont, glyphs->glyph, m, glyphs->x, glyphs->y);
	}

	return item->state;
}

static unsigned int
nr_arena_glyphs_stroke_mask (NRArenaGlyphs *glyphs, const NRRectL *area, NRPixBlock *m)
{
	NRArenaItem *item;

	item = NR_ARENA_ITEM (glyphs);

	if (glyphs->stroke_svp && nr_rect_l_test_intersect (area, &item->bbox)) {
		nr_pixblock_render_svp_mask_or (m, glyphs->stroke_svp);
		m->empty = FALSE;
	}

	return item->state;
}

static void nr_arena_glyphs_group_class_init (NRArenaGlyphsGroupClass *klass);
static void nr_arena_glyphs_group_init (NRArenaGlyphsGroup *group);
static void nr_arena_glyphs_group_finalize (ArikkeiObject *object);

static unsigned int nr_arena_glyphs_group_update (NRArenaItem *item, const NRRectL *area, const NRGC *gc, unsigned int state, unsigned int reset);
static unsigned int nr_arena_glyphs_group_render (NRArenaItem *item, const NRRectL *area, NRPixBlock *pb, unsigned int flags);
static unsigned int nr_arena_glyphs_group_clip (NRArenaItem *item, const NRRectL *area, NRPixBlock *pb);
static NRArenaItem *nr_arena_glyphs_group_pick (NRArenaItem *item, float x, float y, float delta, unsigned int sticky);

static NRArenaGroupClass *group_parent_class;

unsigned int
nr_arena_glyphs_group_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		type = arikkei_object_register_type (NR_TYPE_ARENA_GROUP,
						"NRArenaGlyphsGroup",
						sizeof (NRArenaGlyphsGroupClass),
						sizeof (NRArenaGlyphsGroup),
						(void (*) (ArikkeiObjectClass *)) nr_arena_glyphs_group_class_init,
						(void (*) (ArikkeiObject *)) nr_arena_glyphs_group_init);
	}
	return type;
}

static void
nr_arena_glyphs_group_class_init (NRArenaGlyphsGroupClass *klass)
{
	ArikkeiObjectClass *object_class;
	NRArenaItemClass *item_class;

	object_class = (ArikkeiObjectClass *) klass;
	item_class = (NRArenaItemClass *) klass;

	group_parent_class = (NRArenaGroupClass *) ((ArikkeiObjectClass *) klass)->parent;

	object_class->finalize = nr_arena_glyphs_group_finalize;

	item_class->update = nr_arena_glyphs_group_update;
	item_class->render = nr_arena_glyphs_group_render;
	item_class->clip = nr_arena_glyphs_group_clip;
	item_class->pick = nr_arena_glyphs_group_pick;
}

static void
nr_arena_glyphs_group_init (NRArenaGlyphsGroup *group)
{
	group->paintbox.x0 = group->paintbox.y0 = 0;
	group->paintbox.x1 = group->paintbox.y1 = 1;
}

static void
nr_arena_glyphs_group_finalize (ArikkeiObject *object)
{
	NRArenaGlyphsGroup *group;

	group = NR_ARENA_GLYPHS_GROUP (object);

	if (group->fill_renderer) {
		nr_renderer_delete (group->fill_renderer);
		group->fill_renderer = NULL;
	}

	if (group->stroke_renderer) {
		nr_renderer_delete (group->stroke_renderer);
		group->stroke_renderer = NULL;
	}

	if (group->style) {
		nr_arena_style_unref (group->style);
		group->style = NULL;
	}

		((ArikkeiObjectClass *) group_parent_class)->finalize (object);
}

static unsigned int
nr_arena_glyphs_group_update (NRArenaItem *item, const NRRectL *area, const NRGC *gc, unsigned int state, unsigned int reset)
{
	NRArenaGlyphsGroup *group;

	group = NR_ARENA_GLYPHS_GROUP (item);

	if (group->fill_renderer) {
		nr_renderer_delete (group->fill_renderer);
		group->fill_renderer = NULL;
	}

	if (group->stroke_renderer) {
		nr_renderer_delete (group->stroke_renderer);
		group->stroke_renderer = NULL;
	}

	item->render_opacity = TRUE;
	if (group->style->fill.type == NR_ARENA_PAINT_SERVER) {
		group->fill_renderer = nr_arena_style_get_renderer (group->style, &group->style->fill, &gc->transform, &group->paintbox);
		item->render_opacity = FALSE;
	}

	if (group->style->stroke.type == NR_ARENA_PAINT_SERVER) {
		group->stroke_renderer = nr_arena_style_get_renderer (group->style, &group->style->stroke, &gc->transform, &group->paintbox);
		item->render_opacity = FALSE;
	}

	if (((NRArenaItemClass *) group_parent_class)->update)
		return ((NRArenaItemClass *) group_parent_class)->update (item, area, gc, state, reset);

	return NR_ARENA_ITEM_STATE_ALL;
}

/* This sucks - as soon, as we have inheritable renderprops, do something with that opacity */

static unsigned int
nr_arena_glyphs_group_render (NRArenaItem *item, const NRRectL *area, NRPixBlock *pb, unsigned int flags)
{
	NRArenaGroup *group;
	NRArenaGlyphsGroup *ggroup;
	NRArenaItem *child;
	NRArenaStyle *style;
	unsigned int ret;

	group = NR_ARENA_GROUP (item);
	ggroup = NR_ARENA_GLYPHS_GROUP (item);
	style = ggroup->style;

	ret = item->state;

	/* Fill */
	if (style->fill.type != NR_ARENA_PAINT_NONE) {
		NRPixBlock mb;
		unsigned long rgba;
		nr_pixblock_setup_fast (&mb, NR_PIXBLOCK_MODE_G8, area->x0, area->y0, area->x1, area->y1, TRUE);
		/* Render children fill mask */
		for (child = group->children; child != NULL; child = child->next) {
			ret = nr_arena_glyphs_fill_mask (NR_ARENA_GLYPHS (child), area, &mb);
			if (!(ret & NR_ARENA_ITEM_STATE_RENDER)) {
				nr_pixblock_release (&mb);
				return ret;
			}
		}
		/* Composite into buffer */
		switch (style->fill.type) {
		case NR_ARENA_PAINT_COLOR:
			rgba = nr_arena_color_get_rgba (&style->fill.color, style->opacity);
			nr_blit_pixblock_mask_rgba32 (pb, &mb, rgba);
			pb->empty = FALSE;
			break;
		case NR_ARENA_PAINT_SERVER:
			if (ggroup->fill_renderer) {
				NRPixBlock cb;
				/* Need separate gradient buffer */
				nr_pixblock_setup_fast (&cb, NR_PIXBLOCK_MODE_R8G8B8A8N, area->x0, area->y0, area->x1, area->y1, TRUE);
				nr_renderer_render (ggroup->fill_renderer, &cb, NULL);
				cb.empty = FALSE;
				/* Composite */
				nr_blit_pixblock_pixblock_mask (pb, &cb, &mb);
				pb->empty = FALSE;
				nr_pixblock_release (&cb);
			}
			break;
		default:
			break;
		}
		nr_pixblock_release (&mb);
	}

	/* Stroke */
	if (style->stroke.type != NR_ARENA_PAINT_NONE) {
		NRPixBlock m;
		unsigned long rgba;
		nr_pixblock_setup_fast (&m, NR_PIXBLOCK_MODE_G8, area->x0, area->y0, area->x1, area->y1, TRUE);
		/* Render children stroke mask */
		for (child = group->children; child != NULL; child = child->next) {
			ret = nr_arena_glyphs_stroke_mask (NR_ARENA_GLYPHS (child), area, &m);
			if (!(ret & NR_ARENA_ITEM_STATE_RENDER)) {
				nr_pixblock_release (&m);
				return ret;
			}
		}
		/* Composite into buffer */
		switch (style->stroke.type) {
		case NR_ARENA_PAINT_COLOR:
			rgba = nr_arena_color_get_rgba (&style->stroke.color, style->opacity);
			nr_blit_pixblock_mask_rgba32 (pb, &m, rgba);
			pb->empty = FALSE;
			break;
		case NR_ARENA_PAINT_SERVER:
			if (ggroup->stroke_renderer) {
				NRPixBlock cb;
				/* Need separate gradient buffer */
				nr_pixblock_setup_fast (&cb, NR_PIXBLOCK_MODE_R8G8B8A8N, area->x0, area->y0, area->x1, area->y1, TRUE);
				nr_renderer_render (ggroup->stroke_renderer, &cb, NULL);
				cb.empty = FALSE;
				/* Composite */
				nr_blit_pixblock_pixblock_mask (pb, &cb, &m);
				pb->empty = FALSE;
				nr_pixblock_release (&cb);
			}
			break;
		default:
			break;
		}
		nr_pixblock_release (&m);
	}

	return ret;
}

static unsigned int
nr_arena_glyphs_group_clip (NRArenaItem *item, const NRRectL *area, NRPixBlock *pb)
{
	NRArenaGroup *group;
	NRArenaGlyphsGroup *ggroup;
	NRArenaItem *child;
	unsigned int ret;

	group = NR_ARENA_GROUP (item);
	ggroup = NR_ARENA_GLYPHS_GROUP (item);

	ret = item->state;

	/* Render children fill mask */
	for (child = group->children; child != NULL; child = child->next) {
		ret = nr_arena_glyphs_fill_mask (NR_ARENA_GLYPHS (child), area, pb);
		if (!(ret & NR_ARENA_ITEM_STATE_RENDER)) return ret;
	}

	return ret;
}

static NRArenaItem *
nr_arena_glyphs_group_pick (NRArenaItem *item, float x, float y, float delta, unsigned int sticky)
{
	NRArenaGroup *group;
	NRArenaItem *picked;

	group = NR_ARENA_GROUP (item);

	picked = NULL;

	if (((NRArenaItemClass *) group_parent_class)->pick)
		picked = ((NRArenaItemClass *) group_parent_class)->pick (item, x, y, delta, sticky);

	if (picked) picked = item;

	return picked;
}

void
nr_arena_glyphs_group_clear (NRArenaGlyphsGroup *sg)
{
	NRArenaGroup *group;

	group = NR_ARENA_GROUP (sg);

	nr_arena_item_request_render (NR_ARENA_ITEM (group));

	while (group->children) {
		nr_arena_item_remove_child (NR_ARENA_ITEM (group), group->children);
	}
}

void
nr_arena_glyphs_group_add_component (NRArenaGlyphsGroup *sg, NRFont *font, int glyph, const NRMatrixF *transform)
{
	NRArenaGroup *group;
	NRPath *path;

	group = NR_ARENA_GROUP (sg);

	path = nr_font_glyph_outline_get (font, glyph, FALSE);
	if (path) {
		NRArenaItem *new;
#ifdef AG_DEBUG_BBOX
		NRRectF area;
		NRPointF adv;
#endif
		nr_arena_item_request_render (NR_ARENA_ITEM (group));

#ifdef AG_DEBUG_BBOX
		if (curve && nr_font_glyph_area_get (font, glyph, &area)) {
			sp_curve_moveto (curve, area.x0, area.y0);
			sp_curve_lineto (curve, area.x1, area.y0);
			sp_curve_lineto (curve, area.x1, area.y1);
			sp_curve_lineto (curve, area.x0, area.y1);
			sp_curve_closepath (curve);
		}
		if (curve && nr_font_glyph_advance_get (font, glyph, &adv)) {
			sp_curve_moveto (curve, 0.0, 0.0);
			sp_curve_lineto (curve, adv.x, adv.y);
		}
#endif
		new = nr_arena_item_new (NR_ARENA_ITEM (group)->arena, NR_TYPE_ARENA_GLYPHS);
		nr_arena_item_append_child (NR_ARENA_ITEM (group), new);
		nr_arena_item_unref (new);
		nr_arena_glyphs_set_path (NR_ARENA_GLYPHS (new), path, FALSE, font, glyph, transform);
		nr_arena_glyphs_set_style (NR_ARENA_GLYPHS (new), sg->style);
	}

}

void
nr_arena_glyphs_group_set_style (NRArenaGlyphsGroup *sg, NRArenaStyle *style)
{
	NRArenaGroup *group;
	NRArenaItem *child;

	arikkei_return_if_fail (sg != NULL);
	arikkei_return_if_fail (NR_IS_ARENA_GLYPHS_GROUP (sg));

	group = NR_ARENA_GROUP (sg);

	if (style) nr_arena_style_ref (style);
	if (sg->style) nr_arena_style_unref (sg->style);
	sg->style = style;

	for (child = group->children; child != NULL; child = child->next) {
		arikkei_return_if_fail (NR_IS_ARENA_GLYPHS (child));
		nr_arena_glyphs_set_style (NR_ARENA_GLYPHS (child), sg->style);
	}

	nr_arena_item_request_update (NR_ARENA_ITEM (sg), NR_ARENA_ITEM_STATE_ALL, FALSE);
}

void
nr_arena_glyphs_group_set_paintbox (NRArenaGlyphsGroup *gg, const NRRectF *pbox)
{
	arikkei_return_if_fail (gg != NULL);
	arikkei_return_if_fail (NR_IS_ARENA_GLYPHS_GROUP (gg));
	arikkei_return_if_fail (pbox != NULL);

	if ((pbox->x0 < pbox->x1) && (pbox->y0 < pbox->y1)) {
		gg->paintbox.x0 = pbox->x0;
		gg->paintbox.y0 = pbox->y0;
		gg->paintbox.x1 = pbox->x1;
		gg->paintbox.y1 = pbox->y1;
	} else {
		/* fixme: We kill warning, although not sure what to do here (Lauris) */
		gg->paintbox.x0 = gg->paintbox.y0 = 0.0F;
		gg->paintbox.x1 = gg->paintbox.y1 = 256.0F;
	}

	nr_arena_item_request_update (NR_ARENA_ITEM (gg), NR_ARENA_ITEM_STATE_ALL, FALSE);
}

