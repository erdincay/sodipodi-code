#ifndef __SP_PAINT_SERVER_H__
#define __SP_PAINT_SERVER_H__

/*
 * Base class for gradients and patterns
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 1999-2002 Lauris Kaplinski
 * Copyright (C) 2000-2001 Ximian, Inc.
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#define SP_TYPE_ARENA_PAINTER (sp_arena_painter_get_type ())
#define SP_ARENA_PAINTER(o) (ARIKKEI_CHECK_INSTANCE_CAST ((o), SP_TYPE_ARENA_PAINTER, SPArenaPainter))
#define SP_IS_ARENA_PAINTER(o) (ARIKKEI_CHECK_INSTANCE_TYPE ((o), SP_TYPE_ARENA_PAINTER))

typedef struct _SPArenaPainter SPArenaPainter;
typedef struct _SPArenaPainterClass SPArenaPainterClass;

#define SP_TYPE_PAINT_SERVER (sp_paint_server_get_type ())
#define SP_PAINT_SERVER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), SP_TYPE_PAINT_SERVER, SPPaintServer))
#define SP_IS_PAINT_SERVER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SP_TYPE_PAINT_SERVER))

#include <libnr/nr-render.h>

#include <libsodipodi/sp-object.h>

/* SPPainter */

struct _SPArenaPainter {
	NRPaintServer server;

	SPPaintServer *spserver;
	SPArenaPainter *next;
};

struct _SPArenaPainterClass {
	NRPaintServerClass parent_class;
};

unsigned int sp_arena_painter_get_type (void);

/* SPPaintServer */

struct _SPPaintServer {
	SPObject object;

	SPArenaPainter *painters;
};

struct _SPPaintServerClass {
	SPObjectClass sp_object_class;

	SPArenaPainter *(* show) (SPPaintServer *pserver);
	/* This is called with painter already detached from list */
	void (* hide) (SPPaintServer *pserver, SPArenaPainter *painter);
};

GType sp_paint_server_get_type (void);

SPArenaPainter *sp_paint_server_painter_new (SPPaintServer *ps);

#endif
