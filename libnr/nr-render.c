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
	if (((NRPaintServerClass *) (((AZObject *) renderer->server)->klass))->release_renderer)
		((NRPaintServerClass *) (((AZObject *) renderer->server)->klass))->release_renderer (renderer->server, renderer);
	az_object_unref (AZ_OBJECT(renderer->server));
	free (renderer);
}

unsigned int
nr_paint_server_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		az_register_type (&type, (const unsigned char *) "NRPaintServer", AZ_TYPE_OBJECT, sizeof (NRPaintServerClass), sizeof (NRPaintServer), 0,
						NULL, NULL, NULL);
	}
	return type;
}

NRRenderer *
nr_paint_server_get_renderer (NRPaintServer *server, const NRMatrixF *transform, const NRRectF *paintbox)
{
	NRRenderer *renderer;
	renderer = NULL;
	if (((NRPaintServerClass *) (((AZObject *) server)->klass))->new_renderer)
		renderer = ((NRPaintServerClass *) (((AZObject *) server)->klass))->new_renderer (server, transform, paintbox);
	if (renderer) {
		az_object_ref (AZ_OBJECT(server));
	}
	return renderer;
}

