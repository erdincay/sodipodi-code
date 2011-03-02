#ifndef __NR_IMAGE_H__
#define __NR_IMAGE_H__

/*
 * Pixel buffer rendering library
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#include <libnr/nr-types.h>
#include <libnr/nr-pixblock.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * NRImage
 *
 * An pixel buffer with reference counting
 *
 */

struct _NRImage {
	unsigned int refcount;

	NRPixBlock pixels;
};

NRImage *nr_image_new (void);
NRImage *nr_image_new_sized (unsigned int mode, int x0, int y0, int x1, int y1, unsigned int clear);
void nr_image_ref (NRImage *image);
void nr_image_unref (NRImage *image);

/* Copy on write semantics implementation */
/* It is allowed to call it with NULL image */
NRImage *nr_image_ensure_private_empty (NRImage *image);

/* NULL is allowed */
unsigned int nr_image_is_empty (const NRImage *image);

#ifdef __cplusplus
};
#endif

#endif
