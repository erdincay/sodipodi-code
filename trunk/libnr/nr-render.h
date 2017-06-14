#ifndef __NR_RENDER_H__
#define __NR_RENDER_H__

/*
 * Pixel buffer rendering library
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#define NR_TYPE_PAINT_SERVER (nr_paint_server_get_type ())
#define NR_PAINT_SERVER(o) (ARIKKEI_CHECK_INSTANCE_CAST ((o), NR_TYPE_PAINT_SERVER, NRPaintServer))
#define NR_IS_PAINT_SERVER(o) (ARIKKEI_CHECK_INSTANCE_TYPE ((o), NR_TYPE_PAINT_SERVER))

typedef struct _NRPaintServer NRPaintServer;
typedef struct _NRPaintServerClass NRPaintServerClass;

typedef struct _NRRenderer NRRenderer;

#include <az/object.h>

#include <libnr/nr-pixblock.h>

struct _NRRenderer {
	NRPaintServer *server;
	void (* render) (NRRenderer *renderer, NRPixBlock *dest, NRPixBlock *mask);
};

void nr_renderer_render (NRRenderer *renderer, NRPixBlock *dest, NRPixBlock *mask);
void nr_renderer_delete (NRRenderer *renderer);

struct _NRPaintServer {
	AZObject object;
};

struct _NRPaintServerClass {
	AZObjectClass parent_class;

	NRRenderer *(* new_renderer) (NRPaintServer *server, const NRMatrixF *transform, const NRRectF *paintbox);
	/* This has to free allocated resources but not the actual object */
	void (* release_renderer) (NRPaintServer *server, NRRenderer *renderer);
};

unsigned int nr_paint_server_get_type (void);

NRRenderer *nr_paint_server_get_renderer (NRPaintServer *server, const NRMatrixF *transform, const NRRectF *paintbox);

#endif
