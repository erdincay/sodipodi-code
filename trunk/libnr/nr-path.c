#define __NR_PATH_C__

/*
 * Pixel buffer rendering library
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#include <string.h>
#include <stdio.h>

#include "nr-rect.h"
#include "nr-matrix.h"
#include "nr-path.h"

enum {
	MULTI_NONE,
	MULTI_LINE,
	MULTI_CURVE2,
	MULTI_CURVE3
};

static void nr_curve_bbox (double x000, double y000, double x001, double y001, double x011, double y011, double x111, double y111, NRRectF *bbox);

/* Helpers */

static NRPath *
nr_path_new (unsigned int nelements, unsigned int nreserved)
{
	NRPath *path;
	path = (NRPath *) malloc (sizeof (NRPath) + nelements * sizeof (NRPathElement) - sizeof (NRPathElement));
	path->nelements = nelements;
	path->offset = nreserved;
	return path;
}

static void
nr_path_copy_elements (NRPath *d, unsigned int dpos, const NRPath *s, unsigned int spos, unsigned int nelements)
{
	memcpy (&d->elements[dpos], &s->elements[spos], nelements * sizeof (NRPathElement));
}

static void
nr_path_copy_point (NRPathElement *d, const NRPathElement *s, const NRMatrixF *transform)
{
	if (transform) {
		d[0].fval = NR_MATRIX_DF_TRANSFORM_X (transform, s[0].fval, s[1].fval);
		d[1].fval = NR_MATRIX_DF_TRANSFORM_Y (transform, s[0].fval, s[1].fval);
	} else {
		d[0].fval = s[0].fval;
		d[1].fval = s[1].fval;
	}
}

NRPath *
nr_path_duplicate (const NRPath *path, unsigned int nreserved)
{
	NRPath *newpath;
	if (nreserved > 3) return NULL;
	newpath = nr_path_new (path->nelements - path->offset + nreserved, nreserved);
	memcpy (&newpath->elements[newpath->offset], &path->elements[path->offset], (path->nelements - path->offset) * sizeof (NRPathElement));
	return newpath;
}

NRPath *
nr_path_duplicate_transform (const NRPath *path, const NRMatrixF *transform)
{
	NRPath *newpath;
	newpath = nr_path_new (path->nelements, path->offset);
	if (transform) {
		unsigned int sstart;
		/* Copy structure */
		memcpy (newpath, path, sizeof (NRPath) - sizeof (NRPathElement));
		/* We do not copy reserved data */
		sstart = path->offset;
		while (sstart < path->nelements) {
			const NRPathElement *ss;
			NRPathElement *ds;
			unsigned int slen, idx;

			ss = &path->elements[sstart];
			ds = &newpath->elements[sstart];
			slen = NR_PATH_SEGMENT_LENGTH (ss);

			/* Start new subpath */
			ds[0] = ss[0];
			nr_path_copy_point (ds + 1, ss + 1, transform);
			idx = 3;
			while (idx < slen) {
				int nmulti, ncp, i, j;
				ds[idx] = ss[idx];
				nmulti = NR_PATH_ELEMENT_LENGTH (ss + idx);
				ncp = NR_PATH_CODE_NUM_POINTS (NR_PATH_ELEMENT_CODE (ss + idx));
				idx += 1;
				for (i = 0; i < nmulti; i++) {
					for (j = 0; j < ncp; j++) {
						nr_path_copy_point (ds + idx + 2 * j, ss + idx + 2 * j, transform);
					}
					idx += (2 * ncp);
				}
			}
			/* Finish path */
			sstart += slen;
		}
	} else {
		memcpy (newpath, path, sizeof (NRPath) + path-> nelements * sizeof (NRPathElement) - sizeof (NRPathElement));
	}
	return newpath;
}

unsigned int
nr_path_forall (const NRPath *path, const NRMatrixF *transform, const NRPathGVector *gv, void *data)
{
	float x0, y0, sx, sy;
	unsigned int sstart;

	sstart = path->offset;
	while (sstart < path->nelements) {
		const NRPathElement *seg;
		unsigned int slen, cflags, fflags, lflags, idx, flags;

		seg = path->elements + sstart;
		slen = NR_PATH_SEGMENT_LENGTH (seg);
		cflags = NR_PATH_SEGMENT_IS_CLOSED (seg) ? NR_PATH_CLOSED : 0; 
		fflags = NR_PATH_FIRST;
		lflags = 0;

		/* Start new subpath */
		if (transform) {
			x0 = sx = NR_MATRIX_DF_TRANSFORM_X (transform, seg[1].fval, seg[2].fval);
			y0 = sy = NR_MATRIX_DF_TRANSFORM_Y (transform, seg[1].fval, seg[2].fval);
		} else {
			x0 = sx = seg[1].fval;
			y0 = sy = seg[2].fval;
		}
		if (gv->moveto) {
			/* Moveto is always first */
			flags = cflags | fflags | lflags;
			if (!gv->moveto (x0, y0, flags, data)) return FALSE;
		}

		idx = 3;
		while (idx < slen) {
			int nmulti, i;
			nmulti = NR_PATH_ELEMENT_LENGTH (seg + idx);
			switch (NR_PATH_ELEMENT_CODE (seg + idx)) {
			case NR_PATH_LINETO:
				idx += 1;
				for (i = 0; i < nmulti; i++) {
					float x1, y1;
					if (transform) {
						x1 = NR_MATRIX_DF_TRANSFORM_X (transform, seg[idx].fval, seg[idx + 1].fval);
						y1 = NR_MATRIX_DF_TRANSFORM_Y (transform, seg[idx].fval, seg[idx + 1].fval);
					} else {
						x1 = seg[idx].fval;
						y1 = seg[idx + 1].fval;
					}
					idx += 2;
					if (idx >= slen) lflags = NR_PATH_LAST;
					flags = cflags | fflags | lflags;
					if (gv->lineto && !gv->lineto (x0, y0, x1, y1, flags, data)) {
						return FALSE;
					}
					fflags = 0;
					x0 = x1;
					y0 = y1;
				}
				break;
			case NR_PATH_CURVETO2:
				idx += 1;
				for (i = 0; i < nmulti; i++) {
					float x1, y1, x2, y2;
					if (transform) {
						x1 = NR_MATRIX_DF_TRANSFORM_X (transform, seg[idx].fval, seg[idx + 1].fval);
						y1 = NR_MATRIX_DF_TRANSFORM_Y (transform, seg[idx].fval, seg[idx + 1].fval);
						x2 = NR_MATRIX_DF_TRANSFORM_X (transform, seg[idx + 2].fval, seg[idx + 3].fval);
						y2 = NR_MATRIX_DF_TRANSFORM_Y (transform, seg[idx + 2].fval, seg[idx + 3].fval);
					} else {
						x1 = seg[idx].fval;
						y1 = seg[idx + 1].fval;
						x2 = seg[idx + 2].fval;
						y2 = seg[idx + 3].fval;
					}
					idx += 4;
					if (idx >= slen) lflags = NR_PATH_LAST;
					flags = cflags | fflags | lflags;
					if (gv->curveto2) {
						if (!gv->curveto2 (x0, y0, x1, y1, x2, y2, flags, data)) return FALSE;
					} else if (gv->curveto3) {
						if (!gv->curveto3 (x0, y0, (x0 + 2 * x1) / 3, (y0 + 2 * y1) / 3, (x2 + 2 * x1) / 3, (y2 + 2 * y1) / 3, x2, y2, flags, data)) return FALSE;
					} else {
						return FALSE;
					}
					fflags = 0;
					x0 = x2;
					y0 = y2;
				}
				break;
			case NR_PATH_CURVETO3:
				idx += 1;
				for (i = 0; i < nmulti; i++) {
					float x1, y1, x2, y2, x3, y3;
					if (transform) {
						x1 = NR_MATRIX_DF_TRANSFORM_X (transform, seg[idx].fval, seg[idx + 1].fval);
						y1 = NR_MATRIX_DF_TRANSFORM_Y (transform, seg[idx].fval, seg[idx + 1].fval);
						x2 = NR_MATRIX_DF_TRANSFORM_X (transform, seg[idx + 2].fval, seg[idx + 3].fval);
						y2 = NR_MATRIX_DF_TRANSFORM_Y (transform, seg[idx + 2].fval, seg[idx + 3].fval);
						x3 = NR_MATRIX_DF_TRANSFORM_X (transform, seg[idx + 4].fval, seg[idx + 5].fval);
						y3 = NR_MATRIX_DF_TRANSFORM_Y (transform, seg[idx + 4].fval, seg[idx + 5].fval);
					} else {
						x1 = seg[idx].fval;
						y1 = seg[idx + 1].fval;
						x2 = seg[idx + 2].fval;
						y2 = seg[idx + 3].fval;
						x3 = seg[idx + 4].fval;
						y3 = seg[idx + 5].fval;
					}
					idx += 6;
					/* fixme: LAST flag may be wrong if autoclose is on */
					if (idx >= slen) lflags = NR_PATH_LAST;
					flags = cflags | fflags | lflags;
					if (gv->curveto3 && !gv->curveto3 (x0, y0, x1, y1, x2, y2, x3, y3, flags, data)) {
						return FALSE;
					}
					fflags = 0;
					x0 = x3;
					y0 = y3;
				}
				break;
			default:
				fprintf (stderr, "Invalid path code '%d'\n", NR_PATH_ELEMENT_CODE (seg + idx));
				return FALSE;
				break;
			}
		}
		/* Finish path */
		if (gv->endpath) {
			fflags = 0;
			lflags = NR_PATH_LAST;
			flags = cflags | fflags | lflags;
			if (!gv->endpath (x0, y0, sx, sy, flags, data)) return FALSE;
		}
		sstart += slen;
	}
	return TRUE;
}

