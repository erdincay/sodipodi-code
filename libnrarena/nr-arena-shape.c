#define __NR_ARENA_SHAPE_C__

/*
 * RGBA display list system for sodipodi
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
#include <string.h>
#include <libnr/nr-rect.h>
#include <libnr/nr-matrix.h>
#include <libnr/nr-path.h>
#include <libnr/nr-pathops.h>
#include <libnr/nr-pixops.h>
#include <libnr/nr-blit.h>
#include <libnr/nr-stroke.h>
#include <libnr/nr-svp-render.h>
#include <libnr/nr-pixblock-line.h>

#include <libnr/nr-svp-private.h>

#include "nr-arena.h"
#include "nr-arena-style.h"
#include "nr-arena-painter.h"

#include "nr-arena-shape.h"

static void nr_arena_shape_class_init (NRArenaShapeClass *klass);
static void nr_arena_shape_init (NRArenaShape *shape);
static void nr_arena_shape_finalize (ArikkeiObject *object);

static NRArenaItem *nr_arena_shape_children (NRArenaItem *item);
static void nr_arena_shape_add_child (NRArenaItem *item, NRArenaItem *child, NRArenaItem *ref);
static void nr_arena_shape_remove_child (NRArenaItem *item, NRArenaItem *child);
static void nr_arena_shape_set_child_position (NRArenaItem *item, NRArenaItem *child, NRArenaItem *ref);

static unsigned int nr_arena_shape_update (NRArenaItem *item, const NRRectL *area, const NRGC *gc, unsigned int state, unsigned int reset);
static unsigned int nr_arena_shape_render (NRArenaItem *item, const NRRectL *area, NRPixBlock *pb, unsigned int flags);
static unsigned int nr_arena_shape_clip (NRArenaItem *item, const NRRectL *area, NRPixBlock *pb);
static NRArenaItem *nr_arena_shape_pick (NRArenaItem *item, float x, float y, float delta, unsigned int sticky);

static NRArenaItemClass *shape_parent_class;

unsigned int
nr_arena_shape_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		type = arikkei_object_register_type (NR_TYPE_ARENA_ITEM,
						"NRArenaShape",
						sizeof (NRArenaShapeClass),
						sizeof (NRArenaShape),
						(void (*) (ArikkeiObjectClass *)) nr_arena_shape_class_init,
						(void (*) (ArikkeiObject *)) nr_arena_shape_init);
	}
	return type;
}

static void
nr_arena_shape_class_init (NRArenaShapeClass *klass)
{
	ArikkeiObjectClass *object_class;
	NRArenaItemClass *item_class;

	object_class = (ArikkeiObjectClass *) klass;
	item_class = (NRArenaItemClass *) klass;

	shape_parent_class = (NRArenaItemClass *)  ((ArikkeiObjectClass *) klass)->parent;

	object_class->finalize = nr_arena_shape_finalize;

	item_class->children = nr_arena_shape_children;
	item_class->add_child = nr_arena_shape_add_child;
	item_class->set_child_position = nr_arena_shape_set_child_position;
	item_class->remove_child = nr_arena_shape_remove_child;
	item_class->update = nr_arena_shape_update;
	item_class->render = nr_arena_shape_render;
	item_class->clip = nr_arena_shape_clip;
	item_class->pick = nr_arena_shape_pick;
}

static void
nr_arena_shape_init (NRArenaShape *shape)
{
	shape->paintbox.x0 = shape->paintbox.y0 = 0;
	shape->paintbox.x1 = shape->paintbox.y1 = 256;
	nr_matrix_f_set_identity (&shape->ctm);
}

static void
nr_arena_shape_finalize (ArikkeiObject *object)
{
	NRArenaItem *item;
	NRArenaShape *shape;

	item = (NRArenaItem *) object;
	shape = (NRArenaShape *) (object);

	while (shape->markers) {
		shape->markers = nr_arena_item_detach_unref (item, shape->markers);
	}

	if (shape->path) free (shape->path);
	if (shape->fill_svp) nr_svp_free (shape->fill_svp);
	if (shape->stroke_svp) nr_svp_free (shape->stroke_svp);
	if (shape->fill_renderer) nr_renderer_delete (shape->fill_renderer);
	if (shape->stroke_renderer) nr_renderer_delete (shape->stroke_renderer);
	if (shape->style) nr_arena_style_unref (shape->style);

	((ArikkeiObjectClass *) shape_parent_class)->finalize (object);
}

static NRArenaItem *
nr_arena_shape_children (NRArenaItem *item)
{
	NRArenaShape *shape;

	shape = (NRArenaShape *) item;

	return shape->markers;
}

static void
nr_arena_shape_add_child (NRArenaItem *item, NRArenaItem *child, NRArenaItem *ref)
{
	NRArenaShape *shape;

	shape = (NRArenaShape *) item;

	if (!ref) {
		shape->markers = nr_arena_item_attach_ref (item, child, NULL, shape->markers);
	} else {
		ref->next = nr_arena_item_attach_ref (item, child, ref, ref->next);
	}

	nr_arena_item_request_update (item, NR_ARENA_ITEM_STATE_ALL, FALSE);
}

static void
nr_arena_shape_remove_child (NRArenaItem *item, NRArenaItem *child)
{
	NRArenaShape *shape;

	shape = (NRArenaShape *) item;

	if (child->prev) {
		nr_arena_item_detach_unref (item, child);
	} else {
		shape->markers = nr_arena_item_detach_unref (item, child);
	}

	nr_arena_item_request_update (item, NR_ARENA_ITEM_STATE_ALL, FALSE);
}

static void
nr_arena_shape_set_child_position (NRArenaItem *item, NRArenaItem *child, NRArenaItem *ref)
{
	NRArenaShape *shape;

	shape = (NRArenaShape *) item;

	nr_arena_item_ref (child);

	if (child->prev) {
		nr_arena_item_detach_unref (item, child);
	} else {
		shape->markers = nr_arena_item_detach_unref (item, child);
	}

	if (!ref) {
		shape->markers = nr_arena_item_attach_ref (item, child, NULL, shape->markers);
	} else {
		ref->next = nr_arena_item_attach_ref (item, child, ref, ref->next);
	}

	nr_arena_item_unref (child);

	nr_arena_item_request_render (child);
}

static unsigned int
nr_arena_shape_update (NRArenaItem *item, const NRRectL *area, const NRGC *gc, unsigned int state, unsigned int reset)
{
	NRArenaShape *shape;
	NRArenaItem *child;
	NRArenaStyle *style;
	NRRectF bbox;
	unsigned int newstate, beststate;

	shape = NR_ARENA_SHAPE (item);
	style = shape->style;

	beststate = NR_ARENA_ITEM_STATE_ALL;

	for (child = shape->markers; child != NULL; child = child->next) {
		newstate = nr_arena_item_invoke_update (child, area, gc, state, reset);
		beststate = beststate & newstate;
	}

	if (!(state & NR_ARENA_ITEM_STATE_RENDER)) {
		/* We do not have to create rendering structures */
		shape->ctm = gc->transform;
		if (state & NR_ARENA_ITEM_STATE_BBOX) {
			if (shape->path) {
				/* fixme: */
				bbox.x0 = bbox.y0 = NR_HUGE_F;
				bbox.x1 = bbox.y1 = -NR_HUGE_F;
				nr_path_matrix_f_bbox_f_union (shape->path, &gc->transform, &bbox, 1.0);
				item->bbox.x0 = (int) (bbox.x0 - 1.0f);
				item->bbox.y0 = (int) (bbox.y0 - 1.0f);
				item->bbox.x1 = (int) (bbox.x1 + 1.9999f);
				item->bbox.y1 = (int) (bbox.y1 + 1.9999f);
			}
			if (beststate & NR_ARENA_ITEM_STATE_BBOX) {
				for (child = shape->markers; child != NULL; child = child->next) {
					nr_rect_l_union (&item->bbox, &item->bbox, &child->bbox);
				}
			}
		}
		return (state | item->state);
	}

	/* Request repaint old area if needed */
	/* fixme: Think about it a bit (Lauris) */
	/* fixme: Thios is only needed, if actually rendered/had svp (Lauris) */
	if (!nr_rect_l_test_empty (&item->bbox)) {
		nr_arena_request_render_rect (item->arena, &item->bbox);
		nr_rect_l_set_empty (&item->bbox);
	}

	/* Release state data */
	if (TRUE || !nr_matrix_d_test_transform_equal (&gc->transform, &shape->ctm, NR_EPSILON_D)) {
		/* Concept test */
		if (shape->fill_svp) {
			nr_svp_free (shape->fill_svp);
			shape->fill_svp = NULL;
		}
	}
	if (shape->stroke_svp) {
		nr_svp_free (shape->stroke_svp);
		shape->stroke_svp = NULL;
	}
	if (shape->fill_renderer) {
		nr_renderer_delete (shape->fill_renderer);
		shape->fill_renderer = NULL;
	}
	if (shape->stroke_renderer) {
		nr_renderer_delete (shape->stroke_renderer);
		shape->stroke_renderer = NULL;
	}

	if (!shape->path || !shape->style) return NR_ARENA_ITEM_STATE_ALL;
	if (nr_path_is_empty (shape->path)) return NR_ARENA_ITEM_STATE_ALL;
	if ((shape->style->fill.type == NR_ARENA_PAINT_NONE) && (shape->style->stroke.type == NR_ARENA_PAINT_NONE)) return NR_ARENA_ITEM_STATE_ALL;

	/* Build state data */
	shape->ctm = gc->transform;

	if (shape->style->fill.type != NR_ARENA_PAINT_NONE) {
		if (nr_path_is_shape (shape->path)) {
			NRSVL *svl;
			svl = nr_svl_from_path (shape->path, &gc->transform, shape->style->fill_rule, TRUE, 0.25);
			shape->fill_svp = nr_svp_from_svl (svl, NULL);
			nr_svl_free_list (svl);
		}
	}

	/* fixme: Implement stroke (Lauris) */
	if (shape->style->stroke.type != NR_ARENA_PAINT_NONE) {
		float scale, width;
		NRSVL *svl;
		scale = (float) NR_MATRIX_DF_EXPANSION (&gc->transform);
		width = MAX (0.125f, shape->style->stroke_width * scale);
		svl = nr_path_stroke (shape->path, &gc->transform, width, shape->style->stroke_linecap, shape->style->stroke_linejoin, shape->style->stroke_miterlimit, 0.25);
		shape->stroke_svp = nr_svp_from_svl (svl, NULL);
		nr_svl_free_list (svl);
	}
