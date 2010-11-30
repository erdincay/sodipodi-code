#define __NR_ARENA_C__

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

#include <libnr/nr-rect.h>

#include "nr-arena-item.h"
#include "nr-arena.h"

static void nr_arena_class_init (NRArenaClass *klass);
static void nr_arena_init (NRArena *arena);
static void nr_arena_finalize (ArikkeiObject *object);

static ArikkeiActiveObjectClass *parent_class;

unsigned int
nr_arena_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		type = arikkei_object_register_type (ARIKKEI_TYPE_ACTIVE_OBJECT,
						"NRArena",
						sizeof (NRArenaClass),
						sizeof (NRArena),
						(void (*) (ArikkeiObjectClass *)) nr_arena_class_init,
						(void (*) (ArikkeiObject *)) nr_arena_init);
	}
	return type;
}

static void
nr_arena_class_init (NRArenaClass *klass)
{
	ArikkeiObjectClass *object_class;

	object_class = (ArikkeiObjectClass *) klass;

	parent_class = (ArikkeiActiveObjectClass *) (((ArikkeiObjectClass *) klass)->parent);

	object_class->finalize = nr_arena_finalize;
}

static void
nr_arena_init (NRArena *arena)
{
}

static void
nr_arena_finalize (ArikkeiObject *object)
{
	NRArena *arena;

	arena = NR_ARENA(object);

	((ArikkeiObjectClass *) parent_class)->finalize (object);
}

void
nr_arena_item_added (NRArena *arena, NRArenaItem *parent, NRArenaItem *child)
{
	ArikkeiActiveObject *aobject;
	aobject = (ArikkeiActiveObject *) arena;
	if (aobject->callbacks) {
		unsigned int i;
		for (i = 0; i < aobject->callbacks->length; i++) {
			ArikkeiObjectListener *listener;
			NRArenaEventVector *avector;
			listener = aobject->callbacks->listeners + i;
			avector = (NRArenaEventVector *) listener->vector;
			if ((listener->size >= sizeof (NRArenaEventVector)) && avector->item_added) {
				avector->item_added (arena, parent, child, listener->data);
			}
		}
	}
}

void
nr_arena_item_removed (NRArena *arena, NRArenaItem *parent, NRArenaItem *child)
{
	ArikkeiActiveObject *aobject;
	aobject = (ArikkeiActiveObject *) arena;
	if (aobject->callbacks) {
		unsigned int i;
		for (i = 0; i < aobject->callbacks->length; i++) {
			ArikkeiObjectListener *listener;
			NRArenaEventVector *avector;
			listener = aobject->callbacks->listeners + i;
			avector = (NRArenaEventVector *) listener->vector;
			if ((listener->size >= sizeof (NRArenaEventVector)) && avector->item_removed) {
				avector->item_removed (arena, parent, child, listener->data);
			}
		}
	}
}

void
nr_arena_request_update (NRArena *arena, NRArenaItem *item)
{
	ArikkeiActiveObject *aobject;

	aobject = (ArikkeiActiveObject *) arena;

	arikkei_return_if_fail (arena != NULL);
	arikkei_return_if_fail (NR_IS_ARENA (arena));
	arikkei_return_if_fail (item != NULL);
	arikkei_return_if_fail (NR_IS_ARENA_ITEM (item));

	if (aobject->callbacks) {
		unsigned int i;
		for (i = 0; i < aobject->callbacks->length; i++) {
			ArikkeiObjectListener *listener;
			NRArenaEventVector *avector;
			listener = aobject->callbacks->listeners + i;
			avector = (NRArenaEventVector *) listener->vector;
			if ((listener->size >= sizeof (NRArenaEventVector)) && avector->request_update) {
				avector->request_update (arena, item, listener->data);
			}
		}
	}
}

void
nr_arena_request_render_rect (NRArena *arena, NRRectL *area)
{
	ArikkeiActiveObject *aobject;

	aobject = (ArikkeiActiveObject *) arena;

	arikkei_return_if_fail (arena != NULL);
	arikkei_return_if_fail (NR_IS_ARENA (arena));
	arikkei_return_if_fail (area != NULL);

	if (aobject->callbacks && area && !nr_rect_l_test_empty (area)) {
		unsigned int i;
		for (i = 0; i < aobject->callbacks->length; i++) {
			ArikkeiObjectListener *listener;
			NRArenaEventVector *avector;
			listener = aobject->callbacks->listeners + i;
			avector = (NRArenaEventVector *) listener->vector;
			if ((listener->size >= sizeof (NRArenaEventVector)) && avector->request_render) {
				avector->request_render (arena, area, listener->data);
			}
		}
	}
}

