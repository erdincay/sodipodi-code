#ifndef __NR_ARENA_H__
#define __NR_ARENA_H__

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

#include <libarikkei/arikkei-object.h>

#include <libnrarena/nr-arena-forward.h>

typedef struct _NRArenaClass NRArenaClass;
typedef struct _NRArenaEventVector NRArenaEventVector;

struct _NRArenaEventVector {
	ArikkeiObjectEventVector object_vector;
	void (* item_added) (NRArena *arena, NRArenaItem *parent, NRArenaItem *child, void *data);
	void (* item_removed) (NRArena *arena, NRArenaItem *parent, NRArenaItem *child, void *data);
	void (* request_update) (NRArena *arena, NRArenaItem *item, void *data);
	void (* request_render) (NRArena *arena, NRRectL *area, void *data);
};

struct _NRArena {
	ArikkeiActiveObject object;
};

struct _NRArenaClass {
	ArikkeiActiveObjectClass parent_class;
};

/* Following are meant stricktly for subclass/item implementations */
void nr_arena_item_added (NRArena *arena, NRArenaItem *parent, NRArenaItem *child);
void nr_arena_item_removed (NRArena *arena, NRArenaItem *parent, NRArenaItem *child);

void nr_arena_request_update (NRArena *arena, NRArenaItem *item);
void nr_arena_request_render_rect (NRArena *arena, NRRectL *area);

#endif