#if 0
	if (shape->style->stroke.type != NR_ARENA_PAINT_NONE) {
		float width, scale;
		NRSVL *svl;
		scale = (float) NR_MATRIX_DF_EXPANSION (&gc->transform);
		width = MAX (0.125, style->stroke_width * scale);

		bp.path = art_bpath_affine_transform (shape->curve->bpath, NR_MATRIX_D_TO_DOUBLE (&gc->transform));

		if (!style->stroke_dash.n_dash) {
			svl = nr_bpath_stroke (&bp, NULL, width,
					       shape->style->stroke_linecap.value,
					       shape->style->stroke_linejoin.value,
					       shape->style->stroke_miterlimit.value * M_PI / 180.0,
					       0.25);
		} else {
			double dlen;
			int i;
			ArtVpath *vp, *pvp;
			/* ArtSVP *asvp; */
			vp = art_bez_path_to_vec (bp.path, 0.25);
			pvp = art_vpath_perturb (vp);
			art_free (vp);
			dlen = 0.0;
			for (i = 0; i < style->stroke_dash.n_dash; i++) dlen += style->stroke_dash.dash[i] * scale;
			if (dlen >= 1.0) {
				ArtVpathDash dash;
				int i;
				dash.offset = style->stroke_dash.offset * scale;
				dash.n_dash = style->stroke_dash.n_dash;
				dash.dash = g_new (double, dash.n_dash);
				for (i = 0; i < dash.n_dash; i++) {
					dash.dash[i] = style->stroke_dash.dash[i] * scale;
				}
				vp = art_vpath_dash (pvp, &dash);
				art_free (pvp);
				pvp = vp;
				g_free (dash.dash);
			}
#if 0
			asvp = art_svp_vpath_stroke (pvp,
						     shape->style->stroke_linejoin.value,
						     shape->style->stroke_linecap.value,
						     width,
						     shape->style->stroke_miterlimit.value, 0.25);
			art_free (pvp);
			svl = nr_svl_from_art_svp (asvp);
			art_svp_free (asvp);
#else
			svl = nr_vpath_stroke (pvp, NULL, width,
					       shape->style->stroke_linecap.value,
					       shape->style->stroke_linejoin.value,
					       shape->style->stroke_miterlimit.value * M_PI / 180.0,
					       0.25);
			art_free (pvp);
#endif
		}
		shape->stroke_svp = nr_svp_from_svl (svl, NULL);
		nr_svl_free_list (svl);
		art_free (bp.path);
	}
