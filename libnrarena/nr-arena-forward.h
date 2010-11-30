#ifndef __NR_ARENA_FORWARD_H__
#define __NR_ARENA_FORWARD_H__

/*
 * RGBA display list system for sodipodi
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 2001 Ximian, Inc.
 * Copyright (C) 2001-2010 Lauris Kaplinski
 *
 * Released under GNU GPL
 */

#include <libnr/nr-image.h>

/* NRArena */

#define NR_TYPE_ARENA (nr_arena_get_type ())
#define NR_ARENA(o) (ARIKKEI_CHECK_INSTANCE_CAST ((o), NR_TYPE_ARENA, NRArena))
#define NR_IS_ARENA(o) (ARIKKEI_CHECK_INSTANCE_TYPE ((o), NR_TYPE_ARENA))

typedef struct _NRArena NRArena;

unsigned int nr_arena_get_type (void);

/* NRArenaStyle */

typedef struct _NRArenaColor NRArenaColor;
typedef struct _NRArenaPaint NRArenaPaint;
typedef struct _NRArenaStyle NRArenaStyle;

/* NRArenaItem */

#define NR_TYPE_ARENA_ITEM (nr_arena_item_get_type ())
#define NR_ARENA_ITEM(o) (ARIKKEI_CHECK_INSTANCE_CAST ((o), NR_TYPE_ARENA_ITEM, NRArenaItem))
#define NR_IS_ARENA_ITEM(o) (ARIKKEI_CHECK_INSTANCE_TYPE ((o), NR_TYPE_ARENA_ITEM))

typedef struct _NRArenaItem NRArenaItem;

unsigned int nr_arena_item_get_type (void);

NRArenaItem *nr_arena_item_new (NRArena *arena, unsigned int type);

/* NRArenaGroup */

typedef struct _NRArenaGroup NRArenaGroup;

typedef struct _NRArenaShape NRArenaShape;

/* NRArenaImage */

#define NR_TYPE_ARENA_IMAGE (nr_arena_image_get_type ())
#define NR_ARENA_IMAGE(o) (ARIKKEI_CHECK_INSTANCE_CAST ((o), NR_TYPE_ARENA_IMAGE, NRArenaImage))
#define NR_IS_ARENA_IMAGE(o) (ARIKKEI_CHECK_INSTANCE_TYPE ((o), NR_TYPE_ARENA_IMAGE))

typedef struct _NRArenaImage NRArenaImage;

unsigned int nr_arena_image_get_type (void);

void nr_arena_image_set_pixels (NRArenaImage *image, NRImage *pixels);
void nr_arena_image_set_geometry (NRArenaImage *image, float x, float y, float width, float height);

/* NRarenaGlyphs */

typedef struct _NRArenaGlyphs NRArenaGlyphs;

#endif