struct _NRPathFlattenData {
	const NRPathGVector *pgv;
	void *data;
	double tolerance2;
};

static unsigned int
nr_curve_flatten (double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3,
		  unsigned int flags, struct _NRPathFlattenData *fdata)
{
	double dx1_0, dy1_0, dx2_0, dy2_0, dx3_0, dy3_0, dx2_3, dy2_3, d3_0_2;
	double s1_q, t1_q, s2_q, t2_q, v2_q;
	double f2, f2_q;
	double x00t, y00t, x0tt, y0tt, xttt, yttt, x1tt, y1tt, x11t, y11t;

	dx1_0 = x1 - x0;
	dy1_0 = y1 - y0;
	dx2_0 = x2 - x0;
	dy2_0 = y2 - y0;
	dx3_0 = x3 - x0;
	dy3_0 = y3 - y0;
	dx2_3 = x3 - x2;
	dy2_3 = y3 - y2;
	f2 = fdata->tolerance2;
	d3_0_2 = dx3_0 * dx3_0 + dy3_0 * dy3_0;
	if (d3_0_2 < f2) {
		double d1_0_2, d2_0_2;
		d1_0_2 = dx1_0 * dx1_0 + dy1_0 * dy1_0;
		d2_0_2 = dx2_0 * dx2_0 + dy2_0 * dy2_0;
		if ((d1_0_2 < f2) && (d2_0_2 < f2)) {
			goto nosubdivide;
		} else {
			goto subdivide;
		}
	}
	f2_q = f2 * d3_0_2;
	s1_q = dx1_0 * dx3_0 + dy1_0 * dy3_0;
	t1_q = dy1_0 * dx3_0 - dx1_0 * dy3_0;
	s2_q = dx2_0 * dx3_0 + dy2_0 * dy3_0;
	t2_q = dy2_0 * dx3_0 - dx2_0 * dy3_0;
	v2_q = dx2_3 * dx3_0 + dy2_3 * dy3_0;
	if ((t1_q * t1_q) > f2_q) goto subdivide;
	if ((t2_q * t2_q) > f2_q) goto subdivide;
	if ((s1_q < 0.0) && ((s1_q * s1_q) > f2_q)) goto subdivide;
	if ((v2_q < 0.0) && ((v2_q * v2_q) > f2_q)) goto subdivide;
	if (s1_q >= s2_q) goto subdivide;

 nosubdivide:
	if (fdata->pgv->lineto) {
		return fdata->pgv->lineto ((float) x0, (float) y0, (float) x3, (float) y3, flags, fdata->data);
	}
	return TRUE;

 subdivide:
	x00t = (x0 + x1) * 0.5;
	y00t = (y0 + y1) * 0.5;
	x0tt = (x0 + 2 * x1 + x2) * 0.25;
	y0tt = (y0 + 2 * y1 + y2) * 0.25;
	x1tt = (x1 + 2 * x2 + x3) * 0.25;
	y1tt = (y1 + 2 * y2 + y3) * 0.25;
	x11t = (x2 + x3) * 0.5;
	y11t = (y2 + y3) * 0.5;
	xttt = (x0tt + x1tt) * 0.5;
	yttt = (y0tt + y1tt) * 0.5;

	if (!nr_curve_flatten (x0, y0, x00t, y00t, x0tt, y0tt, xttt, yttt, flags & (~NR_PATH_LAST), fdata)) return FALSE;
	if (!nr_curve_flatten (xttt, yttt, x1tt, y1tt, x11t, y11t, x3, y3, flags & (~NR_PATH_FIRST), fdata)) return FALSE;
	return TRUE;
}

static unsigned int
nr_path_flatten_moveto (float x0, float y0, unsigned int flags, void *data)
{
	struct _NRPathFlattenData *fdata;
	fdata = (struct _NRPathFlattenData *) data;
	if (fdata->pgv->moveto) {
		return fdata->pgv->moveto (x0, y0, flags, fdata->data);
	}
	return TRUE;
}

static unsigned int
nr_path_flatten_lineto (float x0, float y0, float x1, float y1, unsigned int flags, void *data)
{
	struct _NRPathFlattenData *fdata;
	fdata = (struct _NRPathFlattenData *) data;
	if (fdata->pgv->lineto) {
		return fdata->pgv->lineto (x0, y0, x1, y1, flags, fdata->data);
	}
	return TRUE;
}

static unsigned int
nr_path_flatten_curveto2 (float x0, float y0, float x1, float y1, float x2, float y2,
			  unsigned int flags, void *data)
{
	struct _NRPathFlattenData *fdata;
	fdata = (struct _NRPathFlattenData *) data;
	return nr_curve_flatten (x0, y0,
				 x1 + (x0 - x1) / 3.0, y1 + (y0 - y1) / 3.0,
				 x1 + (x2 - x1) / 3.0, y1 + (y2 - y1) / 3.0, x2, y2, flags, fdata);
}

static unsigned int
nr_path_flatten_curveto3 (float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3,
			  unsigned int flags, void *data)
{
	struct _NRPathFlattenData *fdata;
	fdata = (struct _NRPathFlattenData *) data;
	return nr_curve_flatten (x0, y0, x1, y1, x2, y2, x3, y3, flags, fdata);
}