#endif

	bbox.x0 = bbox.y0 = bbox.x1 = bbox.y1 = 0.0;
	if (shape->stroke_svp && shape->stroke_svp->length > 0) {
		nr_svp_bbox (shape->stroke_svp, &bbox, FALSE);
	}
	if (shape->fill_svp && shape->fill_svp->length > 0) {
		nr_svp_bbox (shape->fill_svp, &bbox, FALSE);
	}
	if (nr_rect_f_test_empty (&bbox)) return NR_ARENA_ITEM_STATE_ALL;

	item->bbox.x0 = (int) (bbox.x0 - 1.0f);
	item->bbox.y0 = (int) (bbox.y0 - 1.0);
	item->bbox.x1 = (int) (bbox.x1 + 1.0);
	item->bbox.y1 = (int) (bbox.y1 + 1.0);
	nr_arena_request_render_rect (item->arena, &item->bbox);

	item->render_opacity = TRUE;
	if (shape->style->fill.type == NR_ARENA_PAINT_SERVER) {
		shape->fill_renderer = nr_arena_style_get_renderer (shape->style, &shape->style->fill, &gc->transform, &shape->paintbox);
		item->render_opacity = FALSE;
	}
	if (shape->style->stroke.type == NR_ARENA_PAINT_SERVER) {
		shape->stroke_renderer = nr_arena_style_get_renderer (shape->style, &shape->style->stroke, &gc->transform, &shape->paintbox);
		item->render_opacity = FALSE;
	}

	if (beststate & NR_ARENA_ITEM_STATE_BBOX) {
		for (child = shape->markers; child != NULL; child = child->next) {
			nr_rect_l_union (&item->bbox, &item->bbox, &child->bbox);
		}
	}

	return NR_ARENA_ITEM_STATE_ALL;
}

