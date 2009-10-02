#ifndef __NR_PATH_H__
#define __NR_PATH_H__

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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _NRPath NRPath;

enum NRPathCode {
	NR_PATH_LINETO,
	NR_PATH_CURVETO2,
	NR_PATH_CURVETO3
};

enum {
	NR_WIND_RULE_NONZERO,
	NR_WIND_RULE_EVENODD
};

#include <libnr/nr-types.h>

typedef union _NRPathElement NRPathElement;
typedef struct _NRPathGVector NRPathGVector;

/* NB! Number of elements for each code is hardcoded */

#define NR_PATH_CODE_NUM_POINTS(c) ((c) + 1)

/* Bits 0-23 value; 24 closed; 25-31 code */

union _NRPathElement {
	unsigned int uval;
	float fval;
};

#define NR_PATH_SEGMENT_LENGTH(e) ((e)->uval & 0x7fffffff)
#define NR_PATH_SEGMENT_SET_LENGTH(e,v) ((e)->uval = (((e)->uval & 0x80000000) | ((v) & 0x7fffffff)))
#define NR_PATH_SEGMENT_IS_CLOSED(e) ((e)->uval & 0x80000000)
#define NR_PATH_SEGMENT_SET_CLOSED(e,v) ((e)->uval = (((e)->uval & 0x7fffffff) | ((v) ? 0x80000000 : 0)))

#define NR_PATH_ELEMENT_LENGTH(e) ((e)->uval & 0x00ffffff)
#define NR_PATH_ELEMENT_CODE(e) (((e)->uval & 0xfe000000) >> 25)
#define NR_PATH_ELEMENT_VALUE(e) ((e)->fval)

#define NR_PATH_ELEMENT_SET_LENGTH(e,v) ((e)->uval = (((e)->uval & 0xff000000) | ((v) & 0xffffff)))
#define NR_PATH_ELEMENT_SET_CODE(e,v) ((e)->uval = (((e)->uval & 0x01ffffff) | ((v) << 25)))
#define NR_PATH_ELEMENT_SET_VALUE(e,v) ((e)->fval = (v))

/* Return value FALSE means error and stops processing */

#define NR_PATH_CLOSED (1 << 0)
#define NR_PATH_FIRST (1 << 1)
#define NR_PATH_LAST (1 << 2)

/* If curveto2 is NULL it is emulated using curveto3 */

struct _NRPathGVector {
	unsigned int (* moveto) (float x0, float y0, unsigned int flags, void *data);
	unsigned int (* lineto) (float x0, float y0, float x1, float y1, unsigned int flags, void *data);
	unsigned int (* curveto2) (float x0, float y0, float x1, float y1, float x2, float y2,
				   unsigned int flags, void *data);
	unsigned int (* curveto3) (float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3,
				   unsigned int flags, void *data);
	unsigned int (* endpath) (float ex, float ey, float sx, float sy, unsigned int flags, void *data);
};

/*
 * Path structure
 *   Length (elements, including current field) + closed
 *   x0, y0,
 *   code x, y, [x1, y1, x2, y2] [...], code, x, y...
 */

struct _NRPath {
	/* Maximum is 2e30 elements */
	/* Number of elements, including reserved ones */
	unsigned int nelements : 30;
	/* Number of reserved elements */
	unsigned int offset : 2;
	/* Number of path segments (fixme) */
	/* unsigned int nsegments; */
	NRPathElement elements[1];
};

NRPath *nr_path_duplicate (const NRPath *path, unsigned int nreserved);
NRPath *nr_path_duplicate_transform (const NRPath *path, const NRMatrixF *transform);
/* Return TRUE if paths have identical structure (delta will be maximum difference between any points) */
unsigned int nr_path_compare (const NRPath *lhs, const NRPath *rhs, float *delta);

/* Transform can be NULL */
/* Path is NOT autoclosed */
unsigned int nr_path_forall (const NRPath *path, const NRMatrixF *transform, const NRPathGVector *gv, void *data);
unsigned int nr_path_forall_flat (const NRPath *path, const NRMatrixF *transform, float tolerance, const NRPathGVector *gv, void *data);

void nr_path_matrix_f_bbox_f_union (const NRPath *path, const NRMatrixF *m, NRRectF *bbox, float tolerance);
float nr_path_matrix_f_wind_distance (const NRPath *path, const NRMatrixF *m, float x, float y, int *wind, float tolerance);

unsigned int nr_path_get_num_segments (const NRPath *path);
unsigned int nr_path_get_num_strokes (const NRPath *path);
unsigned int nr_path_get_num_points (const NRPath *path);
unsigned int nr_path_is_empty (const NRPath *path);
unsigned int nr_path_is_shape (const NRPath *path);
#define nr_path_get_first_segment(p) (&(p)->elements[(p)->offset])
NRPathElement *nr_path_get_last_segment (NRPath *path);
unsigned int nr_path_any_closed (const NRPath *path);
unsigned int nr_path_all_closed (const NRPath *path);
unsigned int nr_path_get_first_point (const NRPath *path, float *x, float *y);
unsigned int nr_path_get_last_point (const NRPath *path, float *x, float *y);

NRPath *nr_path_combine (const NRPath *paths[], unsigned int numpaths, unsigned int join, float snaple);
unsigned int nr_path_break (NRPath *paths[], unsigned int maxpaths, const NRPath *path);
NRPath *nr_path_reverse (const NRPath *path);
NRPath *nr_path_append_continuous (const NRPath *lhs, const NRPath *rhs);
void nr_path_close_all (NRPath *path);

typedef struct _NRDynamicPath NRDynamicPath;

struct _NRDynamicPath {
	/* Reference count */
	unsigned int refcount;
	/* Number of allocated elements */
	unsigned int size;
	/* Whether data is owned by us */
	unsigned int isstatic : 1;
	/* Whether currentpoint is defined */
	unsigned int hascpt : 1;
	float cpx, cpy;
	/* Start of current segment */
	unsigned int segstart;
	/* Current pathcode position */
	unsigned int codepos;
	/* Our own path structure */
	NRPath *path;
};

void nr_dynamic_path_setup (NRDynamicPath *dpath, unsigned int nelements);
void nr_dynamic_path_setup_from_path (NRDynamicPath *dpath, NRPath *path, unsigned int isstatic, unsigned int makecopy);
void nr_dynamic_path_release (NRDynamicPath *dpath);

NRDynamicPath *nr_dynamic_path_new (unsigned int nelements);
NRDynamicPath *nr_dynamic_path_new_from_path (NRPath *path, unsigned int isstatic, unsigned int makecopy);

NRDynamicPath *nr_dynamic_path_ref (NRDynamicPath *dpath);
NRDynamicPath *nr_dynamic_path_unref (NRDynamicPath *dpath);

/* Return TRUE on success */
unsigned int nr_dynamic_path_moveto (NRDynamicPath *dpath, float x0, float y0);
unsigned int nr_dynamic_path_lineto (NRDynamicPath *dpath, float x1, float y1);
unsigned int nr_dynamic_path_curveto2 (NRDynamicPath *dpath, float x1, float y1, float x2, float y2);
unsigned int nr_dynamic_path_curveto3 (NRDynamicPath *dpath, float x1, float y1, float x2, float y2, float x3, float y3);
unsigned int nr_dynamic_path_closepath (NRDynamicPath *dpath);

void nr_dynamic_path_reset (NRDynamicPath *dpath);
void nr_dynamic_path_set_path (NRDynamicPath *dpath, NRPath *path, unsigned int isstatic, unsigned int makecopy);

#ifdef __cplusplus
};
#endif

#endif
