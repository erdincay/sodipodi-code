#define __SP_ANCHOR_C__

/*
 * SVG <a> element implementation
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 2001-2002 Lauris Kaplinski
 * Copyright (C) 2001 Ximian, Inc.
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#define noSP_ANCHOR_VERBOSE

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "intl.h"
#include "attributes.h"

#include "sp-anchor.h"

static void sp_anchor_class_init (SPAnchorClass *klass);
static void sp_anchor_init (SPAnchor *anchor);

static void sp_anchor_build (SPObject * object, SPDocument *document, TheraNode *node);
static void sp_anchor_release (SPObject *object);
static void sp_anchor_set (SPObject *object, unsigned int key, const unsigned char *value);
static TheraNode *sp_anchor_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags);

static gchar *sp_anchor_description (SPItem *item);
static gint sp_anchor_event (SPItem *item, SPEvent *event);

static SPGroupClass *parent_class;

GType
sp_anchor_get_type (void)
{
	static GType type = 0;
	if (!type) {
		GTypeInfo info = {
			sizeof (SPAnchorClass),
			NULL, NULL,
			(GClassInitFunc) sp_anchor_class_init,
			NULL, NULL,
			sizeof (SPAnchor),
			16,
			(GInstanceInitFunc) sp_anchor_init,
		};
		type = g_type_register_static (SP_TYPE_GROUP, "SPAnchor", &info, 0);
	}
	return type;
}

static void
sp_anchor_class_init (SPAnchorClass *klass)
{
	GObjectClass *gobject_class;
	SPObjectClass *sp_object_class;
	SPItemClass *item_class;

	gobject_class = (GObjectClass *) klass;
	sp_object_class = (SPObjectClass *) klass;
	item_class = (SPItemClass *) klass;

	parent_class = g_type_class_ref (SP_TYPE_GROUP);

	sp_object_class->build = sp_anchor_build;
	sp_object_class->release = sp_anchor_release;
	sp_object_class->set = sp_anchor_set;
	sp_object_class->write = sp_anchor_write;

	item_class->description = sp_anchor_description;
	item_class->event = sp_anchor_event;
}

static void
sp_anchor_init (SPAnchor *anchor)
{
	anchor->href = NULL;
}

static void sp_anchor_build (SPObject *object, SPDocument * document, TheraNode * node)
{
	SPAnchor *anchor;

	anchor = SP_ANCHOR (object);

	if (((SPObjectClass *) (parent_class))->build)
		((SPObjectClass *) (parent_class))->build (object, document, node);

	sp_object_read_attribute (object, SP_ATTR_XLINK_TYPE);
	sp_object_read_attribute (object, SP_ATTR_XLINK_ROLE);
	sp_object_read_attribute (object, SP_ATTR_XLINK_ARCROLE);
	sp_object_read_attribute (object, SP_ATTR_XLINK_TITLE);
	sp_object_read_attribute (object, SP_ATTR_XLINK_SHOW);
	sp_object_read_attribute (object, SP_ATTR_XLINK_ACTUATE);
	sp_object_read_attribute (object, SP_ATTR_XLINK_HREF);
	sp_object_read_attribute (object, SP_ATTR_TARGET);
}

static void
sp_anchor_release (SPObject *object)
{
	SPAnchor *anchor;

	anchor = SP_ANCHOR (object);

	if (anchor->href) {
		g_free (anchor->href);
		anchor->href = NULL;
	}

	if (((SPObjectClass *) parent_class)->release)
		((SPObjectClass *) parent_class)->release (object);
}

static void
sp_anchor_set (SPObject *object, unsigned int key, const unsigned char *value)
{
	SPAnchor *anchor;

	anchor = SP_ANCHOR (object);

	switch (key) {
	case SP_ATTR_XLINK_HREF:
		if (anchor->href) g_free (anchor->href);
		anchor->href = g_strdup (value);
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_XLINK_TYPE:
	case SP_ATTR_XLINK_ROLE:
	case SP_ATTR_XLINK_ARCROLE:
	case SP_ATTR_XLINK_TITLE:
	case SP_ATTR_XLINK_SHOW:
	case SP_ATTR_XLINK_ACTUATE:
	case SP_ATTR_TARGET:
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	default:
		if (((SPObjectClass *) (parent_class))->set)
			((SPObjectClass *) (parent_class))->set (object, key, value);
		break;
	}
}

#define COPY_ATTR(rd,rs,key) thera_node_set_attribute ((rd), (key), thera_node_get_attribute (rs, key));


static TheraNode *
sp_anchor_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags)
{
	SPAnchor *anchor;

	anchor = SP_ANCHOR (object);

	if ((flags & SP_OBJECT_WRITE_BUILD) && !node) {
		node = thera_document_new_node (thedoc, THERA_NODE_ELEMENT, "a");
	}

	thera_node_set_attribute (node, "xlink:href", anchor->href);

	if (node != SP_OBJECT_REPR (object)) {
		COPY_ATTR (node, object->node, "xlink:type");
		COPY_ATTR (node, object->node, "xlink:role");
		COPY_ATTR (node, object->node, "xlink:arcrole");
		COPY_ATTR (node, object->node, "xlink:title");
		COPY_ATTR (node, object->node, "xlink:show");
		COPY_ATTR (node, object->node, "xlink:actuate");
		COPY_ATTR (node, object->node, "target");
	}

	if (((SPObjectClass *) (parent_class))->write)
		((SPObjectClass *) (parent_class))->write (object, thedoc, node, flags);

	return node;
}

static gchar *
sp_anchor_description (SPItem *item)
{
	SPAnchor * anchor;
	static char c[128];

	anchor = SP_ANCHOR (item);

	g_snprintf (c, 128, _("Link to %s"), anchor->href);

	return g_strdup (c);
}

/* fixme: We should forward event to appropriate container/view */

static gint
sp_anchor_event (SPItem *item, SPEvent *event)
{
#if 0
	SPAnchor *anchor;
	/* fixme: */
	SPSVGView *svgview;
	GdkCursor *cursor;

	anchor = SP_ANCHOR (item);

	switch (event->type) {
	case SP_EVENT_ACTIVATE:
		if (anchor->href) {
			g_print ("Activated xlink:href=\"%s\"\n", anchor->href);
#if 0
			gnome_url_show (anchor->href, NULL);
#endif
			return TRUE;
		}
		break;
	case SP_EVENT_MOUSEOVER:
		/* fixme: */
		if (SP_IS_SVG_VIEW (event->data)) {
			svgview = event->data;
			cursor = gdk_cursor_new (GDK_HAND2);
			gdk_window_set_cursor (GTK_WIDGET (SP_CANVAS_ITEM (svgview->drawing)->canvas)->window, cursor);
			gdk_cursor_destroy (cursor);
		}
		break;
	case SP_EVENT_MOUSEOUT:
		/* fixme: */
		if (SP_IS_SVG_VIEW (event->data)) {
			svgview = event->data;
			gdk_window_set_cursor (GTK_WIDGET (SP_CANVAS_ITEM (svgview->drawing)->canvas)->window, NULL);
		}
		break;
	default:
		break;
	}
#endif
	return FALSE;
}