static unsigned int
nr_arena_shape_render (NRArenaItem *item, const NRRectL *area, NRPixBlock *pb, unsigned int flags)
{
	NRArenaShape *shape;
	NRArenaItem *child;
	NRArenaStyle *style;

	shape = NR_ARENA_SHAPE (item);

	if (!shape->path) return item->state;
	if (!shape->style) return item->state;

	style = shape->style;

	if (0 || (flags & NR_ARENA_ITEM_RENDER_WIREFRAME)) {
		if (shape->path) {
			nr_pixblock_draw_path_rgba32 (pb, shape->path, &shape->ctm, 0x000000ff);
			pb->empty = FALSE;
		}
		return item->state;
	}

	if (shape->fill_svp) {
		NRPixBlock m;
		unsigned long rgba;

		nr_pixblock_setup_fast (&m, NR_PIXBLOCK_MODE_G8, area->x0, area->y0, area->x1, area->y1, TRUE);
		nr_pixblock_render_svp_mask_or (&m, shape->fill_svp);
		m.empty = FALSE;

		switch (style->fill.type) {
		case NR_ARENA_PAINT_COLOR:
			rgba = nr_arena_color_get_rgba (&style->fill.color, style->opacity);
			nr_blit_pixblock_mask_rgba32 (pb, &m, rgba);
			pb->empty = FALSE;
			break;
		case NR_ARENA_PAINT_SERVER:
			if (shape->fill_renderer) {
				NRPixBlock cb;
				/* Need separate gradient buffer */
				/* fixme: We have to port gradient rendering to premultiplied alpha (Lauris) */
				/* fixme: Or alternately write MMX enhanced blitter for this case */
				nr_pixblock_setup_fast (&cb, NR_PIXBLOCK_MODE_R8G8B8A8N, area->x0, area->y0, area->x1, area->y1, TRUE);
				nr_renderer_render (shape->fill_renderer, &cb, NULL);
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

	if (shape->stroke_svp) {
		NRPixBlock m;
		unsigned long rgba;

		nr_pixblock_setup_fast (&m, NR_PIXBLOCK_MODE_G8, area->x0, area->y0, area->x1, area->y1, TRUE);
		nr_pixblock_render_svp_mask_or (&m, shape->stroke_svp);
		m.empty = FALSE;

		switch (style->stroke.type) {
		case NR_ARENA_PAINT_COLOR:
			rgba = nr_arena_color_get_rgba (&style->stroke.color, style->opacity);
			nr_blit_pixblock_mask_rgba32 (pb, &m, rgba);
			pb->empty = FALSE;
			break;
		case NR_ARENA_PAINT_SERVER:
			if (shape->stroke_renderer) {
				NRPixBlock cb;
				/* Need separate gradient buffer */
				nr_pixblock_setup_fast (&cb, NR_PIXBLOCK_MODE_R8G8B8A8N, area->x0, area->y0, area->x1, area->y1, TRUE);
				nr_renderer_render (shape->stroke_renderer, &cb, NULL);
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

	/* Just compose children into parent buffer */
	for (child = shape->markers; child != NULL; child = child->next) {
		unsigned int ret;
		ret = nr_arena_item_invoke_render (child, area, pb, flags);
		if (ret & NR_ARENA_ITEM_STATE_INVALID) return ret;
	}

	return item->state;
}

static unsigned int
nr_arena_shape_clip (NRArenaItem *item, const NRRectL *area, NRPixBlock *pb)
{
	NRArenaShape *shape;

	shape = NR_ARENA_SHAPE (item);

	if (!shape->path) return item->state;

	if (shape->fill_svp) {
		nr_pixblock_render_svp_mask_or (pb, shape->fill_svp);
		pb->empty = FALSE;
	}

	return item->state;
}

static NRArenaItem *
nr_arena_shape_pick (NRArenaItem *item, float x, float y, float delta, unsigned int sticky)
{
	NRArenaShape *shape;

	shape = NR_ARENA_SHAPE (item);

	if (!shape->path) return NULL;
	if (!shape->style) return NULL;

	if (item->state & NR_ARENA_ITEM_STATE_RENDER) {
		if (shape->fill_svp && (shape->style->fill.type != NR_ARENA_PAINT_NONE)) {
			if (nr_svp_point_wind (shape->fill_svp, (float) x, (float) y)) return item;
		}
		if (shape->stroke_svp && (shape->style->stroke.type != NR_ARENA_PAINT_NONE)) {
			if (nr_svp_point_wind (shape->stroke_svp, (float) x, (float) y)) return item;
		}
		if (delta > 1e-3) {
			if (shape->fill_svp && (shape->style->fill.type != NR_ARENA_PAINT_NONE)) {
				if (nr_svp_point_distance (shape->fill_svp, (float) x, (float) y) <= delta) return item;
			}
			if (shape->stroke_svp && (shape->style->stroke.type != NR_ARENA_PAINT_NONE)) {
				if (nr_svp_point_distance (shape->stroke_svp, (float) x, (float) y) <= delta) return item;
			}
		}
	} else {
		float dist;
		int wind;
		wind = 0;
		dist = nr_path_matrix_f_wind_distance (shape->path, &shape->ctm, (float) x, (float) y, &wind, NR_EPSILON_F);
		if (shape->style->fill.type != NR_ARENA_PAINT_NONE) {
			if (!shape->style->fill_rule) {
				if (wind != 0) return item;
			} else {
				if (wind & 0x1) return item;
			}
		}
		if (shape->style->stroke.type != NR_ARENA_PAINT_NONE) {
			/* fixme: We do not take stroke width into account here (Lauris) */
			if (dist < delta) return item;
		}
	}

	return NULL;
}

void
nr_arena_shape_set_path (NRArenaShape *shape, NRPath *path, unsigned int makecopy, const double *affine)
{
	arikkei_return_if_fail (shape != NULL);
	arikkei_return_if_fail (NR_IS_ARENA_SHAPE (shape));

	nr_arena_item_request_render (NR_ARENA_ITEM (shape));

	if (shape->path) {
		free (shape->path);
		shape->path = NULL;
	}

	if (path) {
		if (affine) {
			NRMatrixF t;
			nr_matrix_f_from_d (&t, (NRMatrixD *) affine);
			shape->path = nr_path_duplicate_transform (path, &t);
			if (!makecopy) {
				free (path);
			}
		} else {
			if (makecopy) {
				shape->path = nr_path_duplicate (path, 0);
			} else {
				shape->path = path;
			}
		}
	}

	nr_arena_item_request_update (NR_ARENA_ITEM (shape), NR_ARENA_ITEM_STATE_ALL, FALSE);
}

void
nr_arena_shape_set_style (NRArenaShape *shape, NRArenaStyle *style)
{
	arikkei_return_if_fail (shape != NULL);
	arikkei_return_if_fail (NR_IS_ARENA_SHAPE (shape));

	if (style) nr_arena_style_ref (style);
	if (shape->style) nr_arena_style_unref (shape->style);
	shape->style = style;

	nr_arena_item_request_update (NR_ARENA_ITEM (shape), NR_ARENA_ITEM_STATE_ALL, FALSE);
}

void
nr_arena_shape_set_paintbox (NRArenaShape *shape, const NRRectF *pbox)
{
	arikkei_return_if_fail (shape != NULL);
	arikkei_return_if_fail (NR_IS_ARENA_SHAPE (shape));
	arikkei_return_if_fail (pbox != NULL);

	if ((pbox->x0 < pbox->x1) && (pbox->y0 < pbox->y1)) {
		shape->paintbox = *pbox;
	} else {
		/* fixme: We kill warning, although not sure what to do here (Lauris) */
		shape->paintbox.x0 = shape->paintbox.y0 = 0.0F;
		shape->paintbox.x1 = shape->paintbox.y1 = 256.0F;
	}

	nr_arena_item_request_update (NR_ARENA_ITEM (shape), NR_ARENA_ITEM_STATE_ALL, FALSE);
}

