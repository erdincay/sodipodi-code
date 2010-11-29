#ifndef __SP_ANCHOR_H__
#define __SP_ANCHOR_H__

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

#define SP_TYPE_ANCHOR (sp_anchor_get_type ())
#define SP_ANCHOR(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), SP_TYPE_ANCHOR, SPAnchor))
#define SP_IS_ANCHOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SP_TYPE_ANCHOR))

#include "sp-item-group.h"

struct _SPAnchor {
	SPGroup group;

	unsigned char *href;
};

struct _SPAnchorClass {
	SPGroupClass parent_class;
};

GType sp_anchor_get_type (void);

#endif