static unsigned int
nr_path_flatten_endpath (float ex, float ey, float sx, float sy, unsigned int flags, void *data)
{
	struct _NRPathFlattenData *fdata;
	fdata = (struct _NRPathFlattenData *) data;
	if (fdata->pgv->endpath) {
		return fdata->pgv->endpath (ex, ey, sx, sy, flags, fdata->data);
	}
	return TRUE;
}

static NRPathGVector fpgv = {
	nr_path_flatten_moveto,
	nr_path_flatten_lineto,
	nr_path_flatten_curveto2,
	nr_path_flatten_curveto3,
	nr_path_flatten_endpath
};

unsigned int
nr_path_forall_flat (const NRPath *path, const NRMatrixF *transform, float tolerance, const NRPathGVector *gv, void *data)
{
	struct _NRPathFlattenData fdata;
	fdata.pgv = gv;
	fdata.data = data;
	fdata.tolerance2 = tolerance * tolerance;

	return nr_path_forall (path, transform, &fpgv, &fdata);
}

static unsigned int
nr_path_bbox_moveto (float x0, float y0, unsigned int flags, void *data)
{
	NRRectF *bbox;
	bbox = (NRRectF *) data;
	bbox->x0 = MIN (bbox->x0, x0);
	bbox->y0 = MIN (bbox->y0, y0);
	bbox->x1 = MAX (bbox->x1, x0);
	bbox->y1 = MAX (bbox->y1, y0);
	return TRUE;
}

static unsigned int
nr_path_bbox_lineto (float x0, float y0, float x1, float y1, unsigned int flags, void *data)
{
	NRRectF *bbox;
	bbox = (NRRectF *) data;
	bbox->x0 = MIN (bbox->x0, x1);
	bbox->y0 = MIN (bbox->y0, y1);
	bbox->x1 = MAX (bbox->x1, x1);
	bbox->y1 = MAX (bbox->y1, y1);
	return TRUE;
}

static unsigned int
nr_path_bbox_curveto2 (float x0, float y0, float x1, float y1, float x2, float y2,
		       unsigned int flags, void *data)
{
	NRRectF *bbox;
	bbox = (NRRectF *) data;
	nr_curve_bbox (x0, y0,
		       x1 + (x0 - x1) / 3, y1 + (y0 - y1) / 3,
		       x1 + (x2 - x1) / 3, y1 + (y2 - y1) / 3,
		       x2, y2,
		       bbox);
	return TRUE;
}

static unsigned int
nr_path_bbox_curveto3 (float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3,
			  unsigned int flags, void *data)
{
	NRRectF *bbox;
	bbox = (NRRectF *) data;
	nr_curve_bbox (x0, y0, x1, y1, x2, y2, x3, y3, bbox);
	return TRUE;
}

static NRPathGVector bboxpgv = {
	nr_path_bbox_moveto,
	nr_path_bbox_lineto,
	nr_path_bbox_curveto2,
	nr_path_bbox_curveto3,
	NULL
};

void
nr_path_matrix_f_bbox_f_union (const NRPath *path, const NRMatrixF *m, NRRectF *bbox, float tolerance)
{
	NRRectF lbbox;
	nr_rect_f_set_empty (&lbbox);
	nr_path_forall (path, m, &bboxpgv, &lbbox);
	nr_rect_f_union (bbox, bbox, &lbbox);
}

unsigned int
nr_path_get_num_segments (const NRPath *path)
{
	unsigned int numsegs;
	unsigned int sstart;
	numsegs = 0;
	sstart = path->offset;
	while (sstart < path->nelements) {
		const NRPathElement *seg;
		unsigned int slen;
		seg = &path->elements[sstart];
		slen = NR_PATH_SEGMENT_LENGTH (seg);
		numsegs += 1;
		sstart += slen;
	}
	return numsegs;
}

unsigned int
nr_path_get_num_strokes (const NRPath *path)
{
	unsigned int numstrokes;
	unsigned int sstart;
	numstrokes = 0;
	sstart = path->offset;
	while (sstart < path->nelements) {
		const NRPathElement *sseg;
		unsigned int slen, idx;
		sseg = &path->elements[sstart];
		slen = NR_PATH_SEGMENT_LENGTH (sseg);
		idx = 3;
		while (idx < slen) {
			unsigned int code, nmulti, npoints;
			code = NR_PATH_ELEMENT_CODE (&sseg[idx]);
			nmulti = NR_PATH_ELEMENT_LENGTH (&sseg[idx]);
			numstrokes += nmulti;
			npoints = NR_PATH_CODE_NUM_POINTS (code);
			idx += (2 * npoints + 1);
		}
		sstart += slen;
	}
	return numstrokes;
}

unsigned int
nr_path_is_empty (const NRPath *path)
{
	unsigned int sstart;
	sstart = path->offset;
	while (sstart < path->nelements) {
		const NRPathElement *seg;
		unsigned int slen;
		seg = path->elements + sstart;
		slen = NR_PATH_SEGMENT_LENGTH (seg);
		/* Empty moveto is self + x + y */
		if (slen > 3) return TRUE;
		sstart += slen;
	}
	return FALSE;
}

unsigned int
nr_path_is_shape (const NRPath *path)
{
	unsigned int sstart;
	sstart = path->offset;
	while (sstart < path->nelements) {
		const NRPathElement *seg;
		unsigned int slen;
		seg = path->elements + sstart;
		slen = NR_PATH_SEGMENT_LENGTH (seg);
		/* Single lineto is self + x + y + code + x + y */
		if (slen > 6) return TRUE;
		sstart += slen;
	}
	return FALSE;
}

NRPathElement *
nr_path_get_last_segment (NRPath *path)
{
	NRPathElement *seg;
	unsigned int sstart;
	seg = NULL;
	sstart = path->offset;
	while (sstart < path->nelements) {
		unsigned int slen;
		seg = path->elements + sstart;
		slen = NR_PATH_SEGMENT_LENGTH (seg);
		sstart += slen;
	}
	return seg;
}

static unsigned int
nr_path_get_segment_list (unsigned int segs[], unsigned int maxsegs, NRPath *path)
{
	NRPathElement *seg;
	unsigned int pos, sstart;
	pos = 0;
	sstart = path->offset;
	while ((pos < maxsegs) && (sstart < path->nelements)) {
		unsigned int slen;
		segs[pos++] = sstart;
		seg = path->elements + sstart;
		slen = NR_PATH_SEGMENT_LENGTH (seg);
		sstart += slen;
	}
	return pos;
}

unsigned int
nr_path_is_any_closed (const NRPath *path)
{
	unsigned int sstart;
	sstart = path->offset;
	while (sstart < path->nelements) {
		const NRPathElement *seg;
		unsigned int slen;
		seg = path->elements + sstart;
		slen = NR_PATH_SEGMENT_LENGTH (seg);
		if (NR_PATH_SEGMENT_IS_CLOSED (seg)) return TRUE;
		sstart += slen;
	}
	return FALSE;
}

unsigned int
nr_path_are_all_closed (const NRPath *path)
{
	unsigned int sstart;
	sstart = path->offset;
	while (sstart < path->nelements) {
		const NRPathElement *seg;
		unsigned int slen;
		seg = path->elements + sstart;
		slen = NR_PATH_SEGMENT_LENGTH (seg);
		if (!NR_PATH_SEGMENT_IS_CLOSED (seg)) return FALSE;
		sstart += slen;
	}
	return TRUE;
}

unsigned int
nr_path_get_starting_point (const NRPath *path, float *x, float *y)
{
	if (path->nelements == path->offset) return 0;
	if (x) *x = NR_PATH_ELEMENT_VALUE (&path->elements[path->offset + 1]);
	if (y) *y = NR_PATH_ELEMENT_VALUE (&path->elements[path->offset + 2]);
	return 1;
}

