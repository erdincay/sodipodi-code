#define __NR_PIXBLOCK_TRANSFORM_C__

/*
 * Pixel buffer rendering library
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#include "nr-matrix.h"
#include "nr-rect.h"
#include "nr-pixblock.h"

#include "nr-pixblock-transform.h"

void
nr_pixblock_transform (NRPixBlock *dpx, const NRPixBlock *spx, const NRMatrixF *d2s, NRULong clearrgba)
{
	NRRectF darea, sarea;
	unsigned int bpp;
	int row, col;
	unsigned int c;
	unsigned char bg[4];
	const unsigned char *s;
	/* Can only transform between identical modes */
	if (dpx->mode != spx->mode) return;
	/* Test if source and target overlap */
	darea.x0 = dpx->area.x0 + 0.5f;
	darea.y0 = dpx->area.y0 + 0.5f;
	darea.x1 = dpx->area.x1 - 0.5f;
	darea.y1 = dpx->area.y1 - 0.5f;
	nr_rect_f_matrix_f_transform (&sarea, &darea, d2s);
	if (sarea.x1 <= spx->area.x0 - 0.5f) return;
	if (sarea.y1 <= spx->area.y0 - 0.5f) return;
	if (sarea.x0 >= spx->area.x1 - 0.5f) return;
	if (sarea.y0 >= spx->area.y1 - 0.5f) return;
	/* We can hope that areas overlap */
	bpp = NR_PIXBLOCK_BPP (dpx);
	/* Fill clear color */
	for (c = 0; c < bpp; c++) bg[c] = (clearrgba >> (8 * (3 - c))) & 255;
	s = NR_PIXBLOCK_PX (spx);
	for (row = dpx->area.y0; row < dpx->area.y1; row++) {
		unsigned char *d;
		float dx, dy, sx, sy, sdx, sdy;
		d = NR_PIXBLOCK_PX (dpx) + (row - dpx->area.y0) * dpx->rs;
		dx = dpx->area.x0 + 0.5f;
		dy = row + 0.5f;
		/* The proper source pixel centers are at 0.5 */
		/* We move position down to get easier rounding */
		sx = NR_MATRIX_DF_TRANSFORM_X (d2s, dx, dy) - 0.5f;
		sy = NR_MATRIX_DF_TRANSFORM_Y (d2s, dx, dy) - 0.5f;
		sdx = d2s->c[0];
		sdy = d2s->c[1];
		for (col = dpx->area.x0; col < dpx->area.x1; col++) {
			float wx0, wx1, wy0, wy1;
			float b[4];
			int x, y;
			/* Get weighting factors */
			wx0 = floorf (sx) + 1 - sx;
			wx1 = sx - floorf (sx);
			wy0 = floorf (sy) + 1 - sy;
			wy1 = sy - floorf (sy);
			/* Accumulation buffer */
			for (c = 0; c < bpp; c++) b[c] = 0;
			x = (int) floorf (sx);
			y = (int) floorf (sy);
			if ((y >= spx->area.y0) && (y < spx->area.y1)) {
				/* Upper row */
				if ((x >= spx->area.x0) && (x < spx->area.x1)) {
					/* Left pixel */
					for (c = 0; c < bpp; c++) b[c] += wy0 * wx0 * *(s + y * spx->rs + x * bpp + c);
				} else {
					for (c = 0; c < bpp; c++) b[c] += wy0 * wx0 * bg[c];
				}
				x += 1;
				if ((x >= spx->area.x0) && (x < spx->area.x1)) {
					/* Right pixel */
					for (c = 0; c < bpp; c++) b[c] += wy0 * wx1 * *(s + y * spx->rs + x * bpp + c);
				} else {
					for (c = 0; c < bpp; c++) b[c] += wy0 * wx1 * bg[c];
				}
				x -= 1;
			} else {
				for (c = 0; c < bpp; c++) b[c] += wy0 * bg[c];
			}
			y += 1;
			if ((y >= spx->area.y0) && (y < spx->area.y1)) {
				/* Upper row */
				if ((x >= spx->area.x0) && (x < spx->area.x1)) {
					/* Left pixel */
					for (c = 0; c < bpp; c++) b[c] += wy1 * wx0 * *(s + y * spx->rs + x * bpp + c);
				} else {
					for (c = 0; c < bpp; c++) b[c] += wy1 * wx0 * bg[c];
				}
				x += 1;
				if ((x >= spx->area.x0) && (x < spx->area.x1)) {
					/* Right pixel */
					for (c = 0; c < bpp; c++) b[c] += wy1 * wx1 * *(s + y * spx->rs + x * bpp + c);
				} else {
					for (c = 0; c < bpp; c++) b[c] += wy1 * wx1 * bg[c];
				}
				x -= 1;
			} else {
				for (c = 0; c < bpp; c++) b[c] += wy1 * bg[c];
			}
			for (c = 0; c < bpp; c++) d[c] = (unsigned char) b[c];
			sx += sdx;
			sy += sdy;
			d += bpp;
		}
	}
	dpx->empty = 0;
}

void
nr_pixblock_scale (NRPixBlock *dpx, const NRPixBlock *spx)
{
	NRMatrixF d2s;
	// d2s.c[0] = (float) (spx->area.x1 - spx->area.x0 - 1) / (dpx->area.x1 - dpx->area.x0 - 1);
	d2s.c[0] = (float) (spx->area.x1 - spx->area.x0) / (dpx->area.x1 - dpx->area.x0);
	d2s.c[1] = 0;
	d2s.c[2] = 0;
	// d2s.c[3] = (float) (spx->area.y1 - spx->area.y0 - 1) / (dpx->area.y1 - dpx->area.y0 - 1);
	d2s.c[3] = (float) (spx->area.y1 - spx->area.y0) / (dpx->area.y1 - dpx->area.y0);
	// d2s.c[4] = 0.5f - 0.5f * d2s.c[0];
	d2s.c[4] = 0;
	// d2s.c[5] = 0.5f - 0.5f * d2s.c[3];
	d2s.c[5] = 0;
	nr_pixblock_transform (dpx, spx, &d2s, 0);
}

