#define __SP_SHAPE_C__

/*
 * Base class for shapes, including <path> element
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 1999-2002 Lauris Kaplinski
 * Copyright (C) 2000-2001 Ximian, Inc.
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libnrarena/nr-arena-shape.h>

#include <string.h>

#include <libnr/nr-pixblock.h>
#include <libnr/nr-stroke.h>

#include "macros.h"
#include "attributes.h"
#include "uri-references.h"
#include "document.h"

#if 0
#include "helper/sp-intl.h"
#include "xml/node-private.h"
#include "svg/svg.h"
#include "display/nr-arena-shape.h"
#include "print.h"
#include "document.h"
#include "desktop.h"
#include "selection.h"
#include "desktop-handles.h"
#include "sp-paint-server.h"
#include "sp-root.h"
#endif

#include "style.h"
#include "sp-marker.h"

#include "sp-shape.h"

#define noSHAPE_VERBOSE

static void sp_shape_class_init (SPShapeClass *class);
static void sp_shape_init (SPShape *shape);

static void sp_shape_build (SPObject *object, SPDocument *document, TheraNode *node);
static void sp_shape_release (SPObject *object);

static void sp_shape_update (SPObject *object, SPCtx *ctx, unsigned int flags);
static void sp_shape_modified (SPObject *object, unsigned int flags);

static void sp_shape_bbox (SPItem *item, NRRectF *bbox, const NRMatrixF *transform, unsigned int flags);
void sp_shape_print (SPItem * item, SPPrintContext * ctx);
static NRArenaItem *sp_shape_show (SPItem *item, NRArena *arena, unsigned int key, unsigned int flags);
static void sp_shape_hide (SPItem *item, unsigned int key);

static void sp_shape_update_marker_view (SPShape *shape, NRArenaItem *ai);

static SPShapeClass *parent_class;

GType
sp_shape_get_type (void)
{
	static GType type = 0;
	if (!type) {
		GTypeInfo info = {
			sizeof (SPShapeClass),
			NULL, NULL,
			(GClassInitFunc) sp_shape_class_init,
			NULL, NULL,
			sizeof (SPShape),
			16,
			(GInstanceInitFunc) sp_shape_init,
		};
		type = g_type_register_static (SP_TYPE_ITEM, "SPShape", &info, 0);
	}
	return type;
}

static void
sp_shape_class_init (SPShapeClass *klass)
{
	SPObjectClass *sp_object_class;
	SPItemClass * item_class;
	SPPathClass * path_class;

	sp_object_class = (SPObjectClass *) klass;
	item_class = (SPItemClass *) klass;
	path_class = (SPPathClass *) klass;

	parent_class = g_type_class_peek_parent (klass);

	sp_object_class->build = sp_shape_build;
	sp_object_class->release = sp_shape_release;
	sp_object_class->update = sp_shape_update;
	sp_object_class->modified = sp_shape_modified;

	item_class->bbox = sp_shape_bbox;
	item_class->print = sp_shape_print;
	item_class->show = sp_shape_show;
	item_class->hide = sp_shape_hide;
}

static void
sp_shape_init (SPShape *shape)
{
	/* Nothing here */
}

static void
sp_shape_build (SPObject *object, SPDocument *document, TheraNode *node)
{
	((SPObjectClass *) (parent_class))->build (object, document, node);
}

