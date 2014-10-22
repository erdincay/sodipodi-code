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

#include "nr-pixblock-transform.h"
#include "nr-blit.h"

#include "nr-image.h"

static void nr_image_class_init (NRImageClass *klass);
static void nr_image_finalize (NRImage *image);

static NRImageClass *image_class;

unsigned int
nr_image_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		arikkei_register_type (&type, ARIKKEI_TYPE_REFERENCE,
						(const unsigned char *) "NRImage",
						sizeof (NRImageClass),
						sizeof (NRImage),
						(void (*) (ArikkeiClass *)) nr_image_class_init,
						NULL,
						(void (*) (void *)) nr_image_finalize);
	}
	return type;
}

static void
nr_image_class_init (NRImageClass *klass)
{
	image_class = klass;
	((ArikkeiClass *) klass)->zero_memory = 1;
}

static void
nr_image_finalize (NRImage *image)
{
	nr_pixblock_release (&image->pixels);
}

NRImage *
nr_image_new (void)
{
	NRImage *image = (NRImage *) arikkei_instance_new (NR_TYPE_IMAGE);
	nr_pixblock_setup_fast (&image->pixels, NR_PIXBLOCK_MODE_R8G8B8A8N, 0, 0, 0, 0, 0);
	return image;
}

NRImage *
nr_image_new_sized (unsigned int mode, int x0, int y0, int x1, int y1, unsigned int clear)
{
	NRImage *image = (NRImage *) arikkei_instance_new (NR_TYPE_IMAGE);
	nr_pixblock_setup (&image->pixels, mode, x0, y0, x1, y1, clear);
	return image;
}

void
nr_image_ref (NRImage *image)
{
	arikkei_reference_ref ((ArikkeiClass *) image_class, ARIKKEI_REFERENCE(image));
}

void
nr_image_unref (NRImage *image)
{
	arikkei_reference_unref ((ArikkeiClass *) image_class, ARIKKEI_REFERENCE(image));
}

NRImage *
nr_image_ensure_private_empty (NRImage *image)
{
	assert (!image || (((ArikkeiReference *) image)->refcount > 0));

	if (image) {
		if (((ArikkeiReference *) image)->refcount == 1) {
			nr_pixblock_setup (&image->pixels, NR_PIXBLOCK_MODE_R8G8B8A8N, 0, 0, 0, 0, 0);
			return image;
		}
		nr_image_unref (image);
	}
	return nr_image_new ();
}

unsigned int
nr_image_is_empty (const NRImage *image)
{
	if (!image) return 0;
	return image->pixels.empty;
}

NRImage *
nr_image_get_scaled (NRImage *image, unsigned int width, unsigned int height)
{
	unsigned int sw, sh;
	NRImage *dst;
	sw = image->pixels.area.x1 - image->pixels.area.x0;
	sh = image->pixels.area.y1 - image->pixels.area.y0;
	if ((sw == width) && (sh == height)) {
		nr_image_ref (image);
		return image;
	}
	dst = nr_image_new ();
	nr_pixblock_release (&dst->pixels);
	nr_pixblock_setup (&dst->pixels, image->pixels.mode, 0, 0, width, height, 0);
	if (image->pixels.empty) return dst;
	if (width && height) {
		nr_pixblock_scale (&dst->pixels, &image->pixels);
	}
	return dst;
}

NRImage *
nr_image_get_typed (NRImage *image, unsigned int mode)
{
	NRImage *dst;
	if (mode == image->pixels.mode) {
		nr_image_ref (image);
		return image;
	}
	dst = nr_image_new ();
	nr_pixblock_release (&dst->pixels);
	nr_pixblock_setup (&dst->pixels, mode, image->pixels.area.x0, image->pixels.area.y0, image->pixels.area.x1, image->pixels.area.y1, 0);
	if (image->pixels.empty) return dst;
	if ((image->pixels.area.x0 >= image->pixels.area.x1) && (image->pixels.area.y0 >= image->pixels.area.y1)) return dst;
	nr_blit_pixblock_pixblock (&dst->pixels, &image->pixels);
	dst->pixels.empty = 0;
	return dst;
}

NRImage *
nr_image_apply_mask (NRImage *image, NRImage *mask, unsigned int premultiply)
{
	NRImage *dst;
	if ((image->pixels.area.x0 != mask->pixels.area.x0) || (image->pixels.area.y0 != mask->pixels.area.y0)) return NULL;
	if ((image->pixels.area.x1 != mask->pixels.area.x1) || (image->pixels.area.y1 != mask->pixels.area.y1)) return NULL;
	if ((mask->pixels.empty) || (image->pixels.area.x0 >= image->pixels.area.x1) || (image->pixels.area.y0 >= image->pixels.area.y1)) {
		return nr_image_get_typed (image, (premultiply) ? NR_PIXBLOCK_MODE_R8G8B8A8P : NR_PIXBLOCK_MODE_R8G8B8A8N);
	}
	dst = nr_image_new ();
	nr_pixblock_release (&dst->pixels);
	nr_pixblock_setup (&dst->pixels, (premultiply) ? NR_PIXBLOCK_MODE_R8G8B8A8P : NR_PIXBLOCK_MODE_R8G8B8A8N, image->pixels.area.x0, image->pixels.area.y0, image->pixels.area.x1, image->pixels.area.y1, 0);
	nr_blit_pixblock_pixblock_mask (&dst->pixels, &image->pixels, &mask->pixels);
	dst->pixels.empty = 0;
	return dst;
}
