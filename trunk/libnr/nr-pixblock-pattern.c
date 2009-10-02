#define __NR_PIXBLOCK_PATTERN_C__

/*
 * Pixel buffer rendering library
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#include <string.h>

#include "nr-rect.h"
#include "nr-pixops.h"
#include "nr-pixblock-pattern.h"

void
nr_pixblock_fill (NRPixBlock *pb, unsigned int rgba32)
{
	unsigned int r = NR_RGBA32_R (rgba32);
	unsigned int g = NR_RGBA32_R (rgba32);
	unsigned int b = NR_RGBA32_R (rgba32);
	unsigned int a = NR_RGBA32_R (rgba32);
	int x, y;

	switch (pb->mode) {
	case NR_PIXBLOCK_MODE_G8:
		for (y = pb->area.y0; y < pb->area.y1; y++) {
			unsigned char *d;
			d = NR_PIXBLOCK_PX (pb) + (y - pb->area.y0) * pb->rs;
			memset (d, a, pb->area.x1 - pb->area.x0);
		}
		break;
	case NR_PIXBLOCK_MODE_R8G8B8:
		for (y = pb->area.y0; y < pb->area.y1; y++) {
			unsigned char *d;
			d = NR_PIXBLOCK_PX (pb) + (y - pb->area.y0) * pb->rs;
			for (x = pb->area.x0; x < pb->area.x1; x++) {
				d[0] = r;
				d[1] = g;
				d[2] = b;
				d += 3;
			}
		}
		break;
	case NR_PIXBLOCK_MODE_R8G8B8A8N:
	case NR_PIXBLOCK_MODE_R8G8B8A8P:
		for (y = pb->area.y0; y < pb->area.y1; y++) {
			unsigned char *d;
			d = NR_PIXBLOCK_PX (pb) + (y - pb->area.y0) * pb->rs;
			for (x = pb->area.x0; x < pb->area.x1; x++) {
				d[0] = r;
				d[1] = g;
				d[2] = b;
				d[3] = a;
				d += 4;
			}
		}
		break;
	default:
		break;
	}
}

#define NR_NOISE_SIZE 1024

void
nr_pixblock_render_gray_noise (NRPixBlock *pb, NRPixBlock *mask)
{
	static unsigned char *noise = NULL;
	static unsigned int seed = 0;
	unsigned int v;
	NRRectS clip;
	int x, y, bpp;

	if (mask) {
		if (mask->empty) return;
		nr_rect_s_intersect (&clip, &pb->area, &mask->area);
		if (nr_rect_l_test_empty (&clip)) return;
	} else {
		clip = pb->area;
	}

	if (!noise) {
		int i;
		noise = nr_new (unsigned char, NR_NOISE_SIZE);
		for (i = 0; i < NR_NOISE_SIZE; i++) noise[i] = (rand () / (RAND_MAX >> 8)) & 0xff;
	}

	bpp = NR_PIXBLOCK_BPP (pb);

	v = (rand () / (RAND_MAX >> 8)) & 0xff;

	if (mask) {
		for (y = clip.y0; y < clip.y1; y++) {
			unsigned char *d, *m;
			d = NR_PIXBLOCK_PX (pb) + (y - pb->area.y0) * pb->rs + (clip.x0 - pb->area.x0) * bpp;
			m = NR_PIXBLOCK_PX (mask) + (y - mask->area.y0) * pb->rs + (clip.x0 - mask->area.x0);
			for (x = clip.x0; x < clip.x1; x++) {
				v = v ^ noise[seed];
				switch (pb->mode) {
				case NR_PIXBLOCK_MODE_G8:
					d[0] = (65025 - (255 - m[0]) * (255 - d[0]) + 127) / 255;
					break;
				case NR_PIXBLOCK_MODE_R8G8B8:
					d[0] = NR_COMPOSEN11 (v, m[0], d[0]);
					d[1] = NR_COMPOSEN11 (v, m[0], d[1]);
					d[2] = NR_COMPOSEN11 (v, m[0], d[2]);
					break;
				case NR_PIXBLOCK_MODE_R8G8B8A8N:
					if (m[0] != 0) {
						unsigned int ca;
						ca = NR_A7 (m[0], d[3]);
						d[0] = NR_COMPOSENNN_A7 (v, m[0], d[0], d[3], ca);
						d[1] = NR_COMPOSENNN_A7 (v, m[0], d[1], d[3], ca);
						d[2] = NR_COMPOSENNN_A7 (v, m[0], d[2], d[3], ca);
						d[3] = (ca + 127) / 255;
					}
					break;
				case NR_PIXBLOCK_MODE_R8G8B8A8P:
					d[0] = NR_COMPOSENPP (v, m[0], d[0], d[3]);
					d[1] = NR_COMPOSENPP (v, m[0], d[1], d[3]);
					d[2] = NR_COMPOSENPP (v, m[0], d[2], d[3]);
					d[3] = (NR_A7 (d[3], m[0]) + 127) / 255;
					break;
				default:
					break;
				}
				d += bpp;
				m += 1;
				if (++seed >= NR_NOISE_SIZE) {
					int i;
					i = (rand () / (RAND_MAX / NR_NOISE_SIZE)) % NR_NOISE_SIZE;
					noise[i] ^= v;
					seed = i % (NR_NOISE_SIZE >> 2);
				}
			}
		}
	} else {
		for (y = clip.y0; y < clip.y1; y++) {
			unsigned char *d;
			d = NR_PIXBLOCK_PX (pb) + (y - pb->area.y0) * pb->rs + (clip.x0 - pb->area.x0) * bpp;
			for (x = clip.x0; x < clip.x1; x++) {
				v = v ^ noise[seed];
				switch (pb->mode) {
				case NR_PIXBLOCK_MODE_G8:
					d[0] = 255;
					break;
				case NR_PIXBLOCK_MODE_R8G8B8:
					d[0] = v;
					d[1] = v;
					d[2] = v;
					break;
				case NR_PIXBLOCK_MODE_R8G8B8A8N:
				case NR_PIXBLOCK_MODE_R8G8B8A8P:
					d[0] = v;
					d[1] = v;
					d[2] = v;
					d[3] = 255;
				default:
					break;
				}
				d += bpp;
				if (++seed >= NR_NOISE_SIZE) {
					int i;
					i = (rand () / (RAND_MAX / NR_NOISE_SIZE)) % NR_NOISE_SIZE;
					noise[i] ^= v;
					seed = i % (NR_NOISE_SIZE >> 2);
				}
			}
		}
	}

	pb->empty = 0;
}

void
nr_pixblock_render_checker (NRPixBlock *pb, unsigned int size, unsigned int xoffset, unsigned int yoffset, unsigned int rgb0, unsigned int rgb1)
{
	unsigned char r[2], g[2], b[2];
	unsigned int size2;
	int x, y, bpp;
	int p;

	size2 = 2 * size;
	bpp = NR_PIXBLOCK_BPP (pb);
	if (bpp == 1) {
		g[0] = (NR_RGBA32_R (rgb0) + NR_RGBA32_G (rgb0) + NR_RGBA32_B (rgb0) + 1) / 3;
		g[1] = (NR_RGBA32_R (rgb1) + NR_RGBA32_G (rgb1) + NR_RGBA32_B (rgb1) + 1) / 3;
	} else {
		r[0] = NR_RGBA32_R (rgb0);
		g[0] = NR_RGBA32_G (rgb0);
		b[0] = NR_RGBA32_B (rgb0);
		r[1] = NR_RGBA32_R (rgb1);
		g[1] = NR_RGBA32_G (rgb1);
		b[1] = NR_RGBA32_B (rgb1);
	}

	for (y = pb->area.y0; y < pb->area.y1; y++) {
		unsigned char *d;
		d = NR_PIXBLOCK_PX (pb) + (y - pb->area.y0) * pb->rs;
		for (x = pb->area.x0; x < pb->area.x1; x++) {
			p = (((x + xoffset) ^ (y + yoffset)) % size2) / size;
			switch (pb->mode) {
			case NR_PIXBLOCK_MODE_G8:
				d[0] = g[p];
				break;
			case NR_PIXBLOCK_MODE_R8G8B8:
				d[0] = r[p];
				d[1] = g[p];
				d[2] = b[p];
				break;
			case NR_PIXBLOCK_MODE_R8G8B8A8N:
			case NR_PIXBLOCK_MODE_R8G8B8A8P:
				d[0] = r[p];
				d[1] = g[p];
				d[2] = b[p];
				d[3] = 255;
			default:
				break;
			}
			d += bpp;
		}
	}

	pb->empty = 0;
}