static void
sp_shape_release (SPObject *object)
{
	SPItem *item;
	SPShape *shape;
	SPItemView *v;

	item = (SPItem *) object;
	shape = (SPShape *) object;

	if (shape->marker_start) {
		sp_signal_disconnect_by_data (shape->marker_start, object);
		for (v = item->display; v != NULL; v = v->view.next) {
			sp_marker_hide ((SPMarker *) shape->marker_start, NR_ARENA_ITEM_GET_KEY (v));
		}
		shape->marker_start = sp_object_hunref (shape->marker_start, object);
	}
	if (shape->marker_mid) {
		sp_signal_disconnect_by_data (shape->marker_mid, object);
		for (v = item->display; v != NULL; v = v->view.next) {
			sp_marker_hide ((SPMarker *) shape->marker_mid, NR_ARENA_ITEM_GET_KEY (v));
		}
		shape->marker_mid = sp_object_hunref (shape->marker_mid, object);
	}
	if (shape->marker_end) {
		sp_signal_disconnect_by_data (shape->marker_end, object);
		for (v = item->display; v != NULL; v = v->view.next) {
			sp_marker_hide ((SPMarker *) shape->marker_end, NR_ARENA_ITEM_GET_KEY (v));
		}
		shape->marker_end = sp_object_hunref (shape->marker_end, object);
	}
	if (shape->curve) {
		free (shape->curve);
		shape->curve = NULL;
	}

	((SPObjectClass *) parent_class)->release (object);
}

static void
sp_shape_update (SPObject *object, SPCtx *ctx, unsigned int flags)
{
	SPItem *item;
	SPShape *shape;

	item = (SPItem *) object;
	shape = (SPShape *) object;

	((SPObjectClass *) (parent_class))->update (object, ctx, flags);

	if (flags & (SP_OBJECT_STYLE_MODIFIED_FLAG | SP_OBJECT_VIEWPORT_MODIFIED_FLAG)) {
		SPStyle *style;
		style = SP_OBJECT_STYLE (object);
		if (style->stroke_width.unit == SP_CSS_UNIT_PERCENT) {
			SPItemCtx *ictx;
			SPItemView *v;
			double aw;
			ictx = (SPItemCtx *) ctx;
			aw = 1.0 / NR_MATRIX_DF_EXPANSION (&ictx->i2vp);
			style->stroke_width.computed = style->stroke_width.value * (float) aw;
			for (v = ((SPItem *) (shape))->display; v != NULL; v = v->view.next) {
				NRArenaStyle *arenastyle = sp_style_get_arena_style (style);
				nr_arena_shape_set_style ((NRArenaShape *) v, arenastyle);
			}
		}
	}

	if (flags & (SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_PARENT_MODIFIED_FLAG)) {
		SPItemView *v;
		NRRectF paintbox;
		/* This is suboptimal, because changing parent style schedules recalculation */
		/* But on the other hand - how can we know that parent does not tie style and transform */
		sp_item_invoke_bbox (SP_ITEM (object), &paintbox, NULL, TRUE);
		for (v = SP_ITEM (shape)->display; v != NULL; v = v->view.next) {
			if (flags & SP_OBJECT_MODIFIED_FLAG) {
				nr_arena_shape_set_path (NR_ARENA_SHAPE (v), shape->curve, TRUE, NULL);
			}
			nr_arena_shape_set_paintbox (NR_ARENA_SHAPE (v), &paintbox);
		}
	}

#if 0
	if (shape->curve && (shape->marker_start || shape->marker_mid || shape->marker_end)) {
		SPItemView *v;
		ArtBpath *bp;
		int nstart, nmid, nend;
		/* Determine the number of markers needed */
		nstart = 0;
		nmid = 0;
		nend = 0;
		for (bp = shape->curve->bpath; bp->code != ART_END; bp++) {
			if ((bp[0].code == ART_MOVETO) || (bp[0].code == ART_MOVETO_OPEN)) {
				nstart += 1;
			} else if ((bp[1].code != ART_LINETO) && (bp[1].code != ART_CURVETO)) {
				nend += 1;
			} else {
				nmid += 1;
			}
		}
		/* Dimension marker views */
		for (v = item->display; v != NULL; v = v->view.next) {
			if (!v->key) NR_ARENA_ITEM_SET_KEY (v, sp_item_display_key_new (3));
			if (shape->marker_start) sp_marker_show_dimension ((SPMarker *) shape->marker_start,
									   NR_ARENA_ITEM_GET_KEY (v) + SP_MARKER_START,
									   nstart);
			if (shape->marker_mid) sp_marker_show_dimension ((SPMarker *) shape->marker_mid,
									 NR_ARENA_ITEM_GET_KEY (v) + SP_MARKER_MID,
									 nmid);
			if (shape->marker_end) sp_marker_show_dimension ((SPMarker *) shape->marker_end,
									 NR_ARENA_ITEM_GET_KEY (v) + SP_MARKER_END,
									 nend);
		}
		/* Update marker views */
		for (v = item->display; v != NULL; v = v->view.next) {
			sp_shape_update_marker_view (shape, v);
		}
	}
#endif
}

