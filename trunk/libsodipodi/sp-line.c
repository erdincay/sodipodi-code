#define __SP_LINE_C__

/*
 * SVG <line> implementation
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 1999-2002 Lauris Kaplinski
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#include <math.h>
#include <string.h>

#include "intl.h"
#include "svg/svg.h"
#include "attributes.h"
#include "style.h"
#include "sp-line.h"

#define hypotf(a,b) sqrtf ((a) * (a) + (b) * (b))

static void sp_line_class_init (SPLineClass *class);
static void sp_line_init (SPLine *line);

static void sp_line_build (SPObject * object, SPDocument * document, TheraNode * node);
static void sp_line_set (SPObject *object, unsigned int key, const unsigned char *value);
static TheraNode *sp_line_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags);

static gchar *sp_line_description (SPItem * item);
static void sp_line_write_transform (SPItem *item, TheraNode *node, NRMatrixF *t);

static void sp_line_set_shape (SPLine * line);

static SPShapeClass *parent_class;

GType
sp_line_get_type (void)
{
	static GType line_type = 0;

	if (!line_type) {
		GTypeInfo line_info = {
			sizeof (SPLineClass),
			NULL,	/* base_init */
			NULL,	/* base_finalize */
			(GClassInitFunc) sp_line_class_init,
			NULL,	/* class_finalize */
			NULL,	/* class_data */
			sizeof (SPLine),
			16,	/* n_preallocs */
			(GInstanceInitFunc) sp_line_init,
		};
		line_type = g_type_register_static (SP_TYPE_SHAPE, "SPLine", &line_info, 0);
	}
	return line_type;
}

static void
sp_line_class_init (SPLineClass *class)
{
	GObjectClass * gobject_class;
	SPObjectClass * sp_object_class;
	SPItemClass * item_class;

	gobject_class = (GObjectClass *) class;
	sp_object_class = (SPObjectClass *) class;
	item_class = (SPItemClass *) class;

	parent_class = g_type_class_ref (SP_TYPE_SHAPE);

	sp_object_class->build = sp_line_build;
	sp_object_class->set = sp_line_set;
	sp_object_class->write = sp_line_write;

	item_class->description = sp_line_description;
	item_class->write_transform = sp_line_write_transform;
}

static void
sp_line_init (SPLine * line)
{
	sp_svg_length_unset (&line->x1, SP_SVG_UNIT_NONE, 0.0, 0.0);
	sp_svg_length_unset (&line->y1, SP_SVG_UNIT_NONE, 0.0, 0.0);
	sp_svg_length_unset (&line->x2, SP_SVG_UNIT_NONE, 0.0, 0.0);
	sp_svg_length_unset (&line->y2, SP_SVG_UNIT_NONE, 0.0, 0.0);
}


static void
sp_line_build (SPObject * object, SPDocument * document, TheraNode * node)
{

	if (((SPObjectClass *) parent_class)->build)
		((SPObjectClass *) parent_class)->build (object, document, node);

	sp_object_read_attr (object, "x1");
	sp_object_read_attr (object, "y1");
	sp_object_read_attr (object, "x2");
	sp_object_read_attr (object, "y2");
}

static void
sp_line_set (SPObject *object, unsigned int key, const unsigned char *value)
{
	SPLine * line;

	line = SP_LINE (object);

	/* fixme: we should really collect updates */

	switch (key) {
	case SP_ATTR_X1:
		if (!sp_svg_length_read (value, &line->x1)) {
			sp_svg_length_unset (&line->x1, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_line_set_shape (line);
		break;
	case SP_ATTR_Y1:
		if (!sp_svg_length_read (value, &line->y1)) {
			sp_svg_length_unset (&line->y1, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_line_set_shape (line);
		break;
	case SP_ATTR_X2:
		if (!sp_svg_length_read (value, &line->x2)) {
			sp_svg_length_unset (&line->x2, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_line_set_shape (line);
		break;
	case SP_ATTR_Y2:
		if (!sp_svg_length_read (value, &line->y2)) {
			sp_svg_length_unset (&line->y2, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_line_set_shape (line);
		break;
	default:
		if (((SPObjectClass *) parent_class)->set)
			((SPObjectClass *) parent_class)->set (object, key, value);
		break;
	}
}

static TheraNode *
sp_line_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags)
{
	SPLine *line;

	line = SP_LINE (object);

	if ((flags & SP_OBJECT_WRITE_BUILD) && !node) {
		node = thera_document_new_node (thedoc, THERA_NODE_ELEMENT, "line");
	}

	if (node != object->node) {
		thera_node_merge (node, object->node, "id", TRUE);
	}

	if (((SPObjectClass *) (parent_class))->write)
		((SPObjectClass *) (parent_class))->write (object, thedoc, node, flags);

	return node;
}

static gchar *
sp_line_description (SPItem * item)
{
	return g_strdup ("Line");
}

static void
sp_line_write_transform (SPItem *item, TheraNode *node, NRMatrixF *t)
{
	double sw, sh;
	SPLine *line;

	line = SP_LINE (item);

	/* fixme: Would be nice to preserve units here */
	thera_node_set_double (node, "x1", t->c[0] * line->x1.computed + t->c[2] * line->y1.computed + t->c[4]);
	thera_node_set_double (node, "y1", t->c[1] * line->x1.computed + t->c[3] * line->y1.computed + t->c[5]);
	thera_node_set_double (node, "x2", t->c[0] * line->x2.computed + t->c[2] * line->y2.computed + t->c[4]);
	thera_node_set_double (node, "y2", t->c[1] * line->x2.computed + t->c[3] * line->y2.computed + t->c[5]);

	/* Scalers */
	sw = sqrt (t->c[0] * t->c[0] + t->c[1] * t->c[1]);
	sh = sqrt (t->c[2] * t->c[2] + t->c[3] * t->c[3]);

	/* And last but not least */
	if ((fabs (sw - 1.0) > 1e-9) || (fabs (sh - 1.0) > 1e-9)) {
		SPStyle *style;
		guchar *str;
		/* Scale changed, so we have to adjust stroke width */
		style = SP_OBJECT_STYLE (item);
		style->stroke_width.computed *= sqrtf (fabs (sw * sh));
		str = sp_style_write_difference (style, SP_OBJECT_STYLE (SP_OBJECT_PARENT (item)));
		thera_node_set_attribute (SP_OBJECT_REPR (item), "style", str);
		g_free (str);
	}
}

static void
sp_line_set_shape (SPLine * line)
{
	NRDynamicPath c;
	
	if (hypotf (line->x2.computed - line->x1.computed, line->y2.computed - line->y1.computed) < 1e-12) return;

	nr_dynamic_path_setup (&c, 8);

	nr_dynamic_path_moveto (&c, line->x1.computed, line->y1.computed);
	nr_dynamic_path_lineto (&c, line->x2.computed, line->y2.computed);

	sp_shape_set_curve (SP_SHAPE (line), c.path, TRUE);
}

