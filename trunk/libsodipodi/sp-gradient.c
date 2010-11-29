#define __SP_GRADIENT_C__

/*
 * SVG <stop> <linearGradient> and <radialGradient> implementation
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 1999-2002 Lauris Kaplinski
 * Copyright (C) 2000-2001 Ximian, Inc.
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#define noSP_GRADIENT_VERBOSE

#include <string.h>

#include <libnr/nr-matrix.h>
#include <libnr/nr-gradient.h>

#if 0
#include <math.h>
#include <stdlib.h>


#include <gtk/gtksignal.h>

#include "helper/nr-gradient-gpl.h"
#include "svg/svg.h"
#include "xml/node-private.h"
#include "document-private.h"
#include "sp-object-node.h"

#ifndef SP_MACROS_SILENT
#define SP_MACROS_SILENT
#endif
#include "macros.h"
#endif

#include "macros.h"
#include "attributes.h"
#include "document.h"
#include "factory.h"

#include "sp-gradient.h"

/* Has to be power of 2 */
#define NCOLORS NR_GRADIENT_VECTOR_LENGTH

static void sp_stop_class_init (SPStopClass * klass);
static void sp_stop_init (SPStop * stop);

static void sp_stop_build (SPObject *object, SPDocument *document, TheraNode * node);
static void sp_stop_set (SPObject *object, unsigned int key, const unsigned char *value);
static TheraNode *sp_stop_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags);

static SPObjectClass * stop_parent_class;

GType
sp_stop_get_type (void)
{
	static GType type = 0;
	if (!type) {
		GTypeInfo info = {
			sizeof (SPStopClass),
			NULL, NULL,
			(GClassInitFunc) sp_stop_class_init,
			NULL, NULL,
			sizeof (SPStop),
			16,
			(GInstanceInitFunc) sp_stop_init,
		};
		type = g_type_register_static (SP_TYPE_OBJECT, "SPStop", &info, 0);
	}
	return type;
}

static void
sp_stop_class_init (SPStopClass * klass)
{
	SPObjectClass * sp_object_class;

	sp_object_class = (SPObjectClass *) klass;

	stop_parent_class = g_type_class_ref (SP_TYPE_OBJECT);

	sp_object_class->build = sp_stop_build;
	sp_object_class->set = sp_stop_set;
	sp_object_class->write = sp_stop_write;
}

static void
sp_stop_init (SPStop *stop)
{
	stop->offset = 0.0;
	sp_color_set_rgb_rgba32 (&stop->color, 0x000000ff);
}

static void
sp_stop_build (SPObject * object, SPDocument * document, TheraNode * node)
{
	if (((SPObjectClass *) stop_parent_class)->build)
		(* ((SPObjectClass *) stop_parent_class)->build) (object, document, node);

	sp_object_read_attr (object, "offset");
	sp_object_read_attr (object, "stop-color");
	sp_object_read_attr (object, "stop-opacity");
	sp_object_read_attr (object, "style");
}

static void
sp_stop_set (SPObject *object, unsigned int key, const unsigned char *value)
{
	SPStop *stop;
	guint32 color;
	gdouble opacity;
	const guchar *p;

	stop = SP_STOP (object);

	switch (key) {
	case SP_ATTR_STYLE:
		/* fixme: We are reading simple values 3 times during build (Lauris) */
		/* fixme: We need presentation attributes etc. */
		p = sp_object_get_style_property (object, "stop-color", "black");
		color = sp_svg_read_color (p, sp_color_get_rgba32_ualpha (&stop->color, 0x00));
		sp_color_set_rgb_rgba32 (&stop->color, color);
		p = sp_object_get_style_property (object, "stop-opacity", "1");
		opacity = sp_svg_read_percentage (p, stop->opacity);
		stop->opacity = opacity;
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_STYLE_MODIFIED_FLAG);
		break;
	case SP_PROP_STOP_COLOR:
		/* fixme: We need presentation attributes etc. */
		p = sp_object_get_style_property (object, "stop-color", "black");
		color = sp_svg_read_color (p, sp_color_get_rgba32_ualpha (&stop->color, 0x00));
		sp_color_set_rgb_rgba32 (&stop->color, color);
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_STYLE_MODIFIED_FLAG);
		break;
	case SP_PROP_STOP_OPACITY:
		/* fixme: We need presentation attributes etc. */
		p = sp_object_get_style_property (object, "stop-opacity", "1");
		opacity = sp_svg_read_percentage (p, stop->opacity);
		stop->opacity = opacity;
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_STYLE_MODIFIED_FLAG);
		break;
	case SP_ATTR_OFFSET:
		stop->offset = sp_svg_read_percentage (value, 0.0);
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	default:
		if (((SPObjectClass *) stop_parent_class)->set)
			(* ((SPObjectClass *) stop_parent_class)->set) (object, key, value);
		break;
	}
}

static TheraNode *
sp_stop_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags)
{
	SPStop *stop;
	unsigned char c0[64], c1[32], s[1024];

	stop = SP_STOP (object);

	if ((flags & SP_OBJECT_WRITE_BUILD) && !node) {
		node = thera_document_new_node (thedoc, THERA_NODE_ELEMENT, "stop");
	}

	sp_svg_write_color (c0, 64, sp_color_get_rgba32_ualpha (&stop->color, 255));
	sp_svg_number_write_d (c1, stop->opacity, 4, 2, FALSE);
	g_snprintf (s, 1024, "stop-color:%s;stop-opacity:%s;", c0, c1);
	thera_node_set_attribute (node, "style", s);
	thera_node_set_attribute (node, "stop-color", NULL);
	thera_node_set_attribute (node, "stop-opacity", NULL);
	thera_node_set_double (node, "offset", stop->offset);

	if (((SPObjectClass *) stop_parent_class)->write)
		(* ((SPObjectClass *) stop_parent_class)->write) (object, thedoc, node, flags);

	return node;
}