/* Marker views have to be scaled already */

static void
sp_shape_update_marker_view (SPShape *shape, NRArenaItem *ai)
{
#if 0
	SPStyle *style;
	int nstart, nmid, nend;
	ArtBpath *bp;

	style = ((SPObject *) shape)->style;

	nstart = 0;
	nmid = 0;
	nend = 0;
	for (bp = shape->curve->bpath; bp->code != ART_END; bp++) {
		NRMatrixF m;
		if (shape->marker_start && ((bp->code == ART_MOVETO) || (bp->code == ART_MOVETO_OPEN))) {
			float dx, dy, h;
			if (bp[1].code == ART_LINETO) {
				dx = bp[1].x3 - bp[0].x3;
				dy = bp[1].y3 - bp[0].y3;
			} else if (bp[1].code == ART_CURVETO) {
				dx = bp[1].x1 - bp[0].x3;
				dy = bp[1].y1 - bp[0].y3;
			} else {
				dx = 1.0;
				dy = 0.0;
			}
			h = hypot (dx, dy);
			if (h > 1e-9) {
				m.c[0] = dx / h;
				m.c[1] = dy / h;
				m.c[2] = -dy / h;
				m.c[3] = dx / h;
				m.c[4] = bp->x3;
				m.c[5] = bp->y3;
			} else {
				nr_matrix_f_set_translate (&m, bp->x3, bp->y3);
			}
			sp_marker_show_instance ((SPMarker *) shape->marker_start, ai,
						 NR_ARENA_ITEM_GET_KEY (ai) + SP_MARKER_START, nstart,
						 &m, style->stroke_width.computed);
			nstart += 1;
		} else if (shape->marker_end && ((bp[1].code != ART_LINETO) && (bp[1].code != ART_CURVETO))) {
			float dx, dy, h;
			if ((bp->code == ART_LINETO) && (bp > shape->curve->bpath)) {
				dx = bp->x3 - (bp - 1)->x3;
				dy = bp->y3 - (bp - 1)->y3;
			} else if (bp[1].code == ART_CURVETO) {
				dx = bp->x3 - bp->x2;
				dy = bp->y3 - bp->y2;
			} else {
				dx = 1.0;
				dy = 0.0;
			}
			h = hypot (dx, dy);
			if (h > 1e-9) {
				m.c[0] = dx / h;
				m.c[1] = dy / h;
				m.c[2] = -dy / h;
				m.c[3] = dx / h;
				m.c[4] = bp->x3;
				m.c[5] = bp->y3;
			} else {
				nr_matrix_f_set_translate (&m, bp->x3, bp->y3);
			}
			sp_marker_show_instance ((SPMarker *) shape->marker_end, ai,
						 NR_ARENA_ITEM_GET_KEY (ai) + SP_MARKER_END, nend,
						 &m, style->stroke_width.computed);
			nend += 1;
		} else if (shape->marker_mid) {
			nr_matrix_f_set_translate (&m, bp->x3, bp->y3);
			sp_marker_show_instance ((SPMarker *) shape->marker_mid, ai,
						 NR_ARENA_ITEM_GET_KEY (ai) + SP_MARKER_MID, nmid,
						 &m, style->stroke_width.computed);
			nmid += 1;
		}
	}
#endif
}

static void
sp_shape_modified (SPObject *object, unsigned int flags)
{
	SPShape *shape;

	shape = SP_SHAPE (object);

	if (((SPObjectClass *) (parent_class))->modified)
		(* ((SPObjectClass *) (parent_class))->modified) (object, flags);

	if (flags & SP_OBJECT_STYLE_MODIFIED_FLAG) {
		SPItemView *v;
		for (v = SP_ITEM (shape)->display; v != NULL; v = v->view.next) {
			NRArenaStyle *arenastyle = sp_style_get_arena_style (object->style);
			nr_arena_shape_set_style (NR_ARENA_SHAPE (v), arenastyle);
		}
	}
}