static NRPath *
nr_path_combine_join (const NRPath *paths[], unsigned int numpaths, float snaple)
{
#if 0
	unsigned int nelements, i;
	unsigned int dlseg, dlel, dend;
	unsigned int lastclosed;
	NRPath *path;
	/* In worst case we have to add LINETo X Y, but we get rid of segment start */
	nelements = 0;
	for (i = 0; i < numpaths; i++) nelements += (paths[i]->nelements - paths[i]->offset);

	path = nr_path_new (nelements, 0);
	dlseg = dlel = dend = path->offset;
	/* Set closed flag so the next segment will be added intact */
	for (i = 0; i < numpaths; i++) {
		unsigned int sfseg;
		sfseg = paths[i]->offset;
		if (lastclosed || NR_PATH_SEGMENT_IS_CLOSED (&paths[i]->elements[sfseg])) {
			unsigned int len;
			/* Just add all path elements */
			len = paths[i]->nelements - paths[i]->offset;
			nr_path_copy_elements (path, dpos, paths[i], paths[i]->offset, len);
			dpos += len;
		} else {
	}

			float x0, y0, x1, y1;
			unsigned int spos, llen, flen, len;
			NRPathElement *lseg;
####lseg = nr_path_get_last_segment (paths[i]);
			llen = NR_PATH_SEGMENT_LENGTH (lseg);
			flen = NR_PATH_SEGMENT_LENGTH (fseg);
			x0 = path->elements[dpos - 2].fval;
			y0 = path->elements[dpos - 1].fval;
			spos = paths[i]->offset;
			x1 = paths[i]->elements[spos + 1].fval;
			y1 = paths[i]->elements[spos + 2].fval;
			if ((fabs (x1 - x0) <= snaple) && (fabs (y1 - y0) <= snaple)) {
				/* Snap - i.e. just ignore segment start */
				len = llen + flen - 3;
			} else {
				/* Add lineto - replace segment start */
				NR_PATH_ELEMENT_SET_CODE (&path->elements[dpos], NR_PATH_LINETO);
				NR_PATH_ELEMENT_SET_LENGTH (&path->elements[dpos], 1);
				path->elements[dpos + 1].fval = x1;
				path->elements[dpos + 2].fval = y1;
				len = llen + flen;
				dpos += 3;
			}
			NR_PATH_ELEMENT_SET_LENGTH (lseg, len);
			len = paths[i]->nelements - paths[i]->offset - 3;
			nr_path_copy_elements (path, dpos, paths[i], paths[i]->offset + 3, len);
			dpos += len;
		}
		if (join) {
			const NRPathElement *lseg;
			lseg = nr_path_get_last_segment (paths[i]);
			lastclosed = NR_PATH_SEGMENT_IS_CLOSED (lseg);
		}
	}
	return path;
#endif
return NULL;
}

NRPath *
nr_path_combine (const NRPath *paths[], unsigned int numpaths, unsigned int join, float snaple)
{
	unsigned int nelements, dpos, i;
	NRPath *path;
	if (join) return nr_path_combine_join (paths, numpaths, snaple);
	nelements = 0;
	for (i = 0; i < numpaths; i++) nelements += (paths[i]->nelements - paths[i]->offset);
	path = nr_path_new (nelements, 0);
	dpos = path->offset;
	for (i = 0; i < numpaths; i++) {
		unsigned int len;
		len = paths[i]->nelements - paths[i]->offset;
		nr_path_copy_elements (path, dpos, paths[i], paths[i]->offset, len);
		dpos += len;
	}
	return path;
}

unsigned int
nr_path_break (NRPath *paths[], unsigned int maxpaths, const NRPath *path)
{
	unsigned int numpaths, sstart;
	if (maxpaths < 1) return 0;
	numpaths = 0;
	sstart = path->offset;
	while (sstart < path->nelements) {
		const NRPathElement *seg;
		unsigned int slen;
		seg = &path->elements[sstart];
		slen = NR_PATH_SEGMENT_LENGTH (seg);
		paths[numpaths] = nr_path_new (slen, 0);
		nr_path_copy_elements (paths[numpaths], 0, path, sstart, slen);
		numpaths += 1;
		if (numpaths >= maxpaths) break;
		sstart += slen;
	}
	return numpaths;
}

NRPath *
nr_path_reverse (const NRPath *path)
{
	NRPath *newpath;
	NRPathElement *dseg;
	unsigned int sstart;

	newpath = nr_path_new (path->nelements, 0);
	sstart = path->offset;
	while (sstart < path->nelements) {
		const NRPathElement *sseg;
		unsigned int slen, dstart, idx;

		sseg = &path->elements[sstart];
		slen = NR_PATH_SEGMENT_LENGTH (sseg);
		dstart = newpath->nelements - sstart - slen;
		dseg = &newpath->elements[dstart];

		/* Segment descriptor is identical */
		dseg[0] = sseg[0];
		/* Moveto becomes last point */
		dseg[slen - 2] = sseg[1];
		dseg[slen - 1] = sseg[2];
		/* Start from the first element */
		idx = 3;
		while (idx < slen) {
			unsigned int nmulti, nsingle, ppoints, i;
			nmulti = NR_PATH_ELEMENT_LENGTH (&sseg[idx]);
			nsingle = NR_PATH_CODE_NUM_POINTS (NR_PATH_ELEMENT_CODE (&sseg[idx]));
			/* Points up to last will be prepended to the end */
			ppoints = nmulti * nsingle - 1;
			for (i = 0; i < ppoints; i++) {
				dseg[slen - idx - 2 * i - 1] = sseg[idx + 1 + 2 * i];
				dseg[slen - idx - 2 * i] = sseg[idx + 1 + 2 * i + 1];
			}
			/* Segment code is prepended to the end */
			dseg[slen - idx - 2 * ppoints] = sseg[idx];
			/* Last segment is prepended to the end */
			dseg[slen - idx - 2 * ppoints - 1 - 1] = sseg[idx + 1 + 2 * ppoints];
			dseg[slen - idx - 2 * ppoints - 1] = sseg[idx + 1 + 2 * ppoints + 1];
			idx += (2 * ppoints + 2 + 1);
		}
		sstart += slen;
	}
	return newpath;
}

/* fixme: Collate linetos (Lauris) */

NRPath *
nr_path_append_continuous (const NRPath *lhs, const NRPath *rhs)
{
	NRPath *path;
	const NRPathElement *lhsfseg, *lhslseg;
	NRPathElement *lseg, *seg;
	lhsfseg = &lhs->elements[lhs->offset];
	lhslseg = nr_path_get_last_segment ((NRPath *) lhs);
	if (NR_PATH_SEGMENT_IS_CLOSED (lhslseg)) return NULL;
	/* The start of the second path will be replaced with lineto + NUM + NUM */
	path = nr_path_new (lhs->nelements - lhs->offset + rhs->nelements - rhs->offset, 0);
	/* Copy lhs into new path */
	memcpy (&path->elements[0], &lhs->elements[lhs->offset], (lhs->nelements - lhs->offset) * sizeof (NRPathElement));
	lseg = &path->elements[(lhslseg - lhsfseg)];
	/* fixme: Here is potential oveflow (Lauris) */
	NR_PATH_SEGMENT_SET_LENGTH (lseg, NR_PATH_SEGMENT_LENGTH (lseg) + rhs->nelements - rhs->offset);
	seg = &path->elements[lhs->nelements - lhs->offset];
	NR_PATH_ELEMENT_SET_CODE (seg, NR_PATH_LINETO);
	NR_PATH_ELEMENT_SET_LENGTH (seg, 1);
	memcpy (seg + 1, &rhs->elements[rhs->offset + 1], (rhs->nelements - rhs->offset - 1) * sizeof (NRPathElement));
	return path;
}

