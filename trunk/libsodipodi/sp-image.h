#ifndef __SP_IMAGE_H__
#define __SP_IMAGE_H__

/*
 * SVG <image> implementation
 *
 * Copyright (C) 1999-2010 Lauris Kaplinski
 *
 */

typedef struct _SPImage SPImage;
typedef struct _SPImageClass SPImageClass;

#define SP_TYPE_IMAGE (sp_image_get_type ())
#define SP_IMAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), SP_TYPE_IMAGE, SPImage))
#define SP_IS_IMAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SP_TYPE_IMAGE))

#include <libnr/nr-image.h>

#include <libsodipodi/sp-item.h>
#include <libsodipodi/svg/svg-types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SPImage */

struct _SPImage {
	SPItem item;

	SPSVGLength x;
	SPSVGLength y;
	SPSVGLength width;
	SPSVGLength height;

	unsigned int href_set : 1;
	unsigned char *href;

	NRImage *pixels;
};

struct _SPImageClass {
	SPItemClass parent_class;
};

GType sp_image_get_type (void);

#define SP_IMAGE_HREF_MODIFIED_FLAG SP_OBJECT_USER_MODIFIED_FLAG_A

#ifdef __cplusplus
}
#endif

#endif
