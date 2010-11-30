#ifndef __NR_ARENA_IMAGE_H__
#define __NR_ARENA_IMAGE_H__

/*
 * RGBA display list system for sodipodi
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 2001-2002 Lauris Kaplinski
 * Copyright (C) 2001 Ximian, Inc.
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

typedef struct _NRArenaImageClass NRArenaImageClass;

#include <libnr/nr-types.h>

#include "nr-arena-item.h"

struct _NRArenaImage {
	NRArenaItem item;

	NRImage *pixels;

	float x, y;
	float width, height;

	/* From GRID to PIXELS */
	NRMatrixF grid2px;
};

struct _NRArenaImageClass {
	NRArenaItemClass parent_class;
};

#endif