/* NRDynamicPath */

void
nr_dynamic_path_setup (NRDynamicPath *dpath, unsigned int nelements)
{
	nelements = MIN (nelements, 4);
	memset (dpath, 0x0, sizeof (NRDynamicPath));
	dpath->path = (NRPath *) malloc (sizeof (NRPath) + nelements * sizeof (NRPathElement) - sizeof (NRPathElement));
	dpath->path->nelements = 0;
	dpath->path->offset = 0;
	dpath->refcount = 1;
	dpath->size = nelements;
	dpath->hascpt = 0;
	dpath->isstatic = 0;
}

void
nr_dynamic_path_setup_from_path (NRDynamicPath *dpath, NRPath *path, unsigned int isstatic, unsigned int makecopy)
{
	NRPathElement *lastseg;
	memset (dpath, 0x0, sizeof (NRDynamicPath));
	if (isstatic) {
		dpath->isstatic = 1;
		dpath->path = path;
	} else if (!makecopy) {
		dpath->isstatic = 0;
		dpath->path = path;
	} else {
		dpath->isstatic = 0;
		dpath->path = nr_path_duplicate (path, 0);
	}
	dpath->refcount = 1;
	dpath->size = dpath->path->nelements;
	lastseg = nr_path_get_last_segment (dpath->path);
	dpath->hascpt = (lastseg != NULL) && !(NR_PATH_SEGMENT_IS_CLOSED (lastseg));
}

// void
// nr_dynamic_path_release (NRDynamicPath *dpath)
// {
//	if (!dpath->isstatic) free (dpath->path);
// }

NRDynamicPath *
nr_dynamic_path_new (unsigned int nelements)
{
	NRDynamicPath *dpath;
	dpath = (NRDynamicPath *) malloc (sizeof (NRDynamicPath));
	nr_dynamic_path_setup (dpath, nelements);
	return dpath;
}

NRDynamicPath *
nr_dynamic_path_new_from_path (NRPath *path, unsigned int isstatic, unsigned int makecopy)
{
	NRDynamicPath *dpath;
	dpath = (NRDynamicPath *) malloc (sizeof (NRDynamicPath));
	nr_dynamic_path_setup_from_path (dpath, path, isstatic, makecopy);
	return dpath;
}

NRDynamicPath *
nr_dynamic_path_ref (NRDynamicPath *dpath)
{
	if (!dpath || (dpath->refcount < 1)) return NULL;
	dpath->refcount += 1;
	return dpath;
}

NRDynamicPath *
nr_dynamic_path_unref (NRDynamicPath *dpath)
{
	if (!dpath || (dpath->refcount < 1)) return NULL;
	dpath->refcount -= 1;
	if (dpath->refcount < 1) {
		if (!dpath->isstatic) free (dpath->path);
		free (dpath);
	}
	return NULL;
}

static void
nr_dynamic_path_ensure_space (NRDynamicPath *dpath, unsigned int req)
{
	if ((dpath->size - dpath->path->nelements) < req) {
		req = dpath->path->nelements + req - dpath->size;
		req = MIN (req, 32);
		dpath->size += req;
		dpath->path = realloc (dpath->path, sizeof (NRPath) + (dpath->size - 1) * sizeof (NRPathElement));
	}
}

void
nr_dynamic_path_reset (NRDynamicPath *dpath)
{
	if (dpath->isstatic) return;
	dpath->path->nelements = dpath->path->offset;
	dpath->hascpt = 0;
	dpath->segstart = dpath->path->offset;
	dpath->codepos = dpath->segstart;
}

void
nr_dynamic_path_set_path (NRDynamicPath *dpath, NRPath *path, unsigned int isstatic, unsigned int makecopy)
{
	NRPathElement *lastseg;
	if (!dpath->isstatic) free (dpath->path);
	if (isstatic) {
		dpath->isstatic = 1;
		dpath->path = path;
	} else if (!makecopy) {
		dpath->isstatic = 0;
		dpath->path = path;
	} else {
		dpath->isstatic = 0;
		dpath->path = nr_path_duplicate (path, 0);
	}
	dpath->size = dpath->path->nelements;
	lastseg = nr_path_get_last_segment (dpath->path);
	dpath->hascpt = (lastseg != NULL) && !(NR_PATH_SEGMENT_IS_CLOSED (lastseg));
}

/* Return TRUE on success */

unsigned int
nr_dynamic_path_moveto (NRDynamicPath *dpath, float x0, float y0)
{
	if (dpath->isstatic) return FALSE;
	/* Finish segment and code */
	dpath->segstart = dpath->path->nelements;
	dpath->codepos = dpath->segstart;
	/* Set currentpoint */
	dpath->hascpt = 1;
	dpath->cpx = x0;
	dpath->cpy = y0;
	return TRUE;
}

unsigned int
nr_dynamic_path_lineto (NRDynamicPath *dpath, float x1, float y1)
{
	NRPathElement *el, *sel;
	if (dpath->isstatic) return FALSE;
	if (!dpath->hascpt) return FALSE;
	if (dpath->segstart == dpath->path->nelements) {
		/* Segment is not started yet */
		/* Need len, x0, y0, LINETO, x1, y1 */
		nr_dynamic_path_ensure_space (dpath, 6);
		el = &dpath->path->elements[dpath->path->nelements];
		NR_PATH_SEGMENT_SET_LENGTH (el, 6);
		NR_PATH_SEGMENT_SET_CLOSED (el, 0);
		NR_PATH_ELEMENT_SET_VALUE (el + 1, dpath->cpx);
		NR_PATH_ELEMENT_SET_VALUE (el + 2, dpath->cpy);
		NR_PATH_ELEMENT_SET_LENGTH (el + 3, 1);
		NR_PATH_ELEMENT_SET_CODE (el + 3, NR_PATH_LINETO);
		NR_PATH_ELEMENT_SET_VALUE (el + 4, x1);
		NR_PATH_ELEMENT_SET_VALUE (el + 5, y1);
		dpath->codepos = dpath->path->nelements + 3;
		dpath->path->nelements += 6;
	} else if (NR_PATH_ELEMENT_CODE (&dpath->path->elements[dpath->codepos]) != NR_PATH_LINETO) {
		/* Have to start new lineto */
		/* Need LINETO, x1, y1 */
		nr_dynamic_path_ensure_space (dpath, 3);
		el = &dpath->path->elements[dpath->path->nelements];
		NR_PATH_ELEMENT_SET_LENGTH (el, 1);
		NR_PATH_ELEMENT_SET_CODE (el, NR_PATH_LINETO);
		NR_PATH_ELEMENT_SET_VALUE (el + 1, x1);
		NR_PATH_ELEMENT_SET_VALUE (el + 2, y1);
		dpath->codepos = dpath->path->nelements;
		dpath->path->nelements += 3;
	} else {
		/* Continue existing lineto */
		/* Need x1, y1 */
		nr_dynamic_path_ensure_space (dpath, 2);
		el = &dpath->path->elements[dpath->path->nelements];
		NR_PATH_ELEMENT_SET_VALUE (el, x1);
		NR_PATH_ELEMENT_SET_VALUE (el + 1, y1);
		el = &dpath->path->elements[dpath->codepos];
		NR_PATH_ELEMENT_SET_LENGTH (el, NR_PATH_ELEMENT_LENGTH (el) + 1);
		dpath->path->nelements += 2;
	}
	sel = &dpath->path->elements[dpath->segstart];
	NR_PATH_SEGMENT_SET_LENGTH (sel, dpath->path->nelements - dpath->segstart);
	return TRUE;
}