static void
sp_shape_bbox (SPItem *item, NRRectF *bbox, const NRMatrixF *transform, unsigned int flags)
{
	SPShape *shape;

	shape = SP_SHAPE (item);

	if (shape->curve) {
		/* NRBPath bp; */
		SPStyle *style;

		style = SP_OBJECT_STYLE (item);
		if (flags & SP_ITEM_BBOX_VISUAL) {
			/* Need visual bbox */
			if (style->stroke.type != SP_PAINT_TYPE_NONE) {
				float scale, width;
				/* Have stroke */
				scale = (float) NR_MATRIX_DF_EXPANSION (transform);
				width = MAX (0.125f, style->stroke_width.computed * scale);
				nr_path_stroke_bbox_union (shape->curve, transform,
							    bbox,
							    width,
							    style->stroke_linecap.computed,
							    style->stroke_linejoin.computed,
							    style->stroke_miterlimit.value * NR_PI_F / 180.0f,
							    0.25);
			} else if (style->fill.type != SP_PAINT_TYPE_NONE) {
				/* Have fill */
				nr_path_matrix_f_bbox_f_union (shape->curve, transform, bbox, 0.25);
			}
#if 0
			/* Markers only attribute to visual bbox */
			if (shape->marker_start || shape->marker_mid || shape->marker_end) {
				SPStyle *style=((SPObject*)item)->style;
				ArtBpath *bp;
				int nstart, nmid, nend;
				/* Determine the number of markers needed */
				nstart = 0;
				nmid = 0;
				nend = 0;
				for (bp = shape->curve->bpath; bp->code != ART_END; bp++) {
					if ((bp[0].code == ART_MOVETO) || (bp[0].code == ART_MOVETO_OPEN)) {
						nstart += 1;
					} else if ((bp[1].code != ART_LINETO) && (bp[1].code != ART_CURVETO)) {
						nend += 1;
					} else {
						nmid += 1;
					}
				}

				/* Measure marker views */
				/*if (!arenaitem->key) NR_ARENA_ITEM_SET_KEY (arenaitem, sp_item_display_key_new (3));*/
				if (shape->marker_start) {
					sp_marker_union_bbox ((SPMarker *) shape->marker_start,
							      /*NR_ARENA_ITEM_GET_KEY (arenaitem) + */SP_MARKER_START,
							      nstart,
							      transform,
							      style->stroke_width.computed,
							      bbox);
				}
				if (shape->marker_mid) {
					sp_marker_union_bbox ((SPMarker *) shape->marker_mid,
							      /*NR_ARENA_ITEM_GET_KEY (arenaitem) + */SP_MARKER_MID,
							      nmid,
							      transform,
							      style->stroke_width.computed,
							      bbox);
				}
				if (shape->marker_end) {
					sp_marker_union_bbox ((SPMarker *) shape->marker_end,
							      /*NR_ARENA_ITEM_GET_KEY (arenaitem) + */SP_MARKER_END,
							      nend,
							      transform,
							      style->stroke_width.computed,
							      bbox);
				}
			}
#endif
		} else {
			/* Logical bbox */
			nr_path_matrix_f_bbox_f_union (shape->curve, transform, bbox, 0.25);
		}
	}
}

