#define __SP_ELLIPSE_C__

/*
 * SVG <ellipse> and related implementations
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *   Mitsuru Oka
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

#include <libnr/nr-values.h>
#include <libnr/nr-matrix.h>

#include "svg/svg.h"
#include "attributes.h"
#include "style.h"

#include "sp-ellipse.h"

/* Common parent class */

#define noELLIPSE_VERBOSE

#if 1
/* Hmmm... shouldn't this also qualify */
/* Whether it is faster or not, well, nobody knows */
#define sp_round(v,m) (((v) < 0.0) ? ((ceil ((v) / (m) - 0.5)) * (m)) : ((floor ((v) / (m) + 0.5)) * (m)))
#else
/* we do not use C99 round(3) function yet */
static double sp_round (double x, double y)
{
	double remain;

	g_assert (y > 0.0);

	/* return round(x/y) * y; */

	remain = fmod(x, y);

	if (remain >= 0.5*y)
		return x - remain + y;
	else
		return x - remain;
}
#endif

static void sp_genericellipse_class_init (SPGenericEllipseClass *klass);
static void sp_genericellipse_init (SPGenericEllipse *ellipse);

static void sp_genericellipse_update (SPObject *object, SPCtx *ctx, guint flags);

static int sp_genericellipse_snappoints (SPItem *item, NRPointF *p, int size, const NRMatrixF *transform);

static void sp_genericellipse_set_shape (SPShape *shape);

static SPShapeClass *ge_parent_class;

GType
sp_genericellipse_get_type (void)
{
	static GType type = 0;
	if (!type) {
		GTypeInfo info = {
			sizeof (SPGenericEllipseClass),
			NULL,	/* base_init */
			NULL,	/* base_finalize */
			(GClassInitFunc) sp_genericellipse_class_init,
			NULL,	/* class_finalize */
			NULL,	/* class_data */
			sizeof (SPGenericEllipse),
			16,	/* n_preallocs */
			(GInstanceInitFunc) sp_genericellipse_init,
		};
		type = g_type_register_static (SP_TYPE_SHAPE, "SPGenericEllipse", &info, 0);
	}
	return type;
}

static void
sp_genericellipse_class_init (SPGenericEllipseClass *klass)
{
	GObjectClass *gobject_class;
	SPObjectClass * sp_object_class;
	SPItemClass *item_class;
	SPShapeClass *shape_class;

	gobject_class = (GObjectClass *) klass;
	sp_object_class = (SPObjectClass *) klass;
	item_class = (SPItemClass *) klass;
	shape_class = (SPShapeClass *) klass;

	ge_parent_class = g_type_class_ref (SP_TYPE_SHAPE);

	sp_object_class->update = sp_genericellipse_update;

	item_class->snappoints = sp_genericellipse_snappoints;

	shape_class->set_shape = sp_genericellipse_set_shape;
}

static void
sp_genericellipse_init (SPGenericEllipse *ellipse)
{
	sp_svg_length_unset (&ellipse->cx, SP_SVG_UNIT_NONE, 0.0, 0.0);
	sp_svg_length_unset (&ellipse->cy, SP_SVG_UNIT_NONE, 0.0, 0.0);
	sp_svg_length_unset (&ellipse->rx, SP_SVG_UNIT_NONE, 0.0, 0.0);
	sp_svg_length_unset (&ellipse->ry, SP_SVG_UNIT_NONE, 0.0, 0.0);

	ellipse->start = 0.0;
	ellipse->end = NR_2PI_F;
	ellipse->closed = TRUE;
}

