#ifndef __SP_MASK_H__
#define __SP_MASK_H__

/*
 * SVG <mask> implementation
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 2003 authors
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#define SP_TYPE_MASK (sp_mask_get_type ())
#define SP_MASK(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), SP_TYPE_MASK, SPMask))
#define SP_IS_MASK(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SP_TYPE_MASK))

typedef struct _SPMaskView SPMaskView;

#include <libnrarena/nr-arena-forward.h>

#include <libsodipodi/sp-object-group.h>

struct _SPMask {
	SPObjectGroup group;

	unsigned int maskUnits_set : 1;
	unsigned int maskUnits : 1;

	unsigned int maskContentUnits_set : 1;
	unsigned int maskContentUnits : 1;

	SPMaskView *display;
};

struct _SPMaskClass {
	SPObjectGroupClass parent_class;
};

GType sp_mask_get_type (void);

NRArenaItem *sp_mask_show (SPMask *mask, NRArena *arena, unsigned int key);
void sp_mask_hide (SPMask *mask, unsigned int key);

void sp_mask_set_bbox (SPMask *mask, unsigned int key, NRRectF *bbox);

#endif