/*
 * Gradient
 */

static void sp_gradient_class_init (SPGradientClass *klass);
static void sp_gradient_init (SPGradient *gr);

static void sp_gradient_build (SPObject *object, SPDocument *document, TheraNode *node);
static void sp_gradient_release (SPObject *object);
static void sp_gradient_set (SPObject *object, unsigned int key, const unsigned char *value);
static SPObject *sp_gradient_child_added (SPObject *object, TheraNode *child, TheraNode *ref);
static unsigned int sp_gradient_remove_child (SPObject *object, TheraNode *child);
static void sp_gradient_modified (SPObject *object, guint flags);
static unsigned int sp_gradient_sequence (SPObject *object, SPObject *target, unsigned int *seq);
static TheraNode *sp_gradient_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags);

static void sp_gradient_href_release (SPObject *href, SPGradient *gradient);
static void sp_gradient_href_modified (SPObject *href, guint flags, SPGradient *gradient);

static void sp_gradient_invalidate_vector (SPGradient *gr);
static void sp_gradient_rebuild_vector (SPGradient *gr);

static SPPaintServerClass * gradient_parent_class;

GType
sp_gradient_get_type (void)
{
	static GType gradient_type = 0;
	if (!gradient_type) {
		GTypeInfo gradient_info = {
			sizeof (SPGradientClass),
			NULL, NULL,
			(GClassInitFunc) sp_gradient_class_init,
			NULL, NULL,
			sizeof (SPGradient),
			16,
			(GInstanceInitFunc) sp_gradient_init,
		};
		gradient_type = g_type_register_static (SP_TYPE_PAINT_SERVER, "SPGradient", &gradient_info, 0);
	}
	return gradient_type;
}

static void
sp_gradient_class_init (SPGradientClass *klass)
{
	GObjectClass *gobject_class;
	SPObjectClass *sp_object_class;

	gobject_class = (GObjectClass *) klass;
	sp_object_class = (SPObjectClass *) klass;

	gradient_parent_class = g_type_class_ref (SP_TYPE_PAINT_SERVER);

	sp_object_class->build = sp_gradient_build;
	sp_object_class->release = sp_gradient_release;
	sp_object_class->set = sp_gradient_set;
	sp_object_class->child_added = sp_gradient_child_added;
	sp_object_class->remove_child = sp_gradient_remove_child;
	sp_object_class->modified = sp_gradient_modified;
	sp_object_class->sequence = sp_gradient_sequence;
	sp_object_class->write = sp_gradient_write;
}

static void
sp_gradient_init (SPGradient *gr)
{
	SPObject *object;
	object = SP_OBJECT(gr);
	object->has_children = 1;

	/* fixme: There is one problem - if reprs are rearranged, state has to be cleared somehow */
	/* fixme: Maybe that is not problem at all, as no force can rearrange childrens of <defs> */
	/* fixme: But keep that in mind, if messing with XML tree (Lauris) */
	gr->state = SP_GRADIENT_STATE_UNKNOWN;

	gr->units = SP_GRADIENT_UNITS_OBJECTBOUNDINGBOX;
	nr_matrix_f_set_identity (&gr->transform);

	gr->spread = NR_GRADIENT_SPREAD_PAD;
	gr->spread_set = FALSE;

	gr->has_stops = FALSE;

	gr->vector = NULL;
	gr->color = NULL;

	gr->len = 0.0;
}

static void
sp_gradient_build (SPObject *object, SPDocument *document, TheraNode *node)
{
	SPGradient *gradient;
#if 0
	SPObject *last;
	TheraNode *rchild;
#endif
	SPObject *child;

	gradient = SP_GRADIENT (object);

	if (((SPObjectClass *) gradient_parent_class)->build)
		(* ((SPObjectClass *) gradient_parent_class)->build) (object, document, node);

	sp_object_read_attr (object, "gradientUnits");
	sp_object_read_attr (object, "gradientTransform");
	sp_object_read_attr (object, "spreadMethod");
	sp_object_read_attr (object, "xlink:href");

#if 0
	/* fixme: Add all children, not only stops? */
	last = NULL;
	for (rchild = thera_node_get_first_child (node); rchild != NULL; rchild = thera_node_get_next_sibling (rchild)) {
		GType type;
		SPObject *child;
		type = sp_node_type_lookup (rchild);
		if (g_type_is_a (type, SP_TYPE_OBJECT)) {
			child = g_object_new(type, 0);
			if (last) {
				last->next = sp_object_attach_reref (object, child, NULL);
			} else {
				object->children = sp_object_attach_reref (object, child, NULL);
			}
			sp_object_invoke_build (child, document, rchild, object, last, SP_OBJECT_IS_CLONED (object));
			/* Set has_stops flag */
			if (SP_IS_STOP (child)) gradient->has_stops = TRUE;
			last = child;
		}
	}
#endif
	for (child = object->children; child; child = child->next) {
		if (SP_IS_STOP(child)) {
			gradient->has_stops = TRUE;
			break;
		}
	}

	/* Register ourselves */
	sp_document_add_resource (document, "gradient", object);
}