static void
sp_genericellipse_update (SPObject *object, SPCtx *ctx, guint flags)
{
	if (flags & (SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_STYLE_MODIFIED_FLAG | SP_OBJECT_VIEWPORT_MODIFIED_FLAG)) {
		SPGenericEllipse *ellipse;
		SPStyle *style;
		double d;
		ellipse = (SPGenericEllipse *) object;
		style = object->style;
		d = 1.0 / NR_MATRIX_DF_EXPANSION (&((SPItemCtx *) ctx)->i2vp);
		sp_svg_length_update (&ellipse->cx, style->font_size.computed, style->font_size.computed * 0.5, d);
		sp_svg_length_update (&ellipse->cy, style->font_size.computed, style->font_size.computed * 0.5, d);
		sp_svg_length_update (&ellipse->rx, style->font_size.computed, style->font_size.computed * 0.5, d);
		sp_svg_length_update (&ellipse->ry, style->font_size.computed, style->font_size.computed * 0.5, d);
		sp_shape_set_shape ((SPShape *) object);
	}

	if (((SPObjectClass *) ge_parent_class)->update)
		((SPObjectClass *) ge_parent_class)->update (object, ctx, flags);
}

#define noC1 0.552

#define C1 (4.0f/3.0f*(sqrtf(2.0f)-1.0f))

static void sp_genericellipse_set_shape (SPShape *shape)
{
	SPGenericEllipse *ellipse;
	NRDynamicPath c;
	NRPath *path;

	float cx, cy, rx, ry, s, e;
	float x0, y0, x1, y1, x2, y2, x3, y3;
	float len;
	NRMatrixF matrix;
	gint slice = FALSE;

	ellipse = (SPGenericEllipse *) shape;

	if ((ellipse->rx.computed < 1e-18) || (ellipse->ry.computed < 1e-18)) return;
	if (fabs (ellipse->end - ellipse->start) < 1e-9) return;

	sp_genericellipse_normalize (ellipse);

	cx = 0.0;
	cy = 0.0;
	rx = ellipse->rx.computed;
	ry = ellipse->ry.computed;

	len = fmodf (ellipse->end - ellipse->start, NR_2PI_F);
	if (len < 0.0) len += NR_2PI_F;
	if (fabs (len) < 1e-9) {
		slice = FALSE;
	} else {
		slice = TRUE;
	}

	nr_matrix_f_set_scale (&matrix, rx, ry);
	matrix.c[4]= ellipse->cx.computed;
	matrix.c[5] = ellipse->cy.computed;

	nr_dynamic_path_setup (&c, 16);

	nr_dynamic_path_moveto (&c, cosf (ellipse->start), sinf (ellipse->start));

	for (s = ellipse->start; s < ellipse->end; s += NR_PI2_F) {
		e = s + NR_PI2_F;
		if (e > ellipse->end) e = ellipse->end;
		len = C1 * (e - s) / NR_PI2_F;
		x0 = cosf (s);
		y0 = sinf (s);
		x1 = x0 + len * cosf (s + NR_PI2_F);
		y1 = y0 + len * sinf (s + NR_PI2_F);
		x3 = cosf (e);
		y3 = sinf (e);
		x2 = x3 + len * cosf (e - NR_PI2_F);
		y2 = y3 + len * sinf (e - NR_PI2_F);
		nr_dynamic_path_curveto3 (&c, x1, y1, x2, y2, x3, y3);
	}

	if (slice && ellipse->closed) {
		nr_dynamic_path_lineto (&c, 0, 0);
		nr_dynamic_path_lineto (&c, cosf (ellipse->start), sinf (ellipse->start));
	} else if (ellipse->closed) {
		nr_dynamic_path_lineto (&c, cosf (ellipse->start), sinf (ellipse->start));
	}
	if (ellipse->closed) {
		nr_dynamic_path_closepath (&c);
	}

	path = nr_path_duplicate_transform (c.path, &matrix);
	nr_dynamic_path_release (&c);

	sp_shape_set_curve_insync (SP_SHAPE(ellipse), path, TRUE);
}

