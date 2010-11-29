#define __SP_RECT_C__

/*
 * SVG <rect> implementation
 *
 * Authors:
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

#include <math.h>
#include <string.h>

#include <libnr/nr-matrix.h>

#include "svg/svg.h"
#include "attributes.h"
#include "style.h"

#include "sp-root.h"

#include "sp-rect.h"

#if 0
#include "document.h"
#include "dialogs/object-attributes.h"
#include "helper/sp-intl.h"
#endif

#define noRECT_VERBOSE

static void sp_rect_class_init (SPRectClass *class);
static void sp_rect_init (SPRect *rect);

static void sp_rect_build (SPObject *object, SPDocument *document, TheraNode *node);
static void sp_rect_set (SPObject *object, unsigned int key, const unsigned char *value);
static void sp_rect_update (SPObject *object, SPCtx *ctx, guint flags);
static TheraNode *sp_rect_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags);

static gchar * sp_rect_description (SPItem * item);
static int sp_rect_snappoints (SPItem *item, NRPointF *p, int size, const NRMatrixF *transform);
static void sp_rect_write_transform (SPItem *item, TheraNode *node, NRMatrixF *transform);

static void sp_rect_set_shape (SPShape *shape);

static SPShapeClass *parent_class;

GType
sp_rect_get_type (void)
{
	static GType type = 0;

	if (!type) {
		GTypeInfo info = {
			sizeof (SPRectClass),
			NULL,	/* base_init */
			NULL,	/* base_finalize */
			(GClassInitFunc) sp_rect_class_init,
			NULL,	/* class_finalize */
			NULL,	/* class_data */
			sizeof (SPRect),
			16,	/* n_preallocs */
			(GInstanceInitFunc) sp_rect_init,
		};
		type = g_type_register_static (SP_TYPE_SHAPE, "SPRect", &info, 0);
	}
	return type;
}

static void
sp_rect_class_init (SPRectClass *class)
{
	GObjectClass * object_class;
	SPObjectClass * sp_object_class;
	SPItemClass * item_class;
	SPShapeClass * shape_class;

	object_class = (GObjectClass *) class;
	sp_object_class = (SPObjectClass *) class;
	item_class = (SPItemClass *) class;
	shape_class = (SPShapeClass *) class;

	parent_class = g_type_class_ref (SP_TYPE_SHAPE);

	sp_object_class->build = sp_rect_build;
	sp_object_class->write = sp_rect_write;
	sp_object_class->set = sp_rect_set;
	sp_object_class->update = sp_rect_update;

	item_class->description = sp_rect_description;
	item_class->snappoints = sp_rect_snappoints;
	item_class->write_transform = sp_rect_write_transform;

	shape_class->set_shape = sp_rect_set_shape;
}

static void
sp_rect_init (SPRect * rect)
{
	/* Initializing to zero is automatic */
	/* sp_svg_length_unset (&rect->x, SP_SVG_UNIT_NONE, 0.0, 0.0); */
	/* sp_svg_length_unset (&rect->y, SP_SVG_UNIT_NONE, 0.0, 0.0); */
	/* sp_svg_length_unset (&rect->width, SP_SVG_UNIT_NONE, 0.0, 0.0); */
	/* sp_svg_length_unset (&rect->height, SP_SVG_UNIT_NONE, 0.0, 0.0); */
	/* sp_svg_length_unset (&rect->rx, SP_SVG_UNIT_NONE, 0.0, 0.0); */
	/* sp_svg_length_unset (&rect->ry, SP_SVG_UNIT_NONE, 0.0, 0.0); */
}

/* fixme: Better place (Lauris) */

static guint
sp_rect_find_version (SPObject *object)
{

	while (object) {
		if (SP_IS_ROOT (object)) {
			return SP_ROOT (object)->sodipodi;
		}
		object = SP_OBJECT_PARENT (object);
	}

	return 0;
}

