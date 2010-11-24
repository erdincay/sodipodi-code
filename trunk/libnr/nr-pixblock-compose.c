#define __NR_PIXBLOCK_COMPOSE_C__

/*
 * Pixel buffer rendering library
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#include "nr-pixblock.h"
#include "nr-compose-transform.h"

#include "nr-pixblock-compose.h"

#define XSAMPLE 1
#define YSAMPLE 1

void
nr_pixblock_compose (NRPixBlock *d, const NRPixBlock *s, unsigned int alpha, const NRMatrixF *d2s)
{
	unsigned char *dpx;
	const unsigned char *spx;
	int dw, dh, drs, sw, sh, srs;
	dpx = NR_PIXBLOCK_PX (d);
	dw = d->area.x1 - d->area.x0;
	dh = d->area.y1 - d->area.y0;
	drs = d->rs;
	spx = NR_PIXBLOCK_PX (s);
	sw = s->area.x1 - s->area.x0;
	sh = s->area.y1 - s->area.y0;
	srs = s->rs;
	if (s->mode != NR_PIXBLOCK_MODE_R8G8B8A8N) {
		/* fixme: This is not implemented yet (Lauris) */
	} else if (d->mode == NR_PIXBLOCK_MODE_R8G8B8) {
		/* fixme: This is not implemented yet (Lauris) */
		/* nr_R8G8B8_R8G8B8_R8G8B8A8_N_TRANSFORM (dpx, dw, dh, drs, spx, sw, sh, srs, &d2s, Falpha, XSAMPLE, YSAMPLE); */
	} else if (d->mode == NR_PIXBLOCK_MODE_R8G8B8A8P) {
		nr_R8G8B8A8_P_R8G8B8A8_P_R8G8B8A8_N_TRANSFORM (dpx, dw, dh, drs, spx, sw, sh, srs, d2s, alpha, XSAMPLE, YSAMPLE);
	} else if (d->mode == NR_PIXBLOCK_MODE_R8G8B8A8N) {
		nr_R8G8B8A8_N_R8G8B8A8_N_R8G8B8A8_N_TRANSFORM (dpx, dw, dh, drs, spx, sw, sh, srs, d2s, alpha, XSAMPLE, YSAMPLE);
	}
	d->empty = 0;
}

