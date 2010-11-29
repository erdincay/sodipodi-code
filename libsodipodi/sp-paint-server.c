#define __SP_PAINT_SERVER_C__

/*
 * Base class for gradients and patterns
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 1999-2010 Lauris Kaplinski
 * Copyright (C) 2000-2001 Ximian, Inc.
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#include <libnr/nr-pixblock-pattern.h>

#include "sp-paint-server.h"

/* This is called from NRArenaPainter::finalize to detach links */
static void sp_paint_server_painter_detach (SPPaintServer *ps, SPArenaPainter *painter);

/* SPPainter */

static void sp_arena_painter_class_init (SPArenaPainterClass *klass);
static void sp_arena_painter_init (SPArenaPainter *painter);
static void sp_arena_painter_finalize (ArikkeiObject *object);

static NRPaintServerClass *painter_parent_class;

static GSList *stale_painters = NULL;

unsigned int
sp_arena_painter_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		type = arikkei_object_register_type (NR_TYPE_PAINT_SERVER,
						(const unsigned char *) "SPArenaPainter",
						sizeof (SPArenaPainterClass),
						sizeof (SPArenaPainter),
						(void (*) (ArikkeiObjectClass *)) sp_arena_painter_class_init,
						(void (*) (ArikkeiObject *)) sp_arena_painter_init);
	}
	return type;
}

static void
sp_arena_painter_class_init (SPArenaPainterClass *klass)
{
	ArikkeiObjectClass *object_class;

	object_class = (ArikkeiObjectClass *) klass;

	painter_parent_class = (NRPaintServerClass *) ((ArikkeiObjectClass *) klass)->parent;

	object_class->finalize = sp_arena_painter_finalize;
}

static void
sp_arena_painter_init (SPArenaPainter *painter)
{
}

static void
sp_arena_painter_finalize (ArikkeiObject *object)
{
	SPArenaPainter *painter;

	painter = SP_ARENA_PAINTER(object);

	if (painter->spserver) {
		sp_paint_server_painter_detach (painter->spserver, painter);
	} else {
		stale_painters = g_slist_remove (stale_painters, object);
	}

	if (((ArikkeiObjectClass *) painter_parent_class)->finalize)
		((ArikkeiObjectClass *) painter_parent_class)->finalize (object);
}

/* SPPaintServer */

static void sp_paint_server_class_init (SPPaintServerClass *klass);
static void sp_paint_server_init (SPPaintServer *ps);

static void sp_paint_server_release (SPObject *object);

static SPObjectClass *parent_class;

GType
sp_paint_server_get_type (void)
{
	static GType type = 0;
	if (!type) {
		GTypeInfo info = {
			sizeof (SPPaintServerClass),
			NULL,	/* base_init */
			NULL,	/* base_finalize */
			(GClassInitFunc) sp_paint_server_class_init,
			NULL,	/* class_finalize */
			NULL,	/* class_data */
			sizeof (SPPaintServer),
			16,	/* n_preallocs */
			(GInstanceInitFunc) sp_paint_server_init,
		};
		type = g_type_register_static (SP_TYPE_OBJECT, "SPPaintServer", &info, 0);
	}
	return type;
}

static void
sp_paint_server_class_init (SPPaintServerClass *klass)
{
	SPObjectClass *sp_object_class;

	sp_object_class = (SPObjectClass *) klass;

	parent_class = g_type_class_ref (SP_TYPE_OBJECT);

	sp_object_class->release = sp_paint_server_release;
}

static void
sp_paint_server_init (SPPaintServer *ps)
{
}

static void
sp_paint_server_release (SPObject *object)
{
	SPPaintServer *pserver;

	pserver = SP_PAINT_SERVER(object);

	while (pserver->painters) {
		SPArenaPainter *painter;
		painter = pserver->painters;
		pserver->painters = painter->next;
		painter->next = NULL;
		painter->spserver = NULL;
		stale_painters = g_slist_prepend (stale_painters, painter);
	}

	if (((SPObjectClass *) parent_class)->release)
		((SPObjectClass *) parent_class)->release (object);
}

SPArenaPainter *
sp_paint_server_painter_new (SPPaintServer *ps)
{
	SPArenaPainter *painter;

	g_return_val_if_fail (ps != NULL, NULL);
	g_return_val_if_fail (SP_IS_PAINT_SERVER (ps), NULL);

	painter = NULL;

	if (((SPPaintServerClass *) G_OBJECT_GET_CLASS(ps))->show)
		painter = (* ((SPPaintServerClass *) G_OBJECT_GET_CLASS(ps))->show) (ps);

	if (painter) {
		painter->next = ps->painters;
		ps->painters = painter;
		painter->spserver = ps;
	}

	return painter;
}

static void
sp_paint_server_painter_detach (SPPaintServer *ps, SPArenaPainter *painter)
{
	SPArenaPainter *p, *r;

	r = NULL;
	for (p = ps->painters; p != NULL; p = p->next) {
		if (p == painter) {
			if (r) {
				r->next = p->next;
			} else {
				ps->painters = p->next;
			}
			p->next = NULL;
			p->spserver = NULL;
			if (((SPPaintServerClass *) G_OBJECT_GET_CLASS(ps))->hide)
				((SPPaintServerClass *) G_OBJECT_GET_CLASS(ps))->hide (ps, painter);
			break;
		}
		r = p;
	}
}