unsigned int
nr_dynamic_path_curveto2 (NRDynamicPath *dpath, float x1, float y1, float x2, float y2)
{
	NRPathElement *el, *sel;
	if (dpath->isstatic) return FALSE;
	if (!dpath->hascpt) return FALSE;
	if (dpath->segstart == dpath->path->nelements) {
		/* Segment is not started yet */
		/* Need len, x0, y0, CURVETO2, x1, y1 x2 y2 */
		nr_dynamic_path_ensure_space (dpath, 8);
		el = &dpath->path->elements[dpath->path->nelements];
		NR_PATH_SEGMENT_SET_LENGTH (el, 8);
		NR_PATH_SEGMENT_SET_CLOSED (el, 0);
		NR_PATH_ELEMENT_SET_VALUE (el + 1, dpath->cpx);
		NR_PATH_ELEMENT_SET_VALUE (el + 2, dpath->cpy);
		NR_PATH_ELEMENT_SET_LENGTH (el + 3, 1);
		NR_PATH_ELEMENT_SET_CODE (el + 3, NR_PATH_CURVETO2);
		NR_PATH_ELEMENT_SET_VALUE (el + 4, x1);
		NR_PATH_ELEMENT_SET_VALUE (el + 5, y1);
		NR_PATH_ELEMENT_SET_VALUE (el + 6, x2);
		NR_PATH_ELEMENT_SET_VALUE (el + 7, y2);
		dpath->codepos = dpath->path->nelements + 3;
		dpath->path->nelements += 8;
	} else if (NR_PATH_ELEMENT_CODE (&dpath->path->elements[dpath->codepos]) != NR_PATH_CURVETO2) {
		/* Have to start new lineto */
		/* Need CURVETO2, x1, y1 x2 y2 */
		nr_dynamic_path_ensure_space (dpath, 5);
		el = &dpath->path->elements[dpath->path->nelements];
		NR_PATH_ELEMENT_SET_LENGTH (el, 1);
		NR_PATH_ELEMENT_SET_CODE (el, NR_PATH_CURVETO2);
		NR_PATH_ELEMENT_SET_VALUE (el + 1, x1);
		NR_PATH_ELEMENT_SET_VALUE (el + 2, y1);
		NR_PATH_ELEMENT_SET_VALUE (el + 3, x2);
		NR_PATH_ELEMENT_SET_VALUE (el + 4, y2);
		dpath->codepos = dpath->path->nelements;
		dpath->path->nelements += 5;
	} else {
		/* Continue existing lineto */
		/* Need x1, y1 x2 y2 */
		nr_dynamic_path_ensure_space (dpath, 4);
		el = &dpath->path->elements[dpath->path->nelements];
		NR_PATH_ELEMENT_SET_VALUE (el, x1);
		NR_PATH_ELEMENT_SET_VALUE (el + 1, y1);
		NR_PATH_ELEMENT_SET_VALUE (el + 2, x2);
		NR_PATH_ELEMENT_SET_VALUE (el + 3, y2);
		el = &dpath->path->elements[dpath->codepos];
		NR_PATH_ELEMENT_SET_LENGTH (el, NR_PATH_ELEMENT_LENGTH (el) + 1);
		dpath->path->nelements += 4;
	}
	sel = &dpath->path->elements[dpath->segstart];
	NR_PATH_SEGMENT_SET_LENGTH (sel, dpath->path->nelements - dpath->segstart);
	return TRUE;
}

unsigned int
nr_dynamic_path_curveto3 (NRDynamicPath *dpath, float x1, float y1, float x2, float y2, float x3, float y3)
{
	NRPathElement *el, *sel;
	if (dpath->isstatic) return FALSE;
	if (!dpath->hascpt) return FALSE;
	if (dpath->segstart == dpath->path->nelements) {
		/* Segment is not started yet */
		/* Need len, x0, y0, CURVETO3, x1, y1 x2 y2 x3 y3 */
		nr_dynamic_path_ensure_space (dpath, 10);
		el = &dpath->path->elements[dpath->path->nelements];
		NR_PATH_SEGMENT_SET_LENGTH (el, 10);
		NR_PATH_SEGMENT_SET_CLOSED (el, 0);
		NR_PATH_ELEMENT_SET_VALUE (el + 1, dpath->cpx);
		NR_PATH_ELEMENT_SET_VALUE (el + 2, dpath->cpy);
		NR_PATH_ELEMENT_SET_LENGTH (el + 3, 1);
		NR_PATH_ELEMENT_SET_CODE (el + 3, NR_PATH_CURVETO3);
		NR_PATH_ELEMENT_SET_VALUE (el + 4, x1);
		NR_PATH_ELEMENT_SET_VALUE (el + 5, y1);
		NR_PATH_ELEMENT_SET_VALUE (el + 6, x2);
		NR_PATH_ELEMENT_SET_VALUE (el + 7, y2);
		NR_PATH_ELEMENT_SET_VALUE (el + 8, x3);
		NR_PATH_ELEMENT_SET_VALUE (el + 9, y3);
		dpath->codepos = dpath->path->nelements + 3;
		dpath->path->nelements += 10;
	} else if (NR_PATH_ELEMENT_CODE (&dpath->path->elements[dpath->codepos]) != NR_PATH_CURVETO3) {
		/* Have to start new lineto */
		/* Need LINETO, x1, y1 x2 y2 x3 y3 */
		nr_dynamic_path_ensure_space (dpath, 7);
		el = &dpath->path->elements[dpath->path->nelements];
		NR_PATH_ELEMENT_SET_LENGTH (el, 1);
		NR_PATH_ELEMENT_SET_CODE (el, NR_PATH_CURVETO3);
		NR_PATH_ELEMENT_SET_VALUE (el + 1, x1);
		NR_PATH_ELEMENT_SET_VALUE (el + 2, y1);
		NR_PATH_ELEMENT_SET_VALUE (el + 3, x2);
		NR_PATH_ELEMENT_SET_VALUE (el + 4, y2);
		NR_PATH_ELEMENT_SET_VALUE (el + 5, x3);
		NR_PATH_ELEMENT_SET_VALUE (el + 6, y3);
		dpath->codepos = dpath->path->nelements;
		dpath->path->nelements += 7;
	} else {
		/* Continue existing lineto */
		/* Need x1, y1 x2 y2 x3 y3 */
		nr_dynamic_path_ensure_space (dpath, 6);
		el = &dpath->path->elements[dpath->path->nelements];
		NR_PATH_ELEMENT_SET_VALUE (el, x1);
		NR_PATH_ELEMENT_SET_VALUE (el + 1, y1);
		NR_PATH_ELEMENT_SET_VALUE (el + 2, x2);
		NR_PATH_ELEMENT_SET_VALUE (el + 3, y2);
		NR_PATH_ELEMENT_SET_VALUE (el + 4, x3);
		NR_PATH_ELEMENT_SET_VALUE (el + 5, y3);
		el = &dpath->path->elements[dpath->codepos];
		NR_PATH_ELEMENT_SET_LENGTH (el, NR_PATH_ELEMENT_LENGTH (el) + 1);
		dpath->path->nelements += 6;
	}
	sel = &dpath->path->elements[dpath->segstart];
	NR_PATH_SEGMENT_SET_LENGTH (sel, dpath->path->nelements - dpath->segstart);
	return TRUE;
}

