#ifndef __NR_PIXBLOCK_TRANSFORM_H__
#define __NR_PIXBLOCK_TRANSFORM_H__

/*
 * Pixel buffer rendering library
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#include <libnr/nr-types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Destination has to be of the same mode as source */
/* Destination region has to be allocated */
/* Destination will be marked dirty if areas overlap */

void nr_pixblock_transform (NRPixBlock *dpx, const NRPixBlock *spx, const NRMatrixF *d2s, NRULong clearrgba);

/* Convenience method for orthogonal transform */

void nr_pixblock_scale (NRPixBlock *dpx, const NRPixBlock *spx);

#ifdef __cplusplus
};
#endif

#endif