static int
sp_genericellipse_snappoints (SPItem *item, NRPointF *p, int size, const NRMatrixF *transform)
{
	SPGenericEllipse *ge;
	int pos;

	ge = SP_GENERICELLIPSE (item);

	/* we use corners of item and center of ellipse */
	pos = 0;
	if (((SPItemClass *) ge_parent_class)->snappoints)
		pos = ((SPItemClass *) ge_parent_class)->snappoints (item, p, size, transform);

	if (pos < size) {
		p[pos].x = NR_MATRIX_DF_TRANSFORM_X (transform, ge->cx.computed, ge->cy.computed);
		p[pos].y = NR_MATRIX_DF_TRANSFORM_Y (transform, ge->cx.computed, ge->cy.computed);
	}

	return pos;
}

void
sp_genericellipse_normalize (SPGenericEllipse *ellipse)
{
	float diff;

	ellipse->start = fmodf (ellipse->start, NR_2PI_F);
	ellipse->end = fmodf (ellipse->end, NR_2PI_F);

	if (ellipse->start < 0.0)
		ellipse->start += NR_2PI_F;
	diff = ellipse->start - ellipse->end;
	if (diff >= 0.0)
		ellipse->end += diff - fmodf (diff, NR_2PI_F) + NR_2PI_F;

	/* Now we keep: 0 <= start < end <= 2*PI */
}

/* SVG <ellipse> element */

static void sp_ellipse_class_init (SPEllipseClass *class);
static void sp_ellipse_init (SPEllipse *ellipse);

static void sp_ellipse_build (SPObject * object, SPDocument * document, TheraNode * node);
static TheraNode *sp_ellipse_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags);
static void sp_ellipse_set (SPObject *object, unsigned int key, const unsigned char *value);
static gchar * sp_ellipse_description (SPItem * item);

static SPGenericEllipseClass *ellipse_parent_class;

GType
sp_ellipse_get_type (void)
{
	static GType type = 0;
	if (!type) {
		GTypeInfo info = {
			sizeof (SPEllipseClass),
			NULL,	/* base_init */
			NULL,	/* base_finalize */
			(GClassInitFunc) sp_ellipse_class_init,
			NULL,	/* class_finalize */
			NULL,	/* class_data */
			sizeof (SPEllipse),
			16,	/* n_preallocs */
			(GInstanceInitFunc) sp_ellipse_init,
		};
		type = g_type_register_static (SP_TYPE_GENERICELLIPSE, "SPEllipse", &info, 0);
	}
	return type;
}

static void
sp_ellipse_class_init (SPEllipseClass *class)
{
	GObjectClass * gobject_class;
	SPObjectClass * sp_object_class;
	SPItemClass *item_class;

	gobject_class = (GObjectClass *) class;
	sp_object_class = (SPObjectClass *) class;
	item_class = (SPItemClass *) class;

	ellipse_parent_class = g_type_class_ref (SP_TYPE_GENERICELLIPSE);

	sp_object_class->build = sp_ellipse_build;
	sp_object_class->write = sp_ellipse_write;
	sp_object_class->set = sp_ellipse_set;

	item_class->description = sp_ellipse_description;
}

static void
sp_ellipse_init (SPEllipse *ellipse)
{
	/* Nothing special */
}

static void
sp_ellipse_build (SPObject *object, SPDocument *document, TheraNode *node)
{
	if (((SPObjectClass *) ellipse_parent_class)->build)
		(* ((SPObjectClass *) ellipse_parent_class)->build) (object, document, node);

	sp_object_read_attr (object, "cx");
	sp_object_read_attr (object, "cy");
	sp_object_read_attr (object, "rx");
	sp_object_read_attr (object, "ry");
}

static TheraNode *
sp_ellipse_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags)
{
	SPGenericEllipse *ellipse;

	ellipse = SP_GENERICELLIPSE (object);

	if ((flags & SP_OBJECT_WRITE_BUILD) && !node) {
		node = thera_document_new_node (thedoc, THERA_NODE_ELEMENT, "ellipse");
	}

	thera_node_set_double (node, "cx", ellipse->cx.computed);
	thera_node_set_double (node, "cy", ellipse->cy.computed);
	thera_node_set_double (node, "rx", ellipse->rx.computed);
	thera_node_set_double (node, "ry", ellipse->ry.computed);
	
	if (((SPObjectClass *) ellipse_parent_class)->write)
		(* ((SPObjectClass *) ellipse_parent_class)->write) (object, thedoc, node, flags);

	return node;
}