unsigned int
nr_dynamic_path_closepath (NRDynamicPath *dpath)
{
	NRPathElement *sel, *el;
	if (dpath->isstatic) return FALSE;
	if (!dpath->hascpt) return FALSE;
	if (dpath->segstart == dpath->path->nelements) return FALSE;
	sel = &dpath->path->elements[dpath->segstart];
	el = &dpath->path->elements[dpath->path->nelements - 2];
#if 0
	if ((NR_PATH_ELEMENT_VALUE (sel) != NR_PATH_ELEMENT_VALUE (el)) ||
	    (NR_PATH_ELEMENT_VALUE (sel + 1) != NR_PATH_ELEMENT_VALUE (el + 1))) {
		nr_dynamic_path_lineto (dpath, NR_PATH_ELEMENT_VALUE (sel), NR_PATH_ELEMENT_VALUE (sel + 1));
	}
#endif
	NR_PATH_SEGMENT_SET_CLOSED (sel, 1);
	dpath->hascpt = FALSE;
	return TRUE;
}

/* ---------- older stuff ---------- */

static void nr_curve_bbox (double x000, double y000, double x001, double y001, double x011, double y011, double x111, double y111, NRRectF *bbox);

void
nr_line_wind_distance (double Ax, double Ay, double Bx, double By, double Px, double Py, int *wind, float *best)
{
	double Dx, Dy, s;
	double dist2;

	/* Find distance */
	Dx = Bx - Ax;
	Dy = By - Ay;

	if (best) {
		s = ((Px - Ax) * Dx + (Py - Ay) * Dy) / (Dx * Dx + Dy * Dy);
		if (s <= 0.0) {
			dist2 = (Px - Ax) * (Px - Ax) + (Py - Ay) * (Py - Ay);
		} else if (s >= 1.0) {
			dist2 = (Px - Bx) * (Px - Bx) + (Py - By) * (Py - By);
		} else {
			double Qx, Qy;
			Qx = Ax + s * Dx;
			Qy = Ay + s * Dy;
			dist2 = (Px - Qx) * (Px - Qx) + (Py - Qy) * (Py - Qy);
		}
		if (dist2 < (*best * *best)) *best = (float) sqrt (dist2);
	}

	if (wind) {
		/* Find wind */
		if ((Ax >= Px) && (Bx >= Px)) return;
		if ((Ay >= Py) && (By >= Py)) return;
		if ((Ay < Py) && (By < Py)) return;
		if (Ay == By) return;
		/* Ctach upper y bound */
		if (Ay == Py) {
			if (Ax < Px) *wind -= 1;
			return;
		} else if (By == Py) {
			if (Bx < Px) *wind += 1;
			return;
		} else {
			double Qx;
			/* Have to calculate intersection */
			Qx = Ax + Dx * (Py - Ay) / Dy;
			if (Qx < Px) {
				*wind += (Dy > 0.0) ? 1 : -1;
			}
		}
	}
}

static void
nr_curve_bbox_wind_distance (double x000, double y000,
			     double x001, double y001,
			     double x011, double y011,
			     double x111, double y111,
			     NRPointF *pt,
			     NRRectF *bbox, int *wind, float *best,
			     float tolerance)
{
	double x0, y0, x1, y1, len2;
	double Px, Py;
	int needdist, needwind, needline;

	Px = pt->x;
	Py = pt->y;

	needdist = 0;
	needwind = 0;
	needline = 0;

	if (bbox) nr_curve_bbox (x000, y000, x001, y001, x011, y011, x111, y111, bbox);

	x0 = MIN (x000, x001);
	x0 = MIN (x0, x011);
	x0 = MIN (x0, x111);
	y0 = MIN (y000, y001);
	y0 = MIN (y0, y011);
	y0 = MIN (y0, y111);
	x1 = MAX (x000, x001);
	x1 = MAX (x1, x011);
	x1 = MAX (x1, x111);
	y1 = MAX (y000, y001);
	y1 = MAX (y1, y011);
	y1 = MAX (y1, y111);

	if (best) {
		/* Quicly adjust to endpoints */
		len2 = (x000 - Px) * (x000 - Px) + (y000 - Py) * (y000 - Py);
		if (len2 < (*best * *best)) *best = (float) sqrt (len2);
		len2 = (x111 - Px) * (x111 - Px) + (y111 - Py) * (y111 - Py);
		if (len2 < (*best * *best)) *best = (float) sqrt (len2);

		if (((x0 - Px) < *best) && ((y0 - Py) < *best) && ((Px - x1) < *best) && ((Py - y1) < *best)) {
			/* Point is inside sloppy bbox */
			/* Now we have to decide, whether subdivide */
			/* fixme: (Lauris) */
			if (((y1 - y0) > 5.0) || ((x1 - x0) > 5.0)) {
				needdist = 1;
			} else {
				needline = 1;
			}
		}
	}
	if (!needdist && wind) {
		if ((y1 >= Py) && (y0 < Py) && (x0 < Px)) {
			/* Possible intersection at the left */
			/* Now we have to decide, whether subdivide */
			/* fixme: (Lauris) */
			if (((y1 - y0) > 5.0) || ((x1 - x0) > 5.0)) {
				needwind = 1;
			} else {
				needline = 1;
			}
		}
	}

	if (needdist || needwind) {
		double x00t, x0tt, xttt, x1tt, x11t, x01t;
		double y00t, y0tt, yttt, y1tt, y11t, y01t;
		double s, t;

		t = 0.5;
		s = 1 - t;

		x00t = s * x000 + t * x001;
		x01t = s * x001 + t * x011;
		x11t = s * x011 + t * x111;
		x0tt = s * x00t + t * x01t;
		x1tt = s * x01t + t * x11t;
		xttt = s * x0tt + t * x1tt;

		y00t = s * y000 + t * y001;
		y01t = s * y001 + t * y011;
		y11t = s * y011 + t * y111;
		y0tt = s * y00t + t * y01t;
		y1tt = s * y01t + t * y11t;
		yttt = s * y0tt + t * y1tt;

		nr_curve_bbox_wind_distance (x000, y000, x00t, y00t, x0tt, y0tt, xttt, yttt, pt, NULL, wind, best, tolerance);
		nr_curve_bbox_wind_distance (xttt, yttt, x1tt, y1tt, x11t, y11t, x111, y111, pt, NULL, wind, best, tolerance);
	} else if (1 || needline) {
		nr_line_wind_distance (x000, y000, x111, y111, pt->x, pt->y, wind, best);
	}
}

struct WDData {
	float x, y;
	float dist;
	int wind;
	float tolerance;
};

static unsigned int
wg_lineto (float x0, float y0, float x1, float y1, unsigned int flags, void *data)
{
	struct WDData *wdd = (struct WDData *) data;
	nr_line_wind_distance (x0, y0, x1, y1, wdd->x, wdd->y, &wdd->wind, &wdd->dist);
	return TRUE;
}

static unsigned int
wg_curveto2 (float x0, float y0, float x1, float y1, float x2, float y2, unsigned int flags, void *data)
{
	struct WDData *wdd = (struct WDData *) data;
	NRPointF p;
	p.x = wdd->x;
	p.y = wdd->y;
	nr_curve_bbox_wind_distance (x0, y0, (x0 + 2 * x1) / 3, (y0 + 2 * y1) / 3, (2 * x1 + x2) / 3, (2 * y1 + y2) / 3, x2, y2, &p, NULL, &wdd->wind, &wdd->dist, wdd->tolerance);
	return TRUE;
}

