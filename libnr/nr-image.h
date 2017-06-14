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

#define NR_IMAGE(i) ((NRImage *) (i))
#define NR_TYPE_IMAGE (nr_image_get_type ())

typedef struct _NRImage NRImage;
typedef struct _NRImageClass NRImageClass;

#include <az/reference.h>

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
	AZReference reference;

	NRPixBlock pixels;
};

struct _NRImageClass {
	AZReferenceClass reference_class;
};

unsigned int nr_image_get_type (void);

NRImage *nr_image_new (void);
NRImage *nr_image_new_sized (unsigned int mode, int x0, int y0, int x1, int y1, unsigned int clear);
void nr_image_ref (NRImage *image);
void nr_image_unref (NRImage *image);

/* Copy on write semantics implementation */
/* It is allowed to call it with NULL image */
NRImage *nr_image_ensure_private_empty (NRImage *image);

/* NULL is allowed */
unsigned int nr_image_is_empty (const NRImage *image);

/* Get scaled image (or original if scales match) */
NRImage *nr_image_get_scaled (NRImage *image, unsigned int width, unsigned int height);

/* Get specific type of image (or original if types match) */
NRImage *nr_image_get_typed (NRImage *image, unsigned int mode);

/* Apply mask to image */
/* The sizes of image and mask must match */
/* Resulting image is either R8G8B8A8_N or R8G8B8A8_P */
NRImage *nr_image_apply_mask (NRImage *image, NRImage *mask, unsigned int premultiply);

#ifdef __cplusplus
};
#endif

#endif