void
sp_shape_print (SPItem *item, SPPrintContext *ctx)
{
	SPShape *shape;
	NRRectF pbox, dbox, bbox;
	NRMatrixF i2d;

	shape = SP_SHAPE (item);

	if (!shape->curve) return;

	/* fixme: Think (Lauris) */
	sp_item_invoke_bbox (item, &pbox, NULL, TRUE);
	dbox.x0 = 0.0;
	dbox.y0 = 0.0;
	dbox.x1 = sp_document_get_width (SP_OBJECT_DOCUMENT (item));
	dbox.y1 = sp_document_get_height (SP_OBJECT_DOCUMENT (item));
	sp_item_get_bbox_document (item, &bbox, 0, TRUE);
	sp_item_i2doc_affine (item, &i2d);

	if (SP_OBJECT_STYLE (item)->fill.type != SP_PAINT_TYPE_NONE) {
		sp_print_fill (ctx, shape->curve, &i2d, SP_OBJECT_STYLE (item), &pbox, &dbox, &bbox);
	}

	if (SP_OBJECT_STYLE (item)->stroke.type != SP_PAINT_TYPE_NONE) {
		sp_print_stroke (ctx, shape->curve, &i2d, SP_OBJECT_STYLE (item), &pbox, &dbox, &bbox);
	}
}

static NRArenaItem *
sp_shape_show (SPItem *item, NRArena *arena, unsigned int key, unsigned int flags)
{
	SPObject *object;
	SPShape *shape;
	NRRectF paintbox;
	NRArenaItem *arenaitem;
	NRArenaStyle *arenastyle;

	object = SP_OBJECT (item);
	shape = SP_SHAPE (item);

	arenaitem = nr_arena_item_new (arena, NR_TYPE_ARENA_SHAPE);
	arenastyle = sp_style_get_arena_style (object->style);
	nr_arena_shape_set_style (NR_ARENA_SHAPE (arenaitem), arenastyle);
	nr_arena_shape_set_path (NR_ARENA_SHAPE (arenaitem), shape->curve, TRUE, NULL);
	sp_item_invoke_bbox (item, &paintbox, NULL, TRUE);
	nr_arena_shape_set_paintbox (NR_ARENA_SHAPE (arenaitem), &paintbox);

#if 0
	if (shape->curve && (shape->marker_start || shape->marker_mid || shape->marker_end)) {
		ArtBpath *bp;
		int nstart, nmid, nend;
		/* Determine the number of markers needed */
		nstart = 0;
		nmid = 0;
		nend = 0;
		for (bp = shape->curve->bpath; bp->code != ART_END; bp++) {
			if ((bp[0].code == ART_MOVETO) || (bp[0].code == ART_MOVETO_OPEN)) {
				nstart += 1;
			} else if ((bp[1].code != ART_LINETO) && (bp[1].code != ART_CURVETO)) {
				nend += 1;
			} else {
				nmid += 1;
			}
		}
		/* Dimension marker views */
		if (!arenaitem->key) NR_ARENA_ITEM_SET_KEY (arenaitem, sp_item_display_key_new (3));
		if (shape->marker_start) sp_marker_show_dimension ((SPMarker *) shape->marker_start,
								   NR_ARENA_ITEM_GET_KEY (arenaitem) + SP_MARKER_START,
								   nstart);
		if (shape->marker_mid) sp_marker_show_dimension ((SPMarker *) shape->marker_mid,
								 NR_ARENA_ITEM_GET_KEY (arenaitem) + SP_MARKER_MID,
								 nmid);
		if (shape->marker_end) sp_marker_show_dimension ((SPMarker *) shape->marker_end,
								 NR_ARENA_ITEM_GET_KEY (arenaitem) + SP_MARKER_END,
								 nend);
		/* Update marker views */
		sp_shape_update_marker_view (shape, arenaitem);
	}
#endif

	/* fixme: (Lauris) */
	/* sp_object_request_update ((SPObject *) item, SP_OBJECT_MODIFIED_FLAG); */

	return arenaitem;
}

static void
sp_shape_hide (SPItem *item, unsigned int key)
{
	SPShape *shape;
	SPItemView *v;

	shape = (SPShape *) item;

	if (shape->marker_start) {
		for (v = item->display; v != NULL; v = v->view.next) {
			sp_marker_hide ((SPMarker *) shape->marker_start, NR_ARENA_ITEM_GET_KEY (v));
		}
	}
	if (shape->marker_mid) {
		for (v = item->display; v != NULL; v = v->view.next) {
			sp_marker_hide ((SPMarker *) shape->marker_mid, NR_ARENA_ITEM_GET_KEY (v));
		}
	}
	if (shape->marker_end) {
		for (v = item->display; v != NULL; v = v->view.next) {
			sp_marker_hide ((SPMarker *) shape->marker_end, NR_ARENA_ITEM_GET_KEY (v));
		}
	}

	if (((SPItemClass *) parent_class)->hide)
		((SPItemClass *) parent_class)->hide (item, key);
}