static void
sp_gradient_release (SPObject *object)
{
	SPGradient *gradient;

	gradient = (SPGradient *) object;

#ifdef SP_GRADIENT_VERBOSE
	g_print ("Releasing gradient %s\n", SP_OBJECT_ID (object));
#endif

	if (SP_OBJECT_DOCUMENT (object)) {
		/* Unregister ourselves */
		sp_document_remove_resource (SP_OBJECT_DOCUMENT (object), "gradient", SP_OBJECT (object));
	}

	if (gradient->href) {
		sp_signal_disconnect_by_data (gradient->href, gradient);
		gradient->href = (SPGradient *) sp_object_hunref (SP_OBJECT (gradient->href), object);
	}

	if (gradient->color) {
		g_free (gradient->color);
		gradient->color = NULL;
	}

	if (gradient->vector) {
		g_free (gradient->vector);
		gradient->vector = NULL;
	}

#if 0
	while (object->children) {
		object->children = sp_object_detach_unref (object, object->children);
	}
#endif

	if (((SPObjectClass *) gradient_parent_class)->release)
		((SPObjectClass *) gradient_parent_class)->release (object);
}

static void
sp_gradient_set (SPObject *object, unsigned int key, const unsigned char *value)
{
	SPGradient *gr;

	gr = SP_GRADIENT (object);

	/* fixme: We should unset properties, if val == NULL */
	switch (key) {
	case SP_ATTR_GRADIENTUNITS:
		if (value) {
			if (!strcmp (value, "userSpaceOnUse")) {
				gr->units = SP_GRADIENT_UNITS_USERSPACEONUSE;
			} else {
				gr->units = SP_GRADIENT_UNITS_OBJECTBOUNDINGBOX;
			}
			gr->units_set = TRUE;
		} else {
			gr->units_set = FALSE;
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_GRADIENTTRANSFORM: {
		if (value && sp_svg_transform_read (value, &gr->transform)) {
			gr->transform_set = TRUE;
		} else {
			nr_matrix_f_set_identity (&gr->transform);
			gr->transform_set = FALSE;
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	}
	case SP_ATTR_SPREADMETHOD:
		if (value) {
			if (!strcmp (value, "reflect")) {
				gr->spread = NR_GRADIENT_SPREAD_REFLECT;
			} else if (!strcmp (value, "repeat")) {
				gr->spread = NR_GRADIENT_SPREAD_REPEAT;
			} else {
				gr->spread = NR_GRADIENT_SPREAD_PAD;
			}
			gr->spread_set = TRUE;
		} else {
			gr->spread_set = FALSE;
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_XLINK_HREF:
		if (gr->href) {
 			sp_signal_disconnect_by_data (gr->href, gr);
			gr->href = (SPGradient *) sp_object_hunref (SP_OBJECT (gr->href), object);
		}
		if (value && *value == '#') {
			SPObject *href;
			href = sp_document_lookup_id (object->document, value + 1);
			if (SP_IS_GRADIENT (href)) {
				gr->href = (SPGradient *) sp_object_href (href, object);
				g_signal_connect (G_OBJECT (href), "release", G_CALLBACK (sp_gradient_href_release), gr);
				g_signal_connect (G_OBJECT (href), "modified", G_CALLBACK (sp_gradient_href_modified), gr);
			}
		}
		sp_gradient_invalidate_vector (gr);
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	default:
		if (((SPObjectClass *) gradient_parent_class)->set)
			((SPObjectClass *) gradient_parent_class)->set (object, key, value);
		break;
	}
}

static SPObject *
sp_gradient_child_added (SPObject *object, TheraNode *thechild, TheraNode *ref)
{
	SPGradient *gr;
	SPObject *child;
#if 0
	GType type;
	SPObject * ochild, * prev;
#endif

	gr = SP_GRADIENT (object);

	if (((SPObjectClass *) gradient_parent_class)->child_added)
		child = ((SPObjectClass *) gradient_parent_class)->child_added (object, thechild, ref);

	sp_gradient_invalidate_vector (gr);

#if 0
	type = sp_node_type_lookup (child);
	ochild = g_object_new(type, 0);
	ochild->parent = object;

	prev = NULL;
	if (ref) {
		prev = object->children;
		while (prev->node != ref) prev = prev->next;
	}

	if (!prev) {
		ochild->next = object->children;
		object->children = ochild;
	} else {
		ochild->next = prev->next;
		prev->next = ochild;
	}

	sp_object_invoke_build (ochild, object->document, child, SP_OBJECT_IS_CLONED (object));
#endif

	/* fixme: (Lauris) */
	if (SP_IS_STOP (child)) gr->has_stops = TRUE;

	return child;
}

static unsigned int
sp_gradient_remove_child (SPObject *object, TheraNode *thechild)
{
	SPGradient *gr;
	SPObject *child;
#if 0
	SPObject *prev, *ochild;
	unsigned int ret;
#endif

	gr = SP_GRADIENT (object);

	if (((SPObjectClass *) gradient_parent_class)->remove_child) {
		unsigned int ret;
		ret = ((SPObjectClass *) gradient_parent_class)->remove_child (object, thechild);
		if (!ret) return ret;
	}

	sp_gradient_invalidate_vector (gr);

#if 0
	prev = NULL;
	ochild = object->children;
	while (ochild->node != child) {
		prev = ochild;
		ochild = ochild->next;
	}

	if (prev) {
		prev->next = ochild->next;
	} else {
		object->children = ochild->next;
	}

	ochild->parent = NULL;
	ochild->next = NULL;
	g_object_unref (G_OBJECT (ochild));
#endif

	/* fixme: (Lauris) */
	gr->has_stops = FALSE;
	for (child = object->children; child; child = child->next) {
		if (SP_IS_STOP (child)) {
			gr->has_stops = TRUE;
			break;
		}
	}

	return TRUE;
}

static void
sp_gradient_modified (SPObject *object, guint flags)
{
	SPGradient *gr;
	SPObject *child;
	GSList *l;

	gr = SP_GRADIENT (object);

	if (flags & SP_OBJECT_CHILD_MODIFIED_FLAG) {
		sp_gradient_invalidate_vector (gr);
	}

	if (flags & SP_OBJECT_MODIFIED_FLAG) flags |= SP_OBJECT_PARENT_MODIFIED_FLAG;
	flags &= SP_OBJECT_MODIFIED_CASCADE;

	l = NULL;
	for (child = object->children; child != NULL; child = child->next) {
		g_object_ref (G_OBJECT (child));
		l = g_slist_prepend (l, child);
	}
	l = g_slist_reverse (l);
	while (l) {
		child = SP_OBJECT (l->data);
		l = g_slist_remove (l, child);
		if (flags || (child->mflags & (SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_CHILD_MODIFIED_FLAG))) {
			sp_object_invoke_modified (child, flags);
		}
		g_object_unref (G_OBJECT (child));
	}
}

static unsigned int
sp_gradient_sequence (SPObject *object, SPObject *target, unsigned int *seq)
{
	SPGradient *gr;
	SPObject *child;

	gr = (SPGradient *) object;

	for (child = object->children; child; child = child->next) {
		*seq += 1;
		if (sp_object_invoke_sequence (child, target, seq)) return TRUE;
	}

	return FALSE;
}

static TheraNode *
sp_gradient_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags)
{
	SPGradient *gr;

	gr = SP_GRADIENT (object);

	/* fixme: Why here- shouldn't we create node first? */
	if (((SPObjectClass *) gradient_parent_class)->write)
		(* ((SPObjectClass *) gradient_parent_class)->write) (object, thedoc, node, flags);

	if (flags & SP_OBJECT_WRITE_BUILD) {
		SPObject *child;
		GSList *l;
		l = NULL;
		for (child = object->children; child != NULL; child = child->next) {
			TheraNode *crepr;
			crepr = sp_object_invoke_write (child, thedoc, NULL, flags);
			if (crepr) l = g_slist_prepend (l, crepr);
		}
		while (l) {
			thera_node_add_child (node, (TheraNode *) l->data, NULL);
			l = g_slist_remove (l, l->data);
		}
	}

	if (gr->href) {
		unsigned char *str;
		str = g_strdup_printf ("#%s", SP_OBJECT_ID (gr->href));
		thera_node_set_attribute (node, "xlink:href", str);
		g_free (str);
	}

	if ((flags & SP_OBJECT_WRITE_ALL) || gr->units_set) {
		switch (gr->units) {
		case SP_GRADIENT_UNITS_USERSPACEONUSE:
			thera_node_set_attribute (node, "gradientUnits", "userSpaceOnUse");
			break;
		default:
			thera_node_set_attribute (node, "gradientUnits", "objectBoundingBox");
			break;
		}
	}

	if ((flags & SP_OBJECT_WRITE_ALL) || gr->transform_set) {
		gchar c[256];
		if (sp_svg_transform_write (c, 256, &gr->transform)) {
			thera_node_set_attribute (node, "gradientTransform", c);
		} else {
			thera_node_set_attribute (node, "gradientTransform", NULL);
		}
	}

	if ((flags & SP_OBJECT_WRITE_ALL) || gr->spread_set) {
		switch (gr->spread) {
		case NR_GRADIENT_SPREAD_REFLECT:
			thera_node_set_attribute (node, "spreadMethod", "reflect");
			break;
		case NR_GRADIENT_SPREAD_REPEAT:
			thera_node_set_attribute (node, "spreadMethod", "repeat");
			break;
		default:
			thera_node_set_attribute (node, "spreadMethod", "pad");
			break;
		}
	}

	return node;
}

/* Forces vector to be built, if not present (i.e. changed) */

void
sp_gradient_ensure_vector (SPGradient *gradient)
{
	g_return_if_fail (gradient != NULL);
	g_return_if_fail (SP_IS_GRADIENT (gradient));

	if (!gradient->vector) {
		sp_gradient_rebuild_vector (gradient);
	}
}

void
sp_gradient_set_vector (SPGradient *gradient, SPGradientVector *vector)
{
	g_return_if_fail (gradient != NULL);
	g_return_if_fail (SP_IS_GRADIENT (gradient));
	g_return_if_fail (vector != NULL);

	if (gradient->color) {
		g_free (gradient->color);
		gradient->color = NULL;
	}

	if (gradient->vector && (gradient->vector->nstops != vector->nstops)) {
		g_free (gradient->vector);
		gradient->vector = NULL;
	}
	if (!gradient->vector) {
		gradient->vector = g_malloc (sizeof (SPGradientVector) + (vector->nstops - 1) * sizeof (SPGradientStop));
	}
	memcpy (gradient->vector, vector, sizeof (SPGradientVector) + (vector->nstops - 1) * sizeof (SPGradientStop));

	sp_object_request_modified (SP_OBJECT (gradient), SP_OBJECT_MODIFIED_FLAG);
}

void
sp_gradient_set_units (SPGradient *gr, unsigned int units)
{
	if (units != gr->units) {
		gr->units = units;
		gr->units_set = TRUE;
		sp_object_request_modified (SP_OBJECT (gr), SP_OBJECT_MODIFIED_FLAG);
	}
}

void
sp_gradient_set_spread (SPGradient *gr, unsigned int spread)
{
	if (spread != gr->spread) {
		gr->spread = spread;
		gr->spread_set = TRUE;
		sp_object_request_modified (SP_OBJECT (gr), SP_OBJECT_MODIFIED_FLAG);
	}
}

void
sp_gradient_repr_set_vector (SPGradient *gr, TheraNode *node, SPGradientVector *vector)
{
	TheraNode *child;
	GSList *sl, *cl;
	gint i;

	g_return_if_fail (gr != NULL);
	g_return_if_fail (SP_IS_GRADIENT (gr));
	g_return_if_fail (node != NULL);

	/* We have to be careful, as vector may be our own, so construct node list at first */
	cl = NULL;
	if (vector) {
		for (i = 0; i < vector->nstops; i++) {
			TheraNode *child;
			guchar c0[64], c1[32], s[256];
			child = thera_document_new_node (thera_node_get_document (node), THERA_NODE_ELEMENT, "stop");
			thera_node_set_double (child, "offset",
						      vector->stops[i].offset * (vector->end - vector->start) + vector->start);
			sp_svg_write_color (c0, 64, sp_color_get_rgba32_ualpha (&vector->stops[i].color, 0x00));
			sp_svg_number_write_d (c1, vector->stops[i].opacity, 4, 2, FALSE);
			g_snprintf (s, 256, "stop-color:%s;stop-opacity:%s;", c0, c1);
			thera_node_set_attribute (child, "style", s);
			/* Order will be reversed here */
			cl = g_slist_prepend (cl, child);
		}
	}

	/* Now collect stops from original node */
	sl = NULL;
	for (child = thera_node_get_first_child (node); child != NULL; child = thera_node_get_next_sibling (child)) {
		if (!strcmp (thera_node_get_name (child), "stop")) sl = g_slist_prepend (sl, child);
	}
	/* Remove all stops */
	while (sl) {
		/* fixme: This should work, unless we make gradient into generic group */
		thera_node_unparent (sl->data);
		sl = g_slist_remove (sl, sl->data);
	}

	/* And insert new children from list */
	while (cl) {
		thera_node_add_child (node, (TheraNode *) cl->data, NULL);
		cl = g_slist_remove (cl, cl->data);
	}
}

static void
sp_gradient_href_release (SPObject *href, SPGradient *gradient)
{
	gradient->href = (SPGradient *) sp_object_hunref (href, gradient);
	sp_gradient_invalidate_vector (gradient);
	sp_object_request_modified (SP_OBJECT (gradient), SP_OBJECT_MODIFIED_FLAG);
}

static void
sp_gradient_href_modified (SPObject *href, guint flags, SPGradient *gradient)
{
	sp_gradient_invalidate_vector (gradient);
	sp_object_request_modified (SP_OBJECT (gradient), SP_OBJECT_MODIFIED_FLAG);
}

/* Creates normalized color vector */

static void
sp_gradient_invalidate_vector (SPGradient *gr)
{
	if (gr->color) {
		g_free (gr->color);
		gr->color = NULL;
	}

	if (gr->vector) {
		g_free (gr->vector);
		gr->vector = NULL;
	}
}

static void
sp_gradient_rebuild_vector (SPGradient *gr)
{
	SPObject *child;
	SPColor color;
	gfloat opacity;
	gdouble offsets, offsete, offset;
	gboolean oset;
	gint len, vlen, pos;

	len = 0;
	sp_color_set_rgb_rgba32 (&color, 0x00000000);
	opacity = 0.0;
	offsets = offsete = 0.0;
	oset = FALSE;
	for (child = ((SPObject *) gr)->children; child != NULL; child = child->next) {
		if (SP_IS_STOP (child)) {
			SPStop *stop;
			stop = SP_STOP (child);
			if (!oset) {
				oset = TRUE;
				offsets = offsete = stop->offset;
				len += 1;
			} else if (stop->offset > (offsete + 1e-9)) {
				offsete = stop->offset;
				len += 1;
			}
			sp_color_copy (&color, &stop->color);
			opacity = stop->opacity;
		}
	}

	gr->has_stops = (len != 0);

	if ((len == 0) && (gr->href)) {
		/* Copy vector from parent */
		sp_gradient_ensure_vector (gr->href);
		if (!gr->vector || (gr->vector->nstops != gr->href->vector->nstops)) {
			if (gr->vector) g_free (gr->vector);
			gr->vector = g_malloc (sizeof (SPGradientVector) + (gr->href->vector->nstops - 1) * sizeof (SPGradientStop));
			gr->vector->nstops = gr->href->vector->nstops;
		}
		memcpy (gr->vector, gr->href->vector, sizeof (SPGradientVector) + (gr->vector->nstops - 1) * sizeof (SPGradientStop));
		return;
	}

	vlen = MAX (len, 2);

	if (!gr->vector || gr->vector->nstops != vlen) {
		if (gr->vector) g_free (gr->vector);
		gr->vector = g_malloc (sizeof (SPGradientVector) + (vlen - 1) * sizeof (SPGradientStop));
		gr->vector->nstops = vlen;
	}

	if (len < 2) {
		gr->vector->start = 0.0;
		gr->vector->end = 1.0;
		gr->vector->stops[0].offset = 0.0;
		sp_color_copy (&gr->vector->stops[0].color, &color);
		gr->vector->stops[0].opacity = opacity;
		gr->vector->stops[1].offset = 1.0;
		sp_color_copy (&gr->vector->stops[1].color, &color);
		gr->vector->stops[1].opacity = opacity;
		return;
	}

	/* o' = (o - oS) / (oE - oS) */
	gr->vector->start = offsets;
	gr->vector->end = offsete;

	pos = 0;
	offset = offsets;
	gr->vector->stops[0].offset = 0.0;
	for (child = ((SPObject *) gr)->children; child != NULL; child = child->next) {
		if (SP_IS_STOP (child)) {
			SPStop *stop;
			stop = SP_STOP (child);
			if (stop->offset > (offset + 1e-9)) {
				pos += 1;
				gr->vector->stops[pos].offset = (stop->offset - offsets) / (offsete - offsets);
				offset = stop->offset;
			}
			sp_color_copy (&gr->vector->stops[pos].color, &stop->color);
			gr->vector->stops[pos].opacity = stop->opacity;
		}
	}
}

void
sp_gradient_ensure_colors (SPGradient *gr)
{
	gint i;

	if (!gr->vector) {
		sp_gradient_rebuild_vector (gr);
	}

	if (!gr->color) {
		gr->color = g_new (guchar, 4 * NCOLORS);
	}

	for (i = 0; i < gr->vector->nstops - 1; i++) {
		guint32 color;
		gint r0, g0, b0, a0;
		gint r1, g1, b1, a1;
		gint dr, dg, db, da;
		gint r, g, b, a;
		gint o0, o1;
		gint j;
		color = sp_color_get_rgba32_falpha (&gr->vector->stops[i].color, gr->vector->stops[i].opacity);
		r0 = (color >> 24) & 0xff;
		g0 = (color >> 16) & 0xff;
		b0 = (color >> 8) & 0xff;
		a0 = color & 0xff;
		color = sp_color_get_rgba32_falpha (&gr->vector->stops[i + 1].color, gr->vector->stops[i + 1].opacity);
		r1 = (color >> 24) & 0xff;
		g1 = (color >> 16) & 0xff;
		b1 = (color >> 8) & 0xff;
		a1 = color & 0xff;
		o0 = (gint) floor (gr->vector->stops[i].offset * (NCOLORS - 0.001));
		o1 = (gint) floor (gr->vector->stops[i + 1].offset * (NCOLORS - 0.001));
		if (o1 > o0) {
			dr = ((r1 - r0) << 16) / (o1 - o0);
			dg = ((g1 - g0) << 16) / (o1 - o0);
			db = ((b1 - b0) << 16) / (o1 - o0);
			da = ((a1 - a0) << 16) / (o1 - o0);
			r = r0 << 16;
			g = g0 << 16;
			b = b0 << 16;
			a = a0 << 16;
#if 0
			g_print ("from %d to %d: %x %x %x %x\n", o0, o1, dr, dg, db, da);
#endif
			for (j = o0; j < o1 + 1; j++) {
				gr->color[4 * j] = r >> 16;
				gr->color[4 * j + 1] = g >> 16;
				gr->color[4 * j + 2] = b >> 16;
				gr->color[4 * j + 3] = a >> 16;
#if 0
				g_print ("%x\n", gr->color[j]);
#endif
				r += dr;
				g += dg;
				b += db;
				a += da;
			}
		}
	}

	gr->len = gr->vector->end - gr->vector->start;
}

/*
 * Renders gradient vector to buffer
 *
 * len, width, height, rowstride - buffer parameters (1 or 2 dimensional)
 * span - full integer width of requested gradient
 * pos - buffer starting position in span
 *
 * RGB buffer background should be set up before
 */

void
sp_gradient_render_vector_line_rgba (SPGradient *gradient, guchar *buf, gint len, gint pos, gint span)
{
	gint x, idx, didx;

	g_return_if_fail (gradient != NULL);
	g_return_if_fail (SP_IS_GRADIENT (gradient));
	g_return_if_fail (buf != NULL);
	g_return_if_fail (len > 0);
	g_return_if_fail (pos >= 0);
	g_return_if_fail (pos + len <= span);
	g_return_if_fail (span > 0);

	if (!gradient->color) {
		sp_gradient_ensure_colors (gradient);
	}

	idx = (pos * 1024 << 8) / span;
	didx = (1024 << 8) / span;

	for (x = 0; x < len; x++) {
		*buf++ = gradient->color[4 * (idx >> 8)];
		*buf++ = gradient->color[4 * (idx >> 8) + 1];
		*buf++ = gradient->color[4 * (idx >> 8) + 2];
		*buf++ = gradient->color[4 * (idx >> 8) + 3];
		idx += didx;
	}
}

void
sp_gradient_render_vector_line_rgb (SPGradient *gradient, guchar *buf, gint len, gint pos, gint span)
{
	gint x, idx, didx;

	g_return_if_fail (gradient != NULL);
	g_return_if_fail (SP_IS_GRADIENT (gradient));
	g_return_if_fail (buf != NULL);
	g_return_if_fail (len > 0);
	g_return_if_fail (pos >= 0);
	g_return_if_fail (pos + len <= span);
	g_return_if_fail (span > 0);

	if (!gradient->color) {
		sp_gradient_ensure_colors (gradient);
	}

	idx = (pos * 1024 << 8) / span;
	didx = (1024 << 8) / span;

	for (x = 0; x < len; x++) {
		gint r, g, b, a, fc;
		r = gradient->color[4 * (idx >> 8)];
		g = gradient->color[4 * (idx >> 8) + 1];
		b = gradient->color[4 * (idx >> 8) + 2];
		a = gradient->color[4 * (idx >> 8) + 3];
		fc = (r - *buf) * a;
		buf[0] = *buf + ((fc + (fc >> 8) + 0x80) >> 8);
		fc = (g - *buf) * a;
		buf[1] = *buf + ((fc + (fc >> 8) + 0x80) >> 8);
		fc = (b - *buf) * a;
		buf[2] = *buf + ((fc + (fc >> 8) + 0x80) >> 8);
		buf += 3;
		idx += didx;
	}
}

void
sp_gradient_render_vector_block_rgba (SPGradient *gradient, guchar *buf, gint width, gint height, gint rowstride,
				      gint pos, gint span, gboolean horizontal)
{
	g_return_if_fail (gradient != NULL);
	g_return_if_fail (SP_IS_GRADIENT (gradient));
	g_return_if_fail (buf != NULL);
	g_return_if_fail (width > 0);
	g_return_if_fail (height > 0);
	g_return_if_fail (pos >= 0);
	g_return_if_fail ((horizontal && (pos + width <= span)) || (!horizontal && (pos + height <= span)));
	g_return_if_fail (span > 0);

	if (horizontal) {
		gint y;
		sp_gradient_render_vector_line_rgba (gradient, buf, width, pos, span);
		for (y = 1; y < height; y++) {
			memcpy (buf + y * rowstride, buf, 4 * width);
		}
	} else {
		guchar *tmp;
		gint x, y;
		tmp = alloca (4 * height);
		sp_gradient_render_vector_line_rgba (gradient, tmp, height, pos, span);
		for (y = 0; y < height; y++) {
			guchar *b;
			b = buf + y * rowstride;
			for (x = 0; x < width; x++) {
				*b++ = tmp[0];
				*b++ = tmp[1];
				*b++ = tmp[2];
				*b++ = tmp[3];
			}
			tmp += 4;
		}
	}
}

void
sp_gradient_render_vector_block_rgb (SPGradient *gradient, guchar *buf, gint width, gint height, gint rowstride,
				     gint pos, gint span, gboolean horizontal)
{
	g_return_if_fail (gradient != NULL);
	g_return_if_fail (SP_IS_GRADIENT (gradient));
	g_return_if_fail (buf != NULL);
	g_return_if_fail (width > 0);
	g_return_if_fail (height > 0);
	g_return_if_fail (pos >= 0);
	g_return_if_fail ((horizontal && (pos + width <= span)) || (!horizontal && (pos + height <= span)));
	g_return_if_fail (span > 0);

	if (horizontal) {
		guchar *tmp;
		gint x, y;
		tmp = alloca (4 * width);
		sp_gradient_render_vector_line_rgba (gradient, tmp, width, pos, span);
		for (y = 0; y < height; y++) {
			guchar *b, *t;
			b = buf + y * rowstride;
			t = tmp;
			for (x = 0; x < width; x++) {
				gint a, fc;
				a = t[3];
				fc = (t[0] - buf[0]) * a;
				buf[0] = buf[0] + ((fc + (fc >> 8) + 0x80) >> 8);
				fc = (t[1] - buf[1]) * a;
				buf[1] = buf[1] + ((fc + (fc >> 8) + 0x80) >> 8);
				fc = (t[2] - buf[2]) * a;
				buf[2] = buf[2] + ((fc + (fc >> 8) + 0x80) >> 8);
				buf += 3;
				t += 4;
			}
		}
	} else {
		guchar *tmp;
		gint x, y;
		tmp = alloca (4 * height);
		sp_gradient_render_vector_line_rgba (gradient, tmp, height, pos, span);
		for (y = 0; y < height; y++) {
			guchar *b, *t;
			b = buf + y * rowstride;
			t = tmp + 4 * y;
			for (x = 0; x < width; x++) {
				gint a, fc;
				a = t[3];
				fc = (t[0] - buf[0]) * a;
				buf[0] = buf[0] + ((fc + (fc >> 8) + 0x80) >> 8);
				fc = (t[1] - buf[1]) * a;
				buf[1] = buf[1] + ((fc + (fc >> 8) + 0x80) >> 8);
				fc = (t[2] - buf[2]) * a;
				buf[2] = buf[2] + ((fc + (fc >> 8) + 0x80) >> 8);
			}
		}
	}
}

NRMatrixF *
sp_gradient_get_g2d_matrix_f (SPGradient *gr, NRMatrixF *ctm, NRRectF *bbox, NRMatrixF *g2d)
{
	if (gr->units == SP_GRADIENT_UNITS_OBJECTBOUNDINGBOX) {
		NRMatrixF bb2u;

		bb2u.c[0] = bbox->x1 - bbox->x0;
		bb2u.c[1] = 0.0;
		bb2u.c[2] = 0.0;
		bb2u.c[3] = bbox->y1 - bbox->y0;
		bb2u.c[4] = bbox->x0;
		bb2u.c[5] = bbox->y0;

		nr_matrix_multiply_fff (g2d, &bb2u, ctm);
	} else {
		*g2d = *ctm;
	}

	return g2d;
}

NRMatrixF *
sp_gradient_get_gs2d_matrix_f (SPGradient *gr, NRMatrixF *ctm, NRRectF *bbox, NRMatrixF *gs2d)
{
	if (gr->units == SP_GRADIENT_UNITS_OBJECTBOUNDINGBOX) {
		NRMatrixF bb2u, gs2u;

		bb2u.c[0] = bbox->x1 - bbox->x0;
		bb2u.c[1] = 0.0;
		bb2u.c[2] = 0.0;
		bb2u.c[3] = bbox->y1 - bbox->y0;
		bb2u.c[4] = bbox->x0;
		bb2u.c[5] = bbox->y0;

		nr_matrix_multiply_fff (&gs2u, &gr->transform, &bb2u);
		nr_matrix_multiply_fff (gs2d, &gs2u, ctm);
	} else {
		nr_matrix_multiply_fff (gs2d, &gr->transform, ctm);
	}

	return gs2d;
}

void
sp_gradient_set_gs2d_matrix_f (SPGradient *gr, NRMatrixF *ctm, NRRectF *bbox, NRMatrixF *gs2d)
{
	NRMatrixF g2d, d2g, gs2g;

	SP_PRINT_MATRIX ("* GS2D:", gs2d);

	if (gr->units == SP_GRADIENT_UNITS_OBJECTBOUNDINGBOX) {
		NRMatrixF bb2u;

		bb2u.c[0] = bbox->x1 - bbox->x0;
		bb2u.c[1] = 0.0;
		bb2u.c[2] = 0.0;
		bb2u.c[3] = bbox->y1 - bbox->y0;
		bb2u.c[4] = bbox->x0;
		bb2u.c[5] = bbox->y0;

		SP_PRINT_MATRIX ("* BB2U:", &bb2u);

		nr_matrix_multiply_fff (&g2d, &bb2u, ctm);
	} else {
		g2d = *ctm;
	}

	SP_PRINT_MATRIX ("* G2D:", &g2d);

	nr_matrix_f_invert (&d2g, &g2d);
	SP_PRINT_MATRIX ("* D2G:", &d2g);
	SP_PRINT_MATRIX ("* G2D:", &g2d);
	nr_matrix_f_invert (&g2d, &d2g);
	SP_PRINT_MATRIX ("* D2G:", &d2g);
	SP_PRINT_MATRIX ("* G2D:", &g2d);


	nr_matrix_multiply_fff (&gs2g, gs2d, &d2g);
	SP_PRINT_MATRIX ("* GS2G:", &gs2g);

	gr->transform = gs2g;

	gr->transform_set = TRUE;

	sp_object_request_modified (SP_OBJECT (gr), SP_OBJECT_MODIFIED_FLAG);
}

void
sp_gradient_from_position_xy (SPGradient *gr, NRMatrixF *ctm, NRRectF *bbox, NRPointF *p, float x, float y)
{
	NRMatrixF gs2d;

	g_return_if_fail (gr != NULL);
	g_return_if_fail (SP_IS_GRADIENT (gr));
	g_return_if_fail (p != NULL);

	if (gr->units == SP_GRADIENT_UNITS_OBJECTBOUNDINGBOX) {
		NRMatrixF bb2u, gs2u;

		bb2u.c[0] = bbox->x1 - bbox->x0;
		bb2u.c[1] = 0.0;
		bb2u.c[2] = 0.0;
		bb2u.c[3] = bbox->y1 - bbox->y0;
		bb2u.c[4] = bbox->x0;
		bb2u.c[5] = bbox->y0;

		nr_matrix_multiply_fff (&gs2u, &gr->transform, &bb2u);
		nr_matrix_multiply_fff (&gs2d, &gs2u, ctm);
	} else {
		nr_matrix_multiply_fff (&gs2d, &gr->transform, ctm);
	}

	p->x = gs2d.c[0] * x + gs2d.c[2] * y + gs2d.c[4];
	p->y = gs2d.c[1] * x + gs2d.c[3] * y + gs2d.c[5];
}

void
sp_gradient_to_position_xy (SPGradient *gr, NRMatrixF *ctm, NRRectF *bbox, NRPointF *p, float x, float y)
{
	NRMatrixF gs2d, d2gs;

	g_return_if_fail (gr != NULL);
	g_return_if_fail (SP_IS_GRADIENT (gr));
	g_return_if_fail (p != NULL);

	if (gr->units == SP_GRADIENT_UNITS_OBJECTBOUNDINGBOX) {
		NRMatrixF bb2u, gs2u;

		bb2u.c[0] = bbox->x1 - bbox->x0;
		bb2u.c[1] = 0.0;
		bb2u.c[2] = 0.0;
		bb2u.c[3] = bbox->y1 - bbox->y0;
		bb2u.c[4] = bbox->x0;
		bb2u.c[5] = bbox->y0;

		nr_matrix_multiply_fff (&gs2u, &gr->transform, &bb2u);
		nr_matrix_multiply_fff (&gs2d, &gs2u, ctm);
	} else {
		nr_matrix_multiply_fff (&gs2d, &gr->transform, ctm);
	}

	nr_matrix_f_invert (&d2gs, &gs2d);

	p->x = d2gs.c[0] * x + d2gs.c[2] * y + d2gs.c[4];
	p->y = d2gs.c[1] * x + d2gs.c[3] * y + d2gs.c[5];
}

