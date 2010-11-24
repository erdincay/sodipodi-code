#define __NR_RENDER_C__

/*
 * Pixel buffer rendering library
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#include <malloc.h>
#include <stdlib.h>

#include "nr-render.h"

void
nr_renderer_render (NRRenderer *renderer, NRPixBlock *dest, NRPixBlock *mask)
{
	renderer->render (renderer, dest, mask);
}

void
nr_renderer_delete (NRRenderer *renderer)
{
	if (((NRPaintServerClass *) (((ArikkeiObject *) renderer->server)->klass))->release_renderer)
		((NRPaintServerClass *) (((ArikkeiObject *) renderer->server)->klass))->release_renderer (renderer->server, renderer);
	arikkei_object_unref (ARIKKEI_OBJECT(renderer->server));
	free (renderer);
}

static void nr_paint_server_class_init (NRPaintServerClass *klass);
static void nr_paint_server_init (NRPaintServer *server);

static ArikkeiObjectClass *parent_class;

unsigned int
nr_paint_server_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		type = arikkei_object_register_type (ARIKKEI_TYPE_OBJECT,
						(const unsigned char *) "NRPaintServer",
						sizeof (NRPaintServerClass),
						sizeof (NRPaintServer),
						(void (*) (ArikkeiObjectClass *)) nr_paint_server_class_init,
						(void (*) (ArikkeiObject *)) nr_paint_server_init);
	}
	return type;
}

static void
nr_paint_server_class_init (NRPaintServerClass *klass)
{
	parent_class = ((ArikkeiObjectClass *) klass)->parent;
}

static void
nr_paint_server_init (NRPaintServer *server)
{
}

NRRenderer *
nr_paint_server_get_renderer (NRPaintServer *server, const NRMatrixF *transform, const NRRectF *paintbox)
{
	NRRenderer *renderer;
	renderer = NULL;
	if (((NRPaintServerClass *) (((ArikkeiObject *) server)->klass))->new_renderer)
		renderer = ((NRPaintServerClass *) (((ArikkeiObject *) server)->klass))->new_renderer (server, transform, paintbox);
	if (renderer) {
		arikkei_object_ref (ARIKKEI_OBJECT(server));
	}
	return renderer;
}