/* Marker stuff */

static void
sp_shape_marker_release (SPObject *marker, SPShape *shape)
{
	SPItem *item;

	item = (SPItem *) shape;

	if (marker == shape->marker_start) {
		SPItemView *v;
		/* Hide marker */
		for (v = item->display; v != NULL; v = v->view.next) {
			sp_marker_hide ((SPMarker *) (shape->marker_start), NR_ARENA_ITEM_GET_KEY (v));
			/* fixme: Do we need explicit remove here? (Lauris) */
			/* nr_arena_item_set_mask (v->arenaitem, NULL); */
		}
		/* Detach marker */
		sp_signal_disconnect_by_data (shape->marker_start, item);
		shape->marker_start = sp_object_hunref (shape->marker_start, item);
	}
	if (marker == shape->marker_mid) {
		SPItemView *v;
		/* Hide marker */
		for (v = item->display; v != NULL; v = v->view.next) {
			sp_marker_hide ((SPMarker *) (shape->marker_mid), NR_ARENA_ITEM_GET_KEY (v));
			/* fixme: Do we need explicit remove here? (Lauris) */
			/* nr_arena_item_set_mask (v->arenaitem, NULL); */
		}
		/* Detach marker */
		sp_signal_disconnect_by_data (shape->marker_mid, item);
		shape->marker_mid = sp_object_hunref (shape->marker_mid, item);
	}
	if (marker == shape->marker_end) {
		SPItemView *v;
		/* Hide marker */
		for (v = item->display; v != NULL; v = v->view.next) {
			sp_marker_hide ((SPMarker *) (shape->marker_end), NR_ARENA_ITEM_GET_KEY (v));
			/* fixme: Do we need explicit remove here? (Lauris) */
			/* nr_arena_item_set_mask (v->arenaitem, NULL); */
		}
		/* Detach marker */
		sp_signal_disconnect_by_data (shape->marker_end, item);
		shape->marker_end = sp_object_hunref (shape->marker_end, item);
	}
}

static void
sp_shape_marker_modified (SPObject *marker, guint flags, SPItem *item)
{
	/* I think mask does update automagically */
	/* g_warning ("Item %s mask %s modified", SP_OBJECT_ID (item), SP_OBJECT_ID (mask)); */
}

