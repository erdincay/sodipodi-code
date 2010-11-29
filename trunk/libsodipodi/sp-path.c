#define __SP_PATH_C__

/*
 * SVG <path> implementation
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 1999-2002 Lauris Kaplinski
 * Copyright (C) 2000-2001 Ximian, Inc.
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#include <string.h>

#include <libnr/nr-matrix.h>
#include <libnr/nr-path.h>

#include "svg/svg.h"
#include "attributes.h"
#include "style.h"

#if 0
#include "xml/node-private.h"
#include "sp-root.h"
#endif

#include "sp-path.h"

#define noPATH_VERBOSE

static void sp_path_class_init (SPPathClass *class);
static void sp_path_init (SPPath *path);

static void sp_path_build (SPObject * object, SPDocument * document, TheraNode * node);
static void sp_path_set (SPObject *object, unsigned int key, const unsigned char *value);

static TheraNode *sp_path_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags);
static void sp_path_write_transform (SPItem *item, TheraNode *node, NRMatrixF *transform);

static SPShapeClass *parent_class;

GType
sp_path_get_type (void)
{
	static GType type = 0;
	if (!type) {
		GTypeInfo info = {
			sizeof (SPPathClass),
			NULL, NULL,
			(GClassInitFunc) sp_path_class_init,
			NULL, NULL,
			sizeof (SPPath),
			16,
			(GInstanceInitFunc) sp_path_init,
		};
		type = g_type_register_static (SP_TYPE_SHAPE, "SPPath", &info, 0);
	}
	return type;
}

static void
sp_path_class_init (SPPathClass * klass)
{
	SPObjectClass *sp_object_class;
	SPItemClass *item_class;

	sp_object_class = (SPObjectClass *) klass;
	item_class = (SPItemClass *) klass;

	parent_class = g_type_class_peek_parent (klass);

	sp_object_class->build = sp_path_build;
	sp_object_class->set = sp_path_set;
	sp_object_class->write = sp_path_write;

	item_class->write_transform = sp_path_write_transform;
}

static void
sp_path_init (SPPath *path)
{
	/* Nothing here */
}

static void
sp_path_build (SPObject *object, SPDocument *document, TheraNode *node)
{
	SPPath *path;

	path = SP_PATH (object);

	sp_object_read_attr (object, "d");
	sp_object_read_attr (object, "marker");
	sp_object_read_attr (object, "marker-start");
	sp_object_read_attr (object, "marker-mid");
	sp_object_read_attr (object, "marker-end");

	if (((SPObjectClass *) parent_class)->build)
		((SPObjectClass *) parent_class)->build (object, document, node);
}

static void
sp_path_set (SPObject *object, unsigned int key, const unsigned char *value)
{
	SPPath *path;

	path = (SPPath *) object;

	switch (key) {
	case SP_ATTR_D:
		if (value) {
			NRPath *nrpath;
			nrpath = sp_svg_read_path (value);
			sp_shape_set_curve ((SPShape *) path, nrpath, FALSE);
		} else {
			sp_shape_set_curve ((SPShape *) path, NULL, FALSE);
		}
		break;
	case SP_PROP_MARKER:
	case SP_PROP_MARKER_START:
	case SP_PROP_MARKER_MID:
	case SP_PROP_MARKER_END:
		sp_shape_set_marker (object, key, value);
		break;
	default:
		if (((SPObjectClass *) parent_class)->set)
			((SPObjectClass *) parent_class)->set (object, key, value);
		break;
	}
}

static TheraNode *
sp_path_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags)
{
	SPShape *shape;
	gchar *str;

	shape = (SPShape *) object;

	if ((flags & SP_OBJECT_WRITE_BUILD) && !node) {
		node = thera_document_new_node (thedoc, THERA_NODE_ELEMENT, "path");
	}

	str = sp_svg_write_path (shape->curve);
	thera_node_set_attribute (node, "d", str);
	g_free (str);

	if (((SPObjectClass *) (parent_class))->write)
		((SPObjectClass *) (parent_class))->write (object, thedoc, node, flags);

	return node;
}

static void
sp_path_write_transform (SPItem *item, TheraNode *node, NRMatrixF *transform)
{
	SPPath *path;
	SPShape *shape;
	NRPath *dpath;
	float ex;
	unsigned char *svgpath;
	SPStyle *style;

	path = (SPPath *) item;
	shape = (SPShape *) item;

	ex = (float) NR_MATRIX_DF_EXPANSION (transform);

	dpath = nr_path_duplicate_transform (shape->curve, transform);
	svgpath = sp_svg_write_path (dpath);
	thera_node_set_attribute (node, "d", svgpath);
	free (svgpath);
	nr_free (dpath);
	/* And last but not least */
	style = SP_OBJECT_STYLE (item);
	if (style->stroke.type != SP_PAINT_TYPE_NONE) {
		if (!NR_DF_TEST_CLOSE (ex, 1.0, NR_EPSILON_D)) {
			guchar *str;
			/* Scale changed, so we have to adjust stroke width */
			style->stroke_width.computed *= ex;
			if (style->stroke_dash && style->stroke_dash->ndashes) {
				unsigned int i;
				for (i = 0; i < style->stroke_dash->ndashes; i++) style->stroke_dash->dashes[i] *= ex;
				style->stroke_dash->offset *= ex;
			}
			str = sp_style_write_difference (style, SP_OBJECT_STYLE (SP_OBJECT_PARENT (item)));
			thera_node_set_attribute (node, "style", str);
			g_free (str);
		}
	}
	nr_matrix_f_set_identity (transform);
}