static unsigned int
wg_curveto3 (float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, unsigned int flags, void *data)
{
	struct WDData *wdd = (struct WDData *) data;
	NRPointF p;
	p.x = wdd->x;
	p.y = wdd->y;
	nr_curve_bbox_wind_distance (x0, y0, x1, y1, x2, y2, x3, y3, &p, NULL, &wdd->wind, &wdd->dist, wdd->tolerance);
	return TRUE;
}

static unsigned int
wg_endpath (float ex, float ey, float sx, float sy, unsigned int flags, void *data)
{
	struct WDData *wdd = (struct WDData *) data;
	if ((flags & NR_PATH_CLOSED) && ((ex != sx) || (ey != sy))) {
		nr_line_wind_distance (ex, ey, sx, sy, wdd->x, wdd->y, &wdd->wind, &wdd->dist);
	}
	return TRUE;
}

float
nr_path_matrix_f_wind_distance (const NRPath *path, const NRMatrixF *m, float x, float y, int *wind, float tolerance)
{
	static NRPathGVector wdv = { NULL, wg_lineto, wg_curveto2, wg_curveto3, wg_endpath };
	struct WDData wdd;
	wdd.x = x;
	wdd.y = y;
	wdd.dist = NR_HUGE_F;
	wdd.wind = 0;
	wdd.tolerance = tolerance;
	if (nr_path_forall (path, m, &wdv, &wdd)) {
		*wind = wdd.wind;
		return wdd.dist;
	}
	return NR_HUGE_F;
}

/* Fast bbox calculation */
/* Thanks to Nathan Hurst for suggesting it */

static void
nr_curve_bbox (double x000, double y000, double x001, double y001, double x011, double y011, double x111, double y111, NRRectF *bbox)
{
	double a, b, c, D;

	bbox->x0 = (float) MIN (bbox->x0, x111);
	bbox->y0 = (float) MIN (bbox->y0, y111);
	bbox->x1 = (float) MAX (bbox->x1, x111);
	bbox->y1 = (float) MAX (bbox->y1, y111);

	/*
	 * xttt = s * (s * (s * x000 + t * x001) + t * (s * x001 + t * x011)) + t * (s * (s * x001 + t * x011) + t * (s * x011 + t * x111))
	 * xttt = s * (s2 * x000 + s * t * x001 + t * s * x001 + t2 * x011) + t * (s2 * x001 + s * t * x011 + t * s * x011 + t2 * x111)
	 * xttt = s * (s2 * x000 + 2 * st * x001 + t2 * x011) + t * (s2 * x001 + 2 * st * x011 + t2 * x111)
	 * xttt = s3 * x000 + 2 * s2t * x001 + st2 * x011 + s2t * x001 + 2st2 * x011 + t3 * x111
	 * xttt = s3 * x000 + 3s2t * x001 + 3st2 * x011 + t3 * x111
	 * xttt = s3 * x000 + (1 - s) 3s2 * x001 + (1 - s) * (1 - s) * 3s * x011 + (1 - s) * (1 - s) * (1 - s) * x111
	 * xttt = s3 * x000 + (3s2 - 3s3) * x001 + (3s - 6s2 + 3s3) * x011 + (1 - 2s + s2 - s + 2s2 - s3) * x111
	 * xttt = (x000 - 3 * x001 + 3 * x011 -     x111) * s3 +
	 *        (       3 * x001 - 6 * x011 + 3 * x111) * s2 +
	 *        (                  3 * x011 - 3 * x111) * s  +
	 *        (                                 x111)
	 * xttt' = (3 * x000 - 9 * x001 +  9 * x011 - 3 * x111) * s2 +
	 *         (           6 * x001 - 12 * x011 + 6 * x111) * s  +
	 *         (                       3 * x011 - 3 * x111)
	 */

	/* a * s2 + b * s + c == 0 */

	a = 3 * x000 - 9 * x001 + 9 * x011 - 3 * x111;
	b = 6 * x001 - 12 * x011 + 6 * x111;
	c = 3 * x011 - 3 * x111;

	if (fabs (a) < NR_EPSILON_F) {
		/* s = -c / b */
		if (fabs (b) > NR_EPSILON_F) {
			double s, t, xttt;
			s = -c / b;
			if ((s > 0.0) && (s < 1.0)) {
				t = 1.0 - s;
				xttt = s * s * s * x000 + 3 * s * s * t * x001 + 3 * s * t * t * x011 + t * t * t * x111;
				bbox->x0 = (float) MIN (bbox->x0, xttt);
				bbox->x1 = (float) MAX (bbox->x1, xttt);
			}
		}
	} else {
		/* s = (-b +/- sqrt (b * b - 4 * a * c)) / 2 * a; */
		D = b * b - 4 * a * c;
		if (D >= 0.0) {
			double d, s, t, xttt;
			/* Have solution */
			d = sqrt (D);
			s = (-b + d) / (2 * a);
			if ((s > 0.0) && (s < 1.0)) {
				t = 1.0 - s;
				xttt = s * s * s * x000 + 3 * s * s * t * x001 + 3 * s * t * t * x011 + t * t * t * x111;
				bbox->x0 = (float) MIN (bbox->x0, xttt);
				bbox->x1 = (float) MAX (bbox->x1, xttt);
			}
			s = (-b - d) / (2 * a);
			if ((s > 0.0) && (s < 1.0)) {
				t = 1.0 - s;
				xttt = s * s * s * x000 + 3 * s * s * t * x001 + 3 * s * t * t * x011 + t * t * t * x111;
				bbox->x0 = (float) MIN (bbox->x0, xttt);
				bbox->x1 = (float) MAX (bbox->x1, xttt);
			}
		}
	}

	a = 3 * y000 - 9 * y001 + 9 * y011 - 3 * y111;
	b = 6 * y001 - 12 * y011 + 6 * y111;
	c = 3 * y011 - 3 * y111;

	if (fabs (a) < NR_EPSILON_F) {
		/* s = -c / b */
		if (fabs (b) > NR_EPSILON_F) {
			double s, t, yttt;
			s = -c / b;
			if ((s > 0.0) && (s < 1.0)) {
				t = 1.0 - s;
				yttt = s * s * s * y000 + 3 * s * s * t * y001 + 3 * s * t * t * y011 + t * t * t * y111;
				bbox->y0 = (float) MIN (bbox->y0, yttt);
				bbox->y1 = (float) MAX (bbox->y1, yttt);
			}
		}
	} else {
		/* s = (-b +/- sqrt (b * b - 4 * a * c)) / 2 * a; */
		D = b * b - 4 * a * c;
		if (D >= 0.0) {
			double d, s, t, yttt;
			/* Have solution */
			d = sqrt (D);
			s = (-b + d) / (2 * a);
			if ((s > 0.0) && (s < 1.0)) {
				t = 1.0 - s;
				yttt = s * s * s * y000 + 3 * s * s * t * y001 + 3 * s * t * t * y011 + t * t * t * y111;
				bbox->y0 = (float) MIN (bbox->y0, yttt);
				bbox->y1 = (float) MAX (bbox->y1, yttt);
			}
			s = (-b - d) / (2 * a);
			if ((s > 0.0) && (s < 1.0)) {
				t = 1.0 - s;
				yttt = s * s * s * y000 + 3 * s * s * t * y001 + 3 * s * t * t * y011 + t * t * t * y111;
				bbox->y0 = (float) MIN (bbox->y0, yttt);
				bbox->y1 = (float) MAX (bbox->y1, yttt);
			}
		}
	}
}