static void
sp_rect_build (SPObject *object, SPDocument *document, TheraNode *node)
{
	SPRect *rect;
	guint version;

	rect = SP_RECT (object);

	if (((SPObjectClass *) parent_class)->build)
		((SPObjectClass *) parent_class)->build (object, document, node);

	sp_object_read_attr (object, "x");
	sp_object_read_attr (object, "y");
	sp_object_read_attr (object, "width");
	sp_object_read_attr (object, "height");
	sp_object_read_attr (object, "rx");
	sp_object_read_attr (object, "ry");

	version = sp_rect_find_version (object);

	if (version == 29) {
		if (rect->rx.set && rect->ry.set) {
			/* 0.29 treated 0.0 radius as missing value */
			if ((rect->rx.value != 0.0) && (rect->ry.value == 0.0)) {
				thera_node_set_attribute (node, "ry", NULL);
				sp_object_read_attr (object, "ry");
			} else if ((rect->ry.value != 0.0) && (rect->rx.value == 0.0)) {
				thera_node_set_attribute (node, "rx", NULL);
				sp_object_read_attr (object, "rx");
			}
		}
	}
}

static void
sp_rect_set (SPObject *object, unsigned int key, const unsigned char *value)
{
	SPRect *rect;

	rect = SP_RECT (object);

	/* fixme: We need real error processing some time */

	switch (key) {
	case SP_ATTR_X:
		if (!sp_svg_length_read (value, &rect->x)) {
			sp_svg_length_unset (&rect->x, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_Y:
		if (!sp_svg_length_read (value, &rect->y)) {
			sp_svg_length_unset (&rect->y, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_WIDTH:
		if (!sp_svg_length_read (value, &rect->width) || (rect->width.value < 0.0)) {
			sp_svg_length_unset (&rect->width, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_HEIGHT:
		if (!sp_svg_length_read (value, &rect->height) || (rect->width.value < 0.0)) {
			sp_svg_length_unset (&rect->height, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_RX:
		if (!sp_svg_length_read (value, &rect->rx) || (rect->rx.value < 0.0)) {
			sp_svg_length_unset (&rect->rx, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_RY:
		if (!sp_svg_length_read (value, &rect->ry) || (rect->ry.value < 0.0)) {
			sp_svg_length_unset (&rect->ry, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	default:
		if (((SPObjectClass *) parent_class)->set)
			((SPObjectClass *) parent_class)->set (object, key, value);
		break;
	}
}

static void
sp_rect_update (SPObject *object, SPCtx *ctx, guint flags)
{
	if (flags & (SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_STYLE_MODIFIED_FLAG | SP_OBJECT_VIEWPORT_MODIFIED_FLAG)) {
		SPRect *rect;
		SPStyle *style;
		SPItemCtx *ictx;
		double d, w, h;
		rect = (SPRect *) object;
		style = object->style;
		ictx = (SPItemCtx *) ctx;
		d = 1.0 / NR_MATRIX_DF_EXPANSION (&ictx->i2vp);
		w = d * (ictx->vp.x1 - ictx->vp.x0);
		h = d * (ictx->vp.y1 - ictx->vp.y0);
		sp_svg_length_update (&rect->x, style->font_size.computed, style->font_size.computed * 0.5, w);
		sp_svg_length_update (&rect->y, style->font_size.computed, style->font_size.computed * 0.5, h);
		sp_svg_length_update (&rect->width, style->font_size.computed, style->font_size.computed * 0.5, w);
		sp_svg_length_update (&rect->height, style->font_size.computed, style->font_size.computed * 0.5, h);
		sp_svg_length_update (&rect->rx, style->font_size.computed, style->font_size.computed * 0.5, w);
		sp_svg_length_update (&rect->ry, style->font_size.computed, style->font_size.computed * 0.5, h);
		sp_shape_set_shape ((SPShape *) object);
	}

	if (((SPObjectClass *) parent_class)->update)
		((SPObjectClass *) parent_class)->update (object, ctx, flags);
}

static TheraNode *
sp_rect_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags)
{
	SPRect *rect;

	rect = SP_RECT (object);

	if ((flags & SP_OBJECT_WRITE_BUILD) && !node) {
		node = thera_document_new_node (thedoc, THERA_NODE_ELEMENT, "rect");
	}

	thera_node_set_double (node, "width", rect->width.computed);
	thera_node_set_double (node, "height", rect->height.computed);
	if (rect->rx.set) thera_node_set_double (node, "rx", rect->rx.computed);
	if (rect->ry.set) thera_node_set_double (node, "ry", rect->ry.computed);
	thera_node_set_double (node, "x", rect->x.computed);
	thera_node_set_double (node, "y", rect->y.computed);

	if (((SPObjectClass *) parent_class)->write)
		((SPObjectClass *) parent_class)->write (object, thedoc, node, flags);

	return node;
}

static gchar *
sp_rect_description (SPItem * item)
{
	SPRect *rect;

	rect = SP_RECT (item);

	return g_strdup_printf ("Rectangle %0.2f %0.2f %0.2f %0.2f", rect->x.computed, rect->y.computed, rect->width.computed, rect->height.computed);
}

#define C1 0.554f

static void
sp_rect_set_shape (SPShape *shape)
{
	SPRect *rect;
	float x, y, w, h, w2, h2, rx, ry;
	NRDynamicPath c;

	rect = (SPRect *) shape;

	if ((rect->height.computed < 1e-18) || (rect->width.computed < 1e-18)) return;

	nr_dynamic_path_setup (&c, 16);

	x = (float) rect->x.computed;
	y = (float) rect->y.computed;
	w = (float) rect->width.computed;
	h = (float) rect->height.computed;
	w2 = w / 2;
	h2 = h / 2;
	if (rect->rx.set) {
		rx = CLAMP ((float) rect->rx.computed, 0, (float) rect->width.computed / 2);
	} else if (rect->ry.set) {
		rx = CLAMP ((float) rect->ry.computed, 0, (float) rect->width.computed / 2);
	} else {
		rx = 0.0;
	}
	if (rect->ry.set) {
		ry = CLAMP ((float) rect->ry.computed, 0, (float) rect->height.computed / 2);
	} else if (rect->rx.set) {
		ry = CLAMP ((float) rect->rx.computed, 0, (float) rect->height.computed / 2);
	} else {
		ry = 0;
	}

	if ((rx > 1e-18) && (ry > 1e-18)) {
		nr_dynamic_path_moveto (&c, x + rx, y + 0);
		nr_dynamic_path_curveto3 (&c, x + rx * (1 - C1), y + 0, x + 0, y + ry * (1 - C1), x + 0, y + ry);
		if (ry < h2) nr_dynamic_path_lineto (&c, x + 0, y + h - ry);
		nr_dynamic_path_curveto3 (&c, x + 0, y + h - ry * (1 - C1), x + rx * (1 - C1), y + h, x + rx, y + h);
		if (rx < w2) nr_dynamic_path_lineto (&c, x + w - rx, y + h);
		nr_dynamic_path_curveto3 (&c, x + w - rx * (1 - C1), y + h, x + w, y + h - ry * (1 - C1), x + w, y + h - ry);
		if (ry < h2) nr_dynamic_path_lineto (&c, x + w, y + ry);
		nr_dynamic_path_curveto3 (&c, x + w, y + ry * (1 - C1), x + w - rx * (1 - C1), y + 0, x + w - rx, y + 0);
		if (rx < w2) nr_dynamic_path_lineto (&c, x + rx, y + 0);
	} else {
		nr_dynamic_path_moveto (&c, x + 0, y + 0);
		nr_dynamic_path_lineto (&c, x + 0, y + h);
		nr_dynamic_path_lineto (&c, x + w, y + h);
		nr_dynamic_path_lineto (&c, x + w, y + 0);
		nr_dynamic_path_lineto (&c, x + 0, y + 0);
	}

	nr_dynamic_path_closepath (&c);
	sp_shape_set_curve_insync (SP_SHAPE (rect), c.path, FALSE);
}

/* fixme: Think (Lauris) */

void
sp_rect_position_set (SPRect * rect, gdouble x, gdouble y, gdouble width, gdouble height)
{
	g_return_if_fail (rect != NULL);
	g_return_if_fail (SP_IS_RECT (rect));

	rect->x.computed = (float) x;
	rect->y.computed = (float) y;
	rect->width.computed = (float) width;
	rect->height.computed = (float) height;

	sp_object_request_update (SP_OBJECT (rect), SP_OBJECT_MODIFIED_FLAG);
}

void
sp_rect_set_rx (SPRect *rect, SPSVGLength *value)
{
	g_return_if_fail (rect != NULL);
	g_return_if_fail (SP_IS_RECT(rect));

	if (value) {
		rect->rx = *value;
	} else {
		sp_svg_length_unset (&rect->rx, SP_SVG_UNIT_NONE, 0.0, 0.0);
	}

	sp_object_request_update (SP_OBJECT (rect), SP_OBJECT_MODIFIED_FLAG);
}

void
sp_rect_set_ry (SPRect *rect, SPSVGLength *value)
{
	g_return_if_fail (rect != NULL);
	g_return_if_fail (SP_IS_RECT(rect));

	if (!value) {
		rect->ry = *value;
	} else {
		sp_svg_length_unset (&rect->ry, SP_SVG_UNIT_NONE, 0.0, 0.0);
	}

	sp_object_request_update (SP_OBJECT (rect), SP_OBJECT_MODIFIED_FLAG);
}

static int
sp_rect_snappoints (SPItem *item, NRPointF *p, int size, const NRMatrixF *transform)
{
	SPRect *rect;
	float x0, y0, x1, y1;

	rect = SP_RECT (item);

	/* we use corners of rect only */
	x0 = rect->x.computed;
	y0 = rect->y.computed;
	x1 = x0 + rect->width.computed;
	y1 = y0 + rect->height.computed;

	return sp_corner_snappoints (p, size, transform, x0, y0, x1, y1);
}

/*
 * Initially we'll do:
 * Transform x, y, set x, y, clear translation
 */

/* fixme: Use preferred units somehow (Lauris) */
/* fixme: Alternately preserve whatever units there are (lauris) */

static void
sp_rect_write_transform (SPItem *item, TheraNode *node, NRMatrixF *t)
{
	SPRect *rect;
	NRMatrixF rev;
	float px, py, sw, sh;
	SPStyle *style;

	rect = SP_RECT (item);

	/* Calculate rect start in parent coords */
	px = t->c[0] * rect->x.computed + t->c[2] * rect->y.computed + t->c[4];
	py = t->c[1] * rect->x.computed + t->c[3] * rect->y.computed + t->c[5];

	/* Clear translation */
	t->c[4] = 0.0;
	t->c[5] = 0.0;

	/* Scalers */
	sw = (float) sqrt (t->c[0] * t->c[0] + t->c[1] * t->c[1]);
	sh = (float) sqrt (t->c[2] * t->c[2] + t->c[3] * t->c[3]);
	if (sw > 1e-9) {
		t->c[0] = t->c[0] / sw;
		t->c[1] = t->c[1] / sw;
	} else {
		t->c[0] = 1;
		t->c[1] = 0;
	}
	if (sh > 1e-9) {
		t->c[2] = t->c[2] / sh;
		t->c[3] = t->c[3] / sh;
	} else {
		t->c[2] = 0;
		t->c[3] = 1;
	}
	/* fixme: Would be nice to preserve units here */
	thera_node_set_double (node, "width", rect->width.computed * sw);
	thera_node_set_double (node, "height", rect->height.computed * sh);
	if (rect->rx.set) thera_node_set_double (node, "rx", rect->rx.computed * sw);
	if (rect->ry.set) thera_node_set_double (node, "ry", rect->ry.computed * sh);

	/* Find start in item coords */
	nr_matrix_f_invert (&rev, t);
	thera_node_set_double (node, "x", px * rev.c[0] + py * rev.c[2]);
	thera_node_set_double (node, "y", px * rev.c[1] + py * rev.c[3]);

	/* And last but not least */
	style = SP_OBJECT_STYLE (item);
	if (style->stroke.type != SP_PAINT_TYPE_NONE) {
		if (!NR_DF_TEST_CLOSE (sw, 1.0, NR_EPSILON_D) || !NR_DF_TEST_CLOSE (sh, 1.0, NR_EPSILON_D)) {
			float scale;
			guchar *str;
			/* Scale changed, so we have to adjust stroke width */
			scale = (float) sqrt (fabs (sw * sh));
			style->stroke_width.computed *= scale;
			if (style->stroke_dash && (style->stroke_dash->ndashes)) {
				unsigned int i;
				for (i = 0; i < style->stroke_dash->ndashes; i++) style->stroke_dash->dashes[i] *= scale;
				style->stroke_dash->offset *= scale;
			}
			str = sp_style_write_difference (style, SP_OBJECT_STYLE (SP_OBJECT_PARENT (item)));
			thera_node_set_attribute (SP_OBJECT_REPR (item), "style", str);
			g_free (str);
		}
	}
}


