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

static ArikkeiObjectClass *parent_class;

unsigned int
nr_paint_server_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		arikkei_register_type (&type, ARIKKEI_TYPE_OBJECT,
						(const unsigned char *) "NRPaintServer",
						sizeof (NRPaintServerClass),
						sizeof (NRPaintServer),
						(void (*) (ArikkeiClass *)) nr_paint_server_class_init,
						NULL, NULL);
	}
	return type;
}

static void
nr_paint_server_class_init (NRPaintServerClass *klass)
{
	parent_class = (ArikkeiObjectClass *) ((ArikkeiClass *) klass)->parent;
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