static void
sp_ellipse_set (SPObject *object, unsigned int key, const unsigned char *value)
{
	SPGenericEllipse *ellipse;

	ellipse = SP_GENERICELLIPSE (object);

	switch (key) {
	case SP_ATTR_CX:
		if (!sp_svg_length_read (value, &ellipse->cx)) {
			sp_svg_length_unset (&ellipse->cx, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_CY:
		if (!sp_svg_length_read (value, &ellipse->cy)) {
			sp_svg_length_unset (&ellipse->cy, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_RX:
		if (!sp_svg_length_read (value, &ellipse->rx) || (ellipse->rx.value <= 0.0)) {
			sp_svg_length_unset (&ellipse->rx, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_RY:
		if (!sp_svg_length_read (value, &ellipse->ry) || (ellipse->ry.value <= 0.0)) {
			sp_svg_length_unset (&ellipse->ry, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	default:
		if (((SPObjectClass *) ellipse_parent_class)->set)
			((SPObjectClass *) ellipse_parent_class)->set (object, key, value);
		break;
	}
}

static gchar *
sp_ellipse_description (SPItem * item)
{
	return g_strdup ("Ellipse");
}


void
sp_ellipse_position_set (SPEllipse *ellipse, float x, float y, float rx, float ry)
{
	SPGenericEllipse *ge;

	g_return_if_fail (ellipse != NULL);
	g_return_if_fail (SP_IS_ELLIPSE (ellipse));

	ge = SP_GENERICELLIPSE (ellipse);

	ge->cx.computed = x;
	ge->cy.computed = y;
	ge->rx.computed = rx;
	ge->ry.computed = ry;

	sp_object_request_update ((SPObject *) ge, SP_OBJECT_MODIFIED_FLAG);
}

/* SVG <circle> element */

static void sp_circle_class_init (SPCircleClass *class);
static void sp_circle_init (SPCircle *circle);

static void sp_circle_build (SPObject * object, SPDocument * document, TheraNode * node);
static TheraNode *sp_circle_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags);
static void sp_circle_set (SPObject *object, unsigned int key, const unsigned char *value);
static gchar * sp_circle_description (SPItem * item);

static SPGenericEllipseClass *circle_parent_class;

GType
sp_circle_get_type (void)
{
	static GType type = 0;
	if (!type) {
		GTypeInfo info = {
			sizeof (SPCircleClass),
			NULL,	/* base_init */
			NULL,	/* base_finalize */
			(GClassInitFunc) sp_circle_class_init,
			NULL,	/* class_finalize */
			NULL,	/* class_data */
			sizeof (SPCircle),
			16,	/* n_preallocs */
			(GInstanceInitFunc) sp_circle_init,
		};
		type = g_type_register_static (SP_TYPE_GENERICELLIPSE, "SPCircle", &info, 0);
	}
	return type;
}

static void
sp_circle_class_init (SPCircleClass *class)
{
	GObjectClass * gobject_class;
	SPObjectClass * sp_object_class;
	SPItemClass *item_class;

	gobject_class = (GObjectClass *) class;
	sp_object_class = (SPObjectClass *) class;
	item_class = (SPItemClass *) class;

	circle_parent_class = g_type_class_ref (SP_TYPE_GENERICELLIPSE);

	sp_object_class->build = sp_circle_build;
	sp_object_class->write = sp_circle_write;
	sp_object_class->set = sp_circle_set;

	item_class->description = sp_circle_description;
}

static void
sp_circle_init (SPCircle *circle)
{
	/* Nothing special */
}

static void
sp_circle_build (SPObject *object, SPDocument *document, TheraNode *node)
{
	if (((SPObjectClass *) circle_parent_class)->build)
		(* ((SPObjectClass *) circle_parent_class)->build) (object, document, node);

	sp_object_read_attr (object, "cx");
	sp_object_read_attr (object, "cy");
	sp_object_read_attr (object, "r");
}

static TheraNode *
sp_circle_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags)
{
	SPGenericEllipse *ellipse;

	ellipse = SP_GENERICELLIPSE (object);

	if ((flags & SP_OBJECT_WRITE_BUILD) && !node) {
		node = thera_document_new_node (thedoc, THERA_NODE_ELEMENT, "circle");
	}

	thera_node_set_double (node, "cx", ellipse->cx.computed);
	thera_node_set_double (node, "cy", ellipse->cy.computed);
	thera_node_set_double (node, "r", ellipse->rx.computed);
	
	if (((SPObjectClass *) circle_parent_class)->write)
		((SPObjectClass *) circle_parent_class)->write (object, thedoc, node, flags);

	return node;
}

static void
sp_circle_set (SPObject *object, unsigned int key, const unsigned char *value)
{
	SPGenericEllipse *ge;

	ge = SP_GENERICELLIPSE (object);

	switch (key) {
	case SP_ATTR_CX:
		if (!sp_svg_length_read (value, &ge->cx)) {
			sp_svg_length_unset (&ge->cx, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_CY:
		if (!sp_svg_length_read (value, &ge->cy)) {
			sp_svg_length_unset (&ge->cy, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_R:
		if (!sp_svg_length_read (value, &ge->rx) || (ge->rx.value <= 0.0)) {
			sp_svg_length_unset (&ge->rx, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		ge->ry = ge->rx;
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	default:
		if (((SPObjectClass *) circle_parent_class)->set)
			((SPObjectClass *) circle_parent_class)->set (object, key, value);
		break;
	}
}

static gchar *
sp_circle_description (SPItem * item)
{
	return g_strdup ("Circle");
}

/* <path sodipodi:type="arc"> element */

static void sp_arc_class_init (SPArcClass *class);
static void sp_arc_init (SPArc *arc);

static void sp_arc_build (SPObject * object, SPDocument * document, TheraNode * node);
static TheraNode *sp_arc_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags);
static void sp_arc_set (SPObject *object, unsigned int key, const unsigned char *value);
static void sp_arc_modified (SPObject *object, guint flags);

static gchar * sp_arc_description (SPItem * item);

static SPGenericEllipseClass *arc_parent_class;

GType
sp_arc_get_type (void)
{
	static GType type = 0;
	if (!type) {
		GTypeInfo info = {
			sizeof (SPArcClass),
			NULL,	/* base_init */
			NULL,	/* base_finalize */
			(GClassInitFunc) sp_arc_class_init,
			NULL,	/* class_finalize */
			NULL,	/* class_data */
			sizeof (SPArc),
			16,	/* n_preallocs */
			(GInstanceInitFunc) sp_arc_init,
		};
		type = g_type_register_static (SP_TYPE_GENERICELLIPSE, "SPArc", &info, 0);
	}
	return type;
}

static void
sp_arc_class_init (SPArcClass *class)
{
	GObjectClass * gobject_class;
	SPObjectClass * sp_object_class;
	SPItemClass *item_class;

	gobject_class = (GObjectClass *) class;
	sp_object_class = (SPObjectClass *) class;
	item_class = (SPItemClass *) class;

	arc_parent_class = g_type_class_ref (SP_TYPE_GENERICELLIPSE);

	sp_object_class->build = sp_arc_build;
	sp_object_class->write = sp_arc_write;
	sp_object_class->set = sp_arc_set;
	sp_object_class->modified = sp_arc_modified;

	item_class->description = sp_arc_description;
}

static void
sp_arc_init (SPArc *arc)
{
	/* Nothing special */
}

static void
sp_arc_build (SPObject *object, SPDocument *document, TheraNode *node)
{
	if (((SPObjectClass *) arc_parent_class)->build)
		(* ((SPObjectClass *) arc_parent_class)->build) (object, document, node);

	/* New attributes */
	sp_object_read_attr (object, "sodipodi:cx");
	sp_object_read_attr (object, "sodipodi:cy");
	sp_object_read_attr (object, "sodipodi:rx");
	sp_object_read_attr (object, "sodipodi:ry");

	sp_object_read_attr (object, "sodipodi:start");
	sp_object_read_attr (object, "sodipodi:end");
	sp_object_read_attr (object, "sodipodi:open");
}

/*
 * sp_arc_set_elliptical_path_attribute:
 *
 * Convert center to endpoint parameterization and set it to node.
 *
 * See SVG 1.0 Specification W3C Recommendation
 * ``F.6 Ellptical arc implementation notes'' for more detail.
 */

#define ARC_BUFSIZE 1024

static gboolean
sp_arc_set_elliptical_path_attribute (SPArc *arc, TheraNode *node)
{
	SPGenericEllipse *ge;
	NRPointF p1, p2;
	gint fa, fs;
	float  dt;
	unsigned char c[ARC_BUFSIZE];

	ge = SP_GENERICELLIPSE (arc);

	sp_arc_get_xy (arc, ge->start, &p1);
	sp_arc_get_xy (arc, ge->end, &p2);

	dt = fmodf (ge->end - ge->start, NR_2PI_F);
	if (fabs (dt) < 1e-3) {
		NRPointF ph;
		int pos;
		/* Full circle */
		pos = 0;
		sp_arc_get_xy (arc, (ge->start + ge->end) / 2, &ph);
		c[pos++] = 'M';
		c[pos++] = ' ';
		pos += sp_svg_number_write_d (c + pos, p1.x, 6, 0, FALSE);
		c[pos++] = ' ';
		pos += sp_svg_number_write_d (c + pos, p1.y, 6, 0, FALSE);
		c[pos++] = ' ';
		c[pos++] = 'A';
		c[pos++] = ' ';
		pos += sp_svg_number_write_d (c + pos, ge->rx.computed, 6, 0, FALSE);
		c[pos++] = ' ';
		pos += sp_svg_number_write_d (c + pos, ge->ry.computed, 6, 0, FALSE);
		c[pos++] = ' ';
		c[pos++] = '0';
		c[pos++] = ' ';
		c[pos++] = '1';
		c[pos++] = ' ';
		c[pos++] = (dt > 0) ? '1' : '0';
		c[pos++] = ' ';
		pos += sp_svg_number_write_d (c + pos, ph.x, 6, 0, FALSE);
		c[pos++] = ' ';
		pos += sp_svg_number_write_d (c + pos, ph.y, 6, 0, FALSE);
		c[pos++] = ' ';
		c[pos++] = 'A';
		c[pos++] = ' ';
		pos += sp_svg_number_write_d (c + pos, ge->rx.computed, 6, 0, FALSE);
		c[pos++] = ' ';
		pos += sp_svg_number_write_d (c + pos, ge->ry.computed, 6, 0, FALSE);
		c[pos++] = ' ';
		c[pos++] = '0';
		c[pos++] = ' ';
		c[pos++] = '1';
		c[pos++] = ' ';
		c[pos++] = (dt > 0) ? '1' : '0';
		c[pos++] = ' ';
		c[pos++] = ' ';
		pos += sp_svg_number_write_d (c + pos, p2.x, 6, 0, FALSE);
		c[pos++] = ' ';
		pos += sp_svg_number_write_d (c + pos, p2.y, 6, 0, FALSE);

#if 0
		c[pos++] = ' ';
		c[pos++] = 'L';
		c[pos++] = ' ';
		pos += sp_svg_number_write_d (c + pos, ge->cx.computed, 6, 0, FALSE);
		c[pos++] = ' ';
		pos += sp_svg_number_write_d (c + pos, ge->cy.computed, 6, 0, FALSE);
#endif
		c[pos++] = ' ';
		c[pos++] = 'z';
		c[pos++] = '\0';
	} else {
		fa = (fabsf (dt) > NR_PI_F) ? 1 : 0;
		fs = (dt > 0) ? 1 : 0;
#ifdef ARC_VERBOSE
		g_print ("start:%g end:%g fa=%d fs=%d\n", ge->start, ge->end, fa, fs);
#endif
		if (ge->closed) {
			int pos = 0;
			c[pos++] = 'M';
			c[pos++] = ' ';
			pos += sp_svg_number_write_d (c + pos, p1.x, 6, 0, FALSE);
			c[pos++] = ' ';
			pos += sp_svg_number_write_d (c + pos, p1.y, 6, 0, FALSE);
			
			c[pos++] = ' ';
			c[pos++] = 'A';
			c[pos++] = ' ';
			pos += sp_svg_number_write_d (c + pos, ge->rx.computed, 6, 0, FALSE);
			c[pos++] = ' ';
			pos += sp_svg_number_write_d (c + pos, ge->ry.computed, 6, 0, FALSE);
			c[pos++] = ' ';
			c[pos++] = '0';
			c[pos++] = ' ';
			c[pos++] = (fabs (dt) > NR_PI_F) ? '1' : '0';
			c[pos++] = ' ';
			c[pos++] = (dt > 0) ? '1' : '0';
			c[pos++] = ' ';
			pos += sp_svg_number_write_d (c + pos, p2.x, 6, 0, FALSE);
			c[pos++] = ' ';
			pos += sp_svg_number_write_d (c + pos, p2.y, 6, 0, FALSE);

			c[pos++] = ' ';
			c[pos++] = 'L';
			c[pos++] = ' ';
			pos += sp_svg_number_write_d (c + pos, ge->cx.computed, 6, 0, FALSE);
			c[pos++] = ' ';
			pos += sp_svg_number_write_d (c + pos, ge->cy.computed, 6, 0, FALSE);
			c[pos++] = 'z';
			c[pos++] = '\0';
		} else {
			int pos = 0;
			c[pos++] = 'M';
			c[pos++] = ' ';
			pos += sp_svg_number_write_d (c + pos, p1.x, 6, 0, FALSE);
			c[pos++] = ' ';
			pos += sp_svg_number_write_d (c + pos, p1.y, 6, 0, FALSE);
			
			c[pos++] = ' ';
			c[pos++] = 'A';
			c[pos++] = ' ';
			pos += sp_svg_number_write_d (c + pos, ge->rx.computed, 6, 0, FALSE);
			c[pos++] = ' ';
			pos += sp_svg_number_write_d (c + pos, ge->ry.computed, 6, 0, FALSE);
			c[pos++] = ' ';
			c[pos++] = '0';
			c[pos++] = ' ';
			c[pos++] = (fabs (dt) > NR_PI_F) ? '1' : '0';
			c[pos++] = ' ';
			c[pos++] = (dt > 0) ? '1' : '0';
			c[pos++] = ' ';
			pos += sp_svg_number_write_d (c + pos, p2.x, 6, 0, FALSE);
			c[pos++] = ' ';
			pos += sp_svg_number_write_d (c + pos, p2.y, 6, 0, FALSE);
			c[pos++] = '\0';
		}
	}

	return thera_node_set_attribute (node, "d", c);
}

static TheraNode *
sp_arc_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags)
{
	SPGenericEllipse *ge;
	SPArc *arc;

	ge = SP_GENERICELLIPSE (object);
	arc = SP_ARC (object);

	if (flags & SP_OBJECT_WRITE_SODIPODI) {
		if ((flags & SP_OBJECT_WRITE_BUILD) && !node) {
			node = thera_document_new_node (thedoc, THERA_NODE_ELEMENT, "path");
		}

		thera_node_set_attribute (node, "sodipodi:type", "arc");
		thera_node_set_double (node, "sodipodi:cx", ge->cx.computed);
		thera_node_set_double (node, "sodipodi:cy", ge->cy.computed);
		thera_node_set_double (node, "sodipodi:rx", ge->rx.computed);
		thera_node_set_double (node, "sodipodi:ry", ge->ry.computed);
		thera_node_set_double (node, "sodipodi:start", ge->start);
		thera_node_set_double (node, "sodipodi:end", ge->end);
		thera_node_set_attribute (node, "sodipodi:open", (!ge->closed) ? "true" : NULL);

		sp_arc_set_elliptical_path_attribute (arc, node);
	} else {
		gdouble dt;
		dt = fmod (ge->end - ge->start, NR_2PI_F);
		if (fabs (dt) < 1e-6) {
			if ((flags & SP_OBJECT_WRITE_BUILD) && !node) {
				node = thera_document_new_node (thedoc, THERA_NODE_ELEMENT, "ellipse");
			}
			thera_node_set_double (node, "cx", ge->cx.computed);
			thera_node_set_double (node, "cy", ge->cy.computed);
			thera_node_set_double (node, "rx", ge->rx.computed);
			thera_node_set_double (node, "ry", ge->ry.computed);
		} else {
			if ((flags & SP_OBJECT_WRITE_BUILD) && !node) {
				node = thera_document_new_node (thedoc, THERA_NODE_ELEMENT, "path");
			}
			sp_arc_set_elliptical_path_attribute (arc, node);
		}
	}

	if (((SPObjectClass *) arc_parent_class)->write)
		((SPObjectClass *) arc_parent_class)->write (object, thedoc, node, flags);

	return node;
}

static void
sp_arc_set (SPObject *object, unsigned int key, const unsigned char *value)
{
	SPGenericEllipse *ge;

	ge = SP_GENERICELLIPSE (object);
	
	switch (key) {
	case SP_ATTR_SODIPODI_CX:
		if (!sp_svg_length_read (value, &ge->cx)) {
			sp_svg_length_unset (&ge->cx, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_SODIPODI_CY:
		if (!sp_svg_length_read (value, &ge->cy)) {
			sp_svg_length_unset (&ge->cy, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_SODIPODI_RX:
		if (!sp_svg_length_read (value, &ge->rx) || (ge->rx.computed <= 0.0)) {
			sp_svg_length_unset (&ge->rx, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_SODIPODI_RY:
		if (!sp_svg_length_read (value, &ge->ry) || (ge->ry.computed <= 0.0)) {
			sp_svg_length_unset (&ge->ry, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_SODIPODI_START:
		sp_svg_number_read_f (value, &ge->start);
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_SODIPODI_END:
		sp_svg_number_read_f (value, &ge->end);
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_SODIPODI_OPEN:
		ge->closed = (!value);
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	default:
		if (((SPObjectClass *) arc_parent_class)->set)
			((SPObjectClass *) arc_parent_class)->set (object, key, value);
		break;
	}
}

static void
sp_arc_modified (SPObject *object, guint flags)
{
	if (flags & SP_OBJECT_MODIFIED_FLAG) {
		sp_arc_set_elliptical_path_attribute (SP_ARC (object), object->node);
	}

	if (((SPObjectClass *) arc_parent_class)->modified)
		((SPObjectClass *) arc_parent_class)->modified (object, flags);
}

static gchar *
sp_arc_description (SPItem * item)
{
	return g_strdup ("Arc");
}

void
sp_arc_position_set (SPArc *arc, float x, float y, float rx, float ry)
{
	SPGenericEllipse *ge;

	g_return_if_fail (arc != NULL);
	g_return_if_fail (SP_IS_ARC (arc));

	ge = SP_GENERICELLIPSE (arc);

	ge->cx.computed = x;
	ge->cy.computed = y;
	ge->rx.computed = rx;
	ge->ry.computed = ry;

	sp_object_request_update ((SPObject *) arc, SP_OBJECT_MODIFIED_FLAG);
}

void
sp_arc_get_xy (SPArc *arc, float arg, NRPointF *p)
{
	SPGenericEllipse *ge;

	ge = SP_GENERICELLIPSE (arc);

	p->x = ge->rx.computed * cosf (arg) + ge->cx.computed;
	p->y = ge->ry.computed * sinf (arg) + ge->cy.computed;
}

