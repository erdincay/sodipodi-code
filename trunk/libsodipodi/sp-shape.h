#ifndef __SP_SHAPE_H__
#define __SP_SHAPE_H__

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

#include <libnr/nr-path.h>

#include "sp-item.h"

#define SP_TYPE_SHAPE (sp_shape_get_type ())
#define SP_SHAPE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), SP_TYPE_SHAPE, SPShape))
#define SP_IS_SHAPE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SP_TYPE_SHAPE))

#define SP_SHAPE_WRITE_PATH (1 << 2)

struct _SPShape {
	SPItem item;

	NRPath *curve;

	SPObject *marker_start;
	SPObject *marker_mid;
	SPObject *marker_end;
};

struct _SPShapeClass {
	SPItemClass item_class;

	/* Build bpath from extra shape attributes */
	void (* set_shape) (SPShape *shape);
};

GType sp_shape_get_type (void);

void sp_shape_set_shape (SPShape *shape);

/* Return duplicate of curve or NULL */
NRPath *sp_shape_duplicate_curve (SPShape *shape);

void sp_shape_set_curve (SPShape *shape, NRPath *curve, unsigned int duplicate);

/* NOT FOR GENERAL PUBLIC UNTIL SORTED OUT (Lauris) */
void sp_shape_set_curve_insync (SPShape *shape, NRPath *curve, unsigned int duplicate);

/* PROTECTED */
void sp_shape_set_marker (SPObject *object, unsigned int key, const unsigned char *value);

#endif
