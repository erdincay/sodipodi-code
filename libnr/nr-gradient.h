#ifndef __NR_GRADIENT_H__
#define __NR_GRADIENT_H__

/*
 * Pixel buffer rendering library
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

typedef struct _NRLGradientRenderer NRLGradientRenderer;
typedef struct _NRRGradientRenderer NRRGradientRenderer;

#include <libnr/nr-types.h>
#include <libnr/nr-render.h>

#define NR_GRADIENT_VECTOR_LENGTH 1024

enum {
	NR_GRADIENT_SPREAD_PAD,
	NR_GRADIENT_SPREAD_REFLECT,
	NR_GRADIENT_SPREAD_REPEAT
};

/* Linear */

struct _NRLGradientRenderer {
	NRRenderer renderer;
	const unsigned char *vector;
	unsigned int spread;
	int x0, y0;
	float dx, dy;
};

void nr_lgradient_renderer_setup (NRLGradientRenderer *renderer, NRPaintServer *server,
								  const unsigned char *vector, unsigned int spread, const NRMatrixF *gs2px,
								  float x0, float y0, float x1, float y1);

/* Radial */

enum {
	RGR_END,
	RGR_SYMMETRIC,
	RGR_OPTIMIZED
};

struct _NRRGradientRenderer {
	NRRenderer renderer;
	unsigned int type;
	const unsigned char *vector;
	unsigned int spread;
	NRMatrixF px2gs;
	float cx, cy;
	float fx, fy;
	float r;
	float C;
};

void nr_rgradient_renderer_setup (NRRGradientRenderer *renderer, NRPaintServer *server,
								  const unsigned char *vector, unsigned int spread, const NRMatrixF *gs2px,
								float cx, float cy, float fx, float fy, float r);

#endif
