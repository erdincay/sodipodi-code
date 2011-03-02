#define __NR_IMAGE_C__

/*
 * Pixel buffer rendering library
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#include <malloc.h>
#include <string.h>
#include <assert.h>

#include "nr-image.h"

NRImage *
nr_image_new (void)
{
	NRImage *image;
	image = (NRImage *) malloc (sizeof (NRImage));
	image->refcount = 1;
	nr_pixblock_setup_fast (&image->pixels, NR_PIXBLOCK_MODE_R8G8B8A8N, 0, 0, 0, 0, 0);
	return image;
}

NRImage *
nr_image_new_sized (unsigned int mode, int x0, int y0, int x1, int y1, unsigned int clear)
{
	NRImage *image;
	image = (NRImage *) malloc (sizeof (NRImage));
	image->refcount = 1;
	nr_pixblock_setup (&image->pixels, mode, x0, y0, x1, y1, clear);
	return image;
}

void
nr_image_ref (NRImage *image)
{
	assert (image != NULL);
	assert (image->refcount > 0);

	image->refcount += 1;
}

void
nr_image_unref (NRImage *image)
{
	assert (image != NULL);
	assert (image->refcount > 0);

	if (image->refcount > 1) {
		image->refcount -= 1;
	} else {
		nr_pixblock_release (&image->pixels);
		free (image);
	}
}

NRImage *
nr_image_ensure_private_empty (NRImage *image)
{
	assert (!image || (image->refcount > 0));

	if (image) {
		if (image->refcount == 1) {
			nr_pixblock_setup (&image->pixels, NR_PIXBLOCK_MODE_R8G8B8A8N, 0, 0, 0, 0, 0);
			return image;
		}
		image->refcount -= 1;
	}
	return nr_image_new ();
}

unsigned int
nr_image_is_empty (const NRImage *image)
{
	if (!image) return 0;
	return image->pixels.empty;
}