void
sp_shape_set_marker (SPObject *object, unsigned int key, const unsigned char *value)
{
	SPItem *item;
	SPShape *shape;
	SPObject *mrk;

	item = (SPItem *) object;
	shape = (SPShape *) object;

	switch (key) {
	case SP_PROP_MARKER_START:
		mrk = sp_uri_reference_resolve (SP_OBJECT_DOCUMENT (object), value);
		if (mrk != shape->marker_start) {
			if (shape->marker_start) {
				SPItemView *v;
				/* Detach marker */
				sp_signal_disconnect_by_data (shape->marker_start, item);
				/* Hide marker */
				for (v = item->display; v != NULL; v = v->view.next) {
					sp_marker_hide ((SPMarker *) (shape->marker_start), NR_ARENA_ITEM_GET_KEY (v));
					/* fixme: Do we need explicit remove here? (Lauris) */
					/* nr_arena_item_set_mask (v->arenaitem, NULL); */
				}
				shape->marker_start = sp_object_hunref (shape->marker_start, object);
			}
			if (SP_IS_MARKER (mrk)) {
				shape->marker_start = sp_object_href (mrk, object);
				g_signal_connect (G_OBJECT (shape->marker_start), "release", G_CALLBACK (sp_shape_marker_release), shape);
				g_signal_connect (G_OBJECT (shape->marker_start), "modified", G_CALLBACK (sp_shape_marker_modified), shape);
			}
			sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		}
		break;
	case SP_PROP_MARKER_MID:
		mrk = sp_uri_reference_resolve (SP_OBJECT_DOCUMENT (object), value);
		if (mrk != shape->marker_mid) {
			if (shape->marker_mid) {
				SPItemView *v;
				/* Detach marker */
				sp_signal_disconnect_by_data (shape->marker_mid, item);
				/* Hide marker */
				for (v = item->display; v != NULL; v = v->view.next) {
					sp_marker_hide ((SPMarker *) (shape->marker_mid), NR_ARENA_ITEM_GET_KEY (v));
					/* fixme: Do we need explicit remove here? (Lauris) */
					/* nr_arena_item_set_mask (v->arenaitem, NULL); */
				}
				shape->marker_mid = sp_object_hunref (shape->marker_mid, object);
			}
			if (SP_IS_MARKER (mrk)) {
				shape->marker_mid = sp_object_href (mrk, object);
				g_signal_connect (G_OBJECT (shape->marker_mid), "release", G_CALLBACK (sp_shape_marker_release), shape);
				g_signal_connect (G_OBJECT (shape->marker_mid), "modified", G_CALLBACK (sp_shape_marker_modified), shape);
			}
			sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		}
		break;
	case SP_PROP_MARKER_END:
		mrk = sp_uri_reference_resolve (SP_OBJECT_DOCUMENT (object), value);
		if (mrk != shape->marker_end) {
			if (shape->marker_end) {
				SPItemView *v;
				/* Detach marker */
				sp_signal_disconnect_by_data (shape->marker_end, item);
				/* Hide marker */
				for (v = item->display; v != NULL; v = v->view.next) {
					sp_marker_hide ((SPMarker *) (shape->marker_end), NR_ARENA_ITEM_GET_KEY (v));
					/* fixme: Do we need explicit remove here? (Lauris) */
					/* nr_arena_item_set_mask (v->arenaitem, NULL); */
				}
				shape->marker_end = sp_object_hunref (shape->marker_end, object);
			}
			if (SP_IS_MARKER (mrk)) {
				shape->marker_end = sp_object_href (mrk, object);
				g_signal_connect (G_OBJECT (shape->marker_end), "release", G_CALLBACK (sp_shape_marker_release), shape);
				g_signal_connect (G_OBJECT (shape->marker_end), "modified", G_CALLBACK (sp_shape_marker_modified), shape);
			}
			sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		}
		break;
	default:
		break;
	}
}



/* Shape section */

void
sp_shape_set_shape (SPShape *shape)
{
	g_return_if_fail (shape != NULL);
	g_return_if_fail (SP_IS_SHAPE (shape));

	if (((SPShapeClass *) G_OBJECT_GET_CLASS (shape))->set_shape)
		((SPShapeClass *) G_OBJECT_GET_CLASS (shape))->set_shape (shape);
}

void
sp_shape_set_curve (SPShape *shape, NRPath *curve, unsigned int duplicate)
{
	if (shape->curve) {
		free (shape->curve);
		shape->curve = NULL;
	}
	if (curve) {
		if (!duplicate) {
			shape->curve = curve;
		} else {
			shape->curve = nr_path_duplicate (curve, 0);
		}
	}
	sp_object_request_update (SP_OBJECT (shape), SP_OBJECT_MODIFIED_FLAG);
}

/* Return duplicate of curve or NULL */
NRPath *
sp_shape_duplicate_curve (SPShape *shape)
{
	if (shape->curve) {
		return nr_path_duplicate (shape->curve, 0);
	}
	return NULL;
}

/* NOT FOR GENERAL PUBLIC UNTIL SORTED OUT (Lauris) */
void
sp_shape_set_curve_insync (SPShape *shape, NRPath *curve, unsigned int duplicate)
{
	if (shape->curve) {
		free (shape->curve);
		shape->curve = NULL;
	}
	if (curve) {
		if (!duplicate) {
			shape->curve = curve;
		} else {
			shape->curve = nr_path_duplicate (curve, 0);
		}
	}
}

