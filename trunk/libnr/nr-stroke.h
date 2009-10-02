#ifndef __NR_STROKE_H__
#define __NR_STROKE_H__

/*
 * Pixel buffer rendering library
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libnr/nr-path.h>
#include <libnr/nr-svp.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
	NR_STROKE_CAP_BUTT,
	NR_STROKE_CAP_ROUND,
	NR_STROKE_CAP_SQUARE
};

enum {
	NR_STROKE_JOIN_MITER,
	NR_STROKE_JOIN_ROUND,
	NR_STROKE_JOIN_BEVEL
};

typedef struct _NRDash NRDash;

NRSVL *nr_path_stroke (const NRPath *path, const NRMatrixF *transform,
		       float width,
		       unsigned int cap, unsigned int join, float miterlimit,
		       float flatness);

void nr_path_stroke_bbox_union (const NRPath *path, const NRMatrixF *transform, NRRectF *bbox, float width, unsigned int cap, unsigned int join, float miterlimit, float flatness);

struct _NRDash {
	unsigned int ndashes;
	float offset;
	float dashes[1];
};

#ifdef __cplusplus
};
#endif

#endif
