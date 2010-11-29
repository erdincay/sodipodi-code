#define __SP_IMAGE_C__

/*
 * SVG <image> implementation
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 1999-2002 Lauris Kaplinski
 * Copyright (C) 2000-2001 Ximian, Inc.
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include <libnr/nr-matrix.h>

#include <libnrarena/nr-arena-item.h>

#if 0
#include <math.h>

#include <gdk-pixbuf/gdk-pixbuf.h>
#include "display/nr-arena-image.h"
#include "print.h"
#include "style.h"
#include "dialogs/object-attributes.h"
#include "helper/sp-intl.h"
#endif

#include "intl.h"
#include "attributes.h"
#include "document.h"
#include "image-loaders.h"
#include "svg/svg.h"

#include "sp-image.h"

/*
 * SPImage
 */

static void sp_image_class_init (SPImageClass *klass);
static void sp_image_init (SPImage *image);

static void sp_image_build (SPObject * object, SPDocument * document, TheraNode * node);
static void sp_image_release (SPObject * object);
static void sp_image_set (SPObject *object, unsigned int key, const unsigned char *value);
static void sp_image_update (SPObject *object, SPCtx *ctx, unsigned int flags);
static TheraNode *sp_image_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags);

static void sp_image_bbox (SPItem *item, NRRectF *bbox, const NRMatrixF *transform, unsigned int flags);
static void sp_image_print (SPItem * item, SPPrintContext *ctx);
static gchar * sp_image_description (SPItem * item);
static int sp_image_snappoints (SPItem *item, NRPointF *p, int size, const NRMatrixF *transform);
static NRArenaItem *sp_image_show (SPItem *item, NRArena *arena, unsigned int key, unsigned int flags);
static void sp_image_write_transform (SPItem *item, TheraNode *node, NRMatrixF *transform);

static void sp_image_load_image (SPImage *image);
static void sp_image_update_canvas_image (SPImage *image);

#if 0
GdkPixbuf * sp_image_repr_read_image (TheraNode * node);
static GdkPixbuf *sp_image_pixbuf_force_rgba (GdkPixbuf * pixbuf);
static GdkPixbuf * sp_image_repr_read_dataURI (const gchar * uri_data);
static GdkPixbuf * sp_image_repr_read_b64 (const gchar * uri_data);
#endif

static SPItemClass *parent_class;

GType
sp_image_get_type (void)
{
	static GType type = 0;
	if (!type) {
		GTypeInfo info = {
			sizeof (SPImageClass),
			NULL, NULL,
			(GClassInitFunc) sp_image_class_init,
			NULL, NULL,
			sizeof (SPImage),
			16,
			(GInstanceInitFunc) sp_image_init,
		};
		type = g_type_register_static (SP_TYPE_ITEM, "SPImage", &info, 0);
	}
	return type;
}

static void
sp_image_class_init (SPImageClass *klass)
{
	GObjectClass *g_object_class;
	SPObjectClass *sp_object_class;
	SPItemClass *item_class;

	g_object_class = (GObjectClass *) klass;
	sp_object_class = (SPObjectClass *) klass;
	item_class = (SPItemClass *) klass;

	parent_class = g_type_class_peek_parent (klass);

	sp_object_class->build = sp_image_build;
	sp_object_class->release = sp_image_release;
	sp_object_class->set = sp_image_set;
	sp_object_class->update = sp_image_update;
	sp_object_class->write = sp_image_write;

	item_class->bbox = sp_image_bbox;
	item_class->print = sp_image_print;
	item_class->description = sp_image_description;
	item_class->show = sp_image_show;
	item_class->snappoints = sp_image_snappoints;
	item_class->write_transform = sp_image_write_transform;
}

static void
sp_image_init (SPImage *image)
{
}

static void
sp_image_build (SPObject *object, SPDocument *document, TheraNode *node)
{
	SPImage *image;

	image = SP_IMAGE(object);

	if (((SPObjectClass *) parent_class)->build)
		((SPObjectClass *) parent_class)->build (object, document, node);

	sp_object_read_attribute (object, SP_ATTR_XLINK_HREF);
	sp_object_read_attribute (object, SP_ATTR_X);
	sp_object_read_attribute (object, SP_ATTR_Y);
	sp_object_read_attribute (object, SP_ATTR_WIDTH);
	sp_object_read_attribute (object, SP_ATTR_HEIGHT);

	/* Register */
	sp_document_add_resource (document, "image", object);
}

static void
sp_image_release (SPObject *object)
{
	SPImage *image;

	image = SP_IMAGE(object);

	if (image->pixels) {
		nr_image_unref (image->pixels);
		image->pixels = NULL;
	}

	/* Unregister */
	sp_document_remove_resource (object->document, "image", object);

	if (image->href) {
		free (image->href);
		image->href = NULL;
		image->href_set = 0;
	}

	if (((SPObjectClass *) parent_class)->release)
		((SPObjectClass *) parent_class)->release (object);
}

static void
sp_image_set (SPObject *object, unsigned int key, const unsigned char *value)
{
	SPImage *image;
	unsigned int unit;

	image = SP_IMAGE(object);

	switch (key) {
	case SP_ATTR_XLINK_HREF:
		if (image->href) {
			free (image->href);
			image->href = NULL;
			image->href_set = 0;
		}
		if (value) {
			image->href = strdup (value);
			image->href_set = TRUE;
		} else {
			image->href_set = FALSE;
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG | SP_IMAGE_HREF_MODIFIED_FLAG);
		break;
	case SP_ATTR_X:
		if (sp_svg_length_read_uff (value, &unit, &image->x.value, &image->x.computed) &&
		    /* fixme: These are probably valid, but require special treatment (Lauris) */
		    (unit != SP_SVG_UNIT_EM) && (unit != SP_SVG_UNIT_EX) && (unit != SP_SVG_UNIT_PERCENT)) {
			image->x.set = TRUE;
			image->x.unit = unit;
		} else {
			sp_svg_length_unset (&image->x, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_Y:
		if (sp_svg_length_read_uff (value, &unit, &image->y.value, &image->y.computed) &&
		    /* fixme: These are probably valid, but require special treatment (Lauris) */
		    (unit != SP_SVG_UNIT_EM) && (unit != SP_SVG_UNIT_EX) && (unit != SP_SVG_UNIT_PERCENT)) {
			image->y.set = TRUE;
			image->y.unit = unit;
		} else {
			sp_svg_length_unset (&image->y, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_WIDTH:
		if (sp_svg_length_read_uff (value, &unit, &image->width.value, &image->width.computed) &&
		    /* fixme: These are probably valid, but require special treatment (Lauris) */
		    (unit != SP_SVG_UNIT_EM) && (unit != SP_SVG_UNIT_EX) && (unit != SP_SVG_UNIT_PERCENT)) {
			image->width.set = TRUE;
			image->width.unit = unit;
		} else {
			sp_svg_length_unset (&image->width, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_HEIGHT:
		if (sp_svg_length_read_uff (value, &unit, &image->height.value, &image->height.computed) &&
		    /* fixme: These are probably valid, but require special treatment (Lauris) */
		    (unit != SP_SVG_UNIT_EM) && (unit != SP_SVG_UNIT_EX) && (unit != SP_SVG_UNIT_PERCENT)) {
			image->height.set = TRUE;
			image->height.unit = unit;
		} else {
			sp_svg_length_unset (&image->height, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	default:
		if (((SPObjectClass *) (parent_class))->set)
			((SPObjectClass *) (parent_class))->set (object, key, value);
		break;
	}
}

static void
sp_image_update (SPObject *object, SPCtx *ctx, unsigned int flags)
{
	SPImage *image;

	image = (SPImage *) object;

	if (((SPObjectClass *) (parent_class))->update)
		((SPObjectClass *) (parent_class))->update (object, ctx, flags);

	if (flags & SP_IMAGE_HREF_MODIFIED_FLAG) {
		sp_image_load_image (image);
	}

	sp_image_update_canvas_image ((SPImage *) object);
}

static TheraNode *
sp_image_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags)
{
	SPImage *image;

	image = SP_IMAGE (object);

	if ((flags & SP_OBJECT_WRITE_BUILD) && !node) {
		node = thera_document_new_element (thedoc, "image");
	}

	thera_node_set_attribute (node, "xlink:href", image->href);
	/* fixme: Reset attribute if needed (Lauris) */
	if (image->x.set) thera_node_set_double (node, "x", image->x.computed);
	if (image->y.set) thera_node_set_double (node, "y", image->y.computed);
	if (image->width.set) thera_node_set_double (node, "width", image->width.computed);
	if (image->height.set) thera_node_set_double (node, "height", image->height.computed);

	if (((SPObjectClass *) (parent_class))->write)
		((SPObjectClass *) (parent_class))->write (object, thedoc, node, flags);

	return node;
}

static void
sp_image_bbox (SPItem *item, NRRectF *bbox, const NRMatrixF *transform, unsigned int flags)
{
	SPImage *image;

	image = SP_IMAGE (item);

	if ((image->width.computed > 0.0) && (image->height.computed > 0.0)) {
		float x0, y0, x1, y1;
		float x, y;

		x0 = image->x.computed;
		y0 = image->y.computed;
		x1 = x0 + image->width.computed;
		y1 = y0 + image->height.computed;

		x = NR_MATRIX_DF_TRANSFORM_X (transform, x0, y0);
		y = NR_MATRIX_DF_TRANSFORM_Y (transform, x0, y0);
		bbox->x0 = MIN (bbox->x0, x);
		bbox->y0 = MIN (bbox->y0, y);
		bbox->x1 = MAX (bbox->x1, x);
		bbox->y1 = MAX (bbox->y1, y);
		x = NR_MATRIX_DF_TRANSFORM_X (transform, x1, y0);
		y = NR_MATRIX_DF_TRANSFORM_Y (transform, x1, y0);
		bbox->x0 = MIN (bbox->x0, x);
		bbox->y0 = MIN (bbox->y0, y);
		bbox->x1 = MAX (bbox->x1, x);
		bbox->y1 = MAX (bbox->y1, y);
		x = NR_MATRIX_DF_TRANSFORM_X (transform, x1, y1);
		y = NR_MATRIX_DF_TRANSFORM_Y (transform, x1, y1);
		bbox->x0 = MIN (bbox->x0, x);
		bbox->y0 = MIN (bbox->y0, y);
		bbox->x1 = MAX (bbox->x1, x);
		bbox->y1 = MAX (bbox->y1, y);
		x = NR_MATRIX_DF_TRANSFORM_X (transform, x0, y1);
		y = NR_MATRIX_DF_TRANSFORM_Y (transform, x0, y1);
		bbox->x0 = MIN (bbox->x0, x);
		bbox->y0 = MIN (bbox->y0, y);
		bbox->x1 = MAX (bbox->x1, x);
		bbox->y1 = MAX (bbox->y1, y);
	}
}

static void
sp_image_print (SPItem *item, SPPrintContext *ctx)
{
	SPImage *image;
	NRMatrixF tp, ti, s, t;
	unsigned char *px;
	int w, h, rs;

	image = SP_IMAGE (item);

	if (!image->pixels) return;
	if ((image->width.computed <= 0) || (image->height.computed <= 0)) return;

	px = NR_PIXBLOCK_PX(&image->pixels->pixels);
	w = image->pixels->pixels.area.x1 - image->pixels->pixels.area.x0;
	h = image->pixels->pixels.area.y1 - image->pixels->pixels.area.y0;
	rs = image->pixels->pixels.rs;

	/* fixme: (Lauris) */
	nr_matrix_f_set_translate (&tp, image->x.computed, image->y.computed);
	nr_matrix_f_set_scale (&s, image->width.computed, -image->height.computed);
	nr_matrix_f_set_translate (&ti, 0.0, -1.0);

	nr_matrix_multiply_fff (&t, &s, &tp);
	nr_matrix_multiply_fff (&t, &ti, &t);

	sp_print_image_R8G8B8A8_N (ctx, px, w, h, rs, &t, SP_OBJECT_STYLE (item));
}

static gchar *
sp_image_description (SPItem * item)
{
	SPImage *image;

	image = SP_IMAGE(item);

	if (image->pixels) {
		NRRectS *area;
		area = &image->pixels->pixels.area;
		return g_strdup_printf (_("Color image %d x %d: %s"), area->x1 - area->x0, area->y1 - area->y0, image->href);
	} else {
		return g_strdup_printf (_("Image with bad reference: %s"), (image->href) ? image->href : "NULL");
	}
}

static NRArenaItem *
sp_image_show (SPItem *item, NRArena *arena, unsigned int key, unsigned int flags)
{
	SPImage *image;
	NRArenaItem *ai;

	image = SP_IMAGE(item);
	

	ai = nr_arena_item_new (arena, NR_TYPE_ARENA_IMAGE);

	nr_arena_image_set_pixels (NR_ARENA_IMAGE (ai), image->pixels);

	nr_arena_image_set_geometry (NR_ARENA_IMAGE (ai), image->x.computed, image->y.computed, image->width.computed, image->height.computed);

	return ai;
}

/*
 * Initially we'll do:
 * Transform x, y, set x, y, clear translation
 */

static void
sp_image_write_transform (SPItem *item, TheraNode *node, NRMatrixF *t)
{
	SPImage *image;
	NRMatrixF rev;
	gdouble px, py, sw, sh;

	image = SP_IMAGE (item);

	/* Calculate text start in parent coords */
	px = NR_MATRIX_DF_TRANSFORM_X (t, image->x.computed, image->y.computed);
	py = NR_MATRIX_DF_TRANSFORM_Y (t, image->x.computed, image->y.computed);

	/* Clear translation */
	t->c[4] = 0.0;
	t->c[5] = 0.0;

	/* Scalers */
	sw = sqrt (t->c[0] * t->c[0] + t->c[1] * t->c[1]);
	sh = sqrt (t->c[2] * t->c[2] + t->c[3] * t->c[3]);
	if (sw > 1e-9) {
		t->c[0] = t->c[0] / sw;
		t->c[1] = t->c[1] / sw;
	} else {
		t->c[0] = 1.0;
		t->c[1] = 0.0;
	}
	if (sh > 1e-9) {
		t->c[2] = t->c[2] / sh;
		t->c[3] = t->c[3] / sh;
	} else {
		t->c[2] = 0.0;
		t->c[3] = 1.0;
	}
	thera_node_set_double (node, "width", image->width.computed * sw);
	thera_node_set_double (node, "height", image->height.computed * sh);

	/* Find start in item coords */
	nr_matrix_f_invert (&rev, t);
	thera_node_set_double (node, "x", px * rev.c[0] + py * rev.c[2]);
	thera_node_set_double (node, "y", px * rev.c[1] + py * rev.c[3]);
}

static int
sp_image_snappoints (SPItem *item, NRPointF *p, int size, const NRMatrixF *transform)
{
	SPImage *image;
	float x0, y0, x1, y1;

	image = SP_IMAGE (item);

	/* we use corners of image only */
	x0 = image->x.computed;
	y0 = image->y.computed;
	x1 = x0 + image->width.computed;
	y1 = y0 + image->height.computed;

	return sp_corner_snappoints (p, size, transform, x0, y0, x1, y1);
}

static void
sp_image_load_image (SPImage *image)
{
	SPObject *object;

	object = SP_OBJECT(image);

	if (image->href_set) {
		unsigned char *url;
		const unsigned char *cdata;
		size_t csize;
		SPURLHandler *handler;

		cdata = NULL;
		url = sp_build_canonic_url (image->href);
		if (object->document->handler) {
			cdata = sp_url_mmap (object->document->handler, url, &csize);
			if (cdata) {
				image->pixels = nr_image_ensure_private_empty (image->pixels);
				sp_image_load_from_data (&image->pixels->pixels, cdata, csize);
				sp_url_munmap (object->document->handler, cdata, csize);
				free (url);
				return;
			}
		}
		handler = sp_url_handler_get (url);
		if (handler) {
			cdata = sp_url_mmap (handler, image->href, &csize);
			if (cdata) {
				image->pixels = nr_image_ensure_private_empty (image->pixels);
				sp_image_load_from_data (&image->pixels->pixels, cdata, csize);
				sp_url_munmap (handler, cdata, csize);
				sp_url_handler_release (handler);
				free (url);
				return;
			}
			sp_url_handler_release (handler);
		}
		free (url);
	}

	if (image->pixels) {
		nr_image_unref (image->pixels);
		image->pixels = NULL;
	}
}

static void
sp_image_update_canvas_image (SPImage *image)
{
	SPItem *item;
	SPItemView *view;

	item = SP_ITEM(image);

	if (image->pixels) {
		/* fixme: We are slightly violating spec here (Lauris) */
		if (!image->width.set) {
			image->width.computed = image->pixels->pixels.area.x1 - image->pixels->pixels.area.x0;
		}
		if (!image->height.set) {
			image->height.computed = image->pixels->pixels.area.y1 - image->pixels->pixels.area.y0;
		}
	}

	for (view = item->display; view != NULL; view = view->view.next) {
		nr_arena_image_set_pixels (NR_ARENA_IMAGE(view), image->pixels);
		nr_arena_image_set_geometry (NR_ARENA_IMAGE(view), image->x.computed, image->y.computed, image->width.computed, image->height.computed);
	}
}

/*
 * utility function to try loading image from href
 *
 * docbase/relative_src
 * absolute_src
 *
 */

#if 0
static GdkPixbuf *
gdk_pixbuf_new_from_utf8_file (const unsigned char *utf8fn, void *unused)
{
	unsigned char *osfn;
	gsize bytesin, bytesout;
	GdkPixbuf *pb;
	osfn = g_filename_from_utf8 (utf8fn, strlen (utf8fn), &bytesin, &bytesout, NULL);
	pb = gdk_pixbuf_new_from_file (osfn, NULL);
	g_free (osfn);
	return pb;
}

GdkPixbuf *
sp_image_repr_read_image (TheraNode * node)
{
	const gchar * filename, * docbase;
	gchar * fullname;
	GdkPixbuf * pixbuf;

	filename = thera_node_get_attribute (node, "xlink:href");
	if (filename == NULL) filename = thera_node_get_attribute (node, "href"); /* FIXME */
	if (filename != NULL) {
		if (strncmp (filename,"file:",5) == 0) {
			fullname = g_filename_from_uri(filename, NULL, NULL);
			if (fullname) {
				pixbuf = gdk_pixbuf_new_from_utf8_file (fullname, NULL);
				g_free (fullname);
				if (pixbuf != NULL) return pixbuf;
			}
		} else if (strncmp (filename,"data:",5) == 0) {
			/* data URI - embedded image */
			filename += 5;
			pixbuf = sp_image_repr_read_dataURI (filename);
			if (pixbuf != NULL) return pixbuf;
		} else if (!g_path_is_absolute (filename)) {
			/* try to load from relative pos */
			docbase = thera_node_get_attribute (thera_document_get_root (thera_node_get_document (node)), "sodipodi:docbase");
			if (!docbase) docbase = "./";
			fullname = g_strconcat (docbase, filename, NULL);
			pixbuf = gdk_pixbuf_new_from_utf8_file (fullname, NULL);
			g_free (fullname);
			if (pixbuf != NULL) return pixbuf;
		} else {
			/* try absolute filename */
			pixbuf = gdk_pixbuf_new_from_utf8_file (filename, NULL);
			if (pixbuf != NULL) return pixbuf;
		}
	}
	/* at last try to load from sp absolute path name */
	filename = thera_node_get_attribute (node, "sodipodi:absref");
	if (filename != NULL) {
		pixbuf = gdk_pixbuf_new_from_utf8_file (filename, NULL);
		if (pixbuf != NULL) return pixbuf;
	}
	/* Nope: We do not find any valid pixmap file :-( */
	pixbuf = gdk_pixbuf_new_from_xpm_data ((const gchar **) brokenimage_xpm);

	/* It should be included xpm, so if it still does not does load, */
	/* our libraries are broken */
	g_assert (pixbuf != NULL);

	return pixbuf;
}

static GdkPixbuf *
sp_image_pixbuf_force_rgba (GdkPixbuf * pixbuf)
{
	GdkPixbuf * newbuf;

	if (gdk_pixbuf_get_has_alpha (pixbuf)) return pixbuf;

	newbuf = gdk_pixbuf_add_alpha (pixbuf, FALSE, 0, 0, 0);
	gdk_pixbuf_unref (pixbuf);

	return newbuf;
}

#ifdef ENABLE_AUTOTRACE
static void
sp_image_autotrace (GtkMenuItem *menuitem, SPAnchor *anchor)
{
  autotrace_dialog (SP_IMAGE(anchor));
}
#endif /* Def: ENABLE_AUTOTRACE */

static GdkPixbuf *
sp_image_repr_read_dataURI (const gchar * uri_data)
{	GdkPixbuf * pixbuf = NULL;

	gint data_is_image = 0;
	gint data_is_base64 = 0;

	const gchar * data = uri_data;

	while (*data) {
		if (strncmp (data,"base64",6) == 0) {
			/* base64-encoding */
			data_is_base64 = 1;
			data += 6;
		}
		else if (strncmp (data,"image/png",9) == 0) {
			/* PNG image */
			data_is_image = 1;
			data += 9;
		}
		else if (strncmp (data,"image/jpg",9) == 0) {
			/* JPEG image */
			data_is_image = 1;
			data += 9;
		}
		else if (strncmp (data,"image/jpeg",10) == 0) {
			/* JPEG image */
			data_is_image = 1;
			data += 10;
		}
		else { /* unrecognized option; skip it */
			while (*data) {
				if (((*data) == ';') || ((*data) == ',')) break;
				data++;
			}
		}
		if ((*data) == ';') {
			data++;
			continue;
		}
		if ((*data) == ',') {
			data++;
			break;
		}
	}

	if ((*data) && data_is_image && data_is_base64) {
		pixbuf = sp_image_repr_read_b64 (data);
	}

	return pixbuf;
}

static GdkPixbuf *
sp_image_repr_read_b64 (const gchar * uri_data)
{	GdkPixbuf * pixbuf = NULL;
	GdkPixbufLoader * loader = NULL;

	gint j;
	gint k;
	gint l;
	gint b;
	gint len;
	gint eos = 0;
	gint failed = 0;

	guint32 bits;

	static const gchar B64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	const gchar* btr = uri_data;

	gchar ud[4];

	guchar bd[57];

	loader = gdk_pixbuf_loader_new ();

	if (loader == NULL) return NULL;

	while (eos == 0) {
		l = 0;
		for (j = 0; j < 19; j++) {
			len = 0;
			for (k = 0; k < 4; k++) {
				while (isspace ((int) (*btr))) {
					if ((*btr) == '\0') break;
					btr++;
				}
				if (eos) {
					ud[k] = 0;
					continue;
				}
				if (((*btr) == '\0') || ((*btr) == '=')) {
					eos = 1;
					ud[k] = 0;
					continue;
				}
				ud[k] = 64;
				for (b = 0; b < 64; b++) { /* There must a faster way to do this... ?? */
					if (B64[b] == (*btr)) {
						ud[k] = (gchar) b;
						break;
					}
				}
				if (ud[k] == 64) { /* data corruption ?? */
					eos = 1;
					ud[k] = 0;
					continue;
				}
				btr++;
				len++;
			}
			bits = (guint32) ud[0];
			bits = (bits << 6) | (guint32) ud[1];
			bits = (bits << 6) | (guint32) ud[2];
			bits = (bits << 6) | (guint32) ud[3];
			bd[l++] = (guchar) ((bits & 0xff0000) >> 16);
			if (len > 2) {
				bd[l++] = (guchar) ((bits & 0xff00) >>  8);
			}
			if (len > 3) {
				bd[l++] = (guchar)  (bits & 0xff);
			}
		}

		if (!gdk_pixbuf_loader_write (loader, (const guchar *) bd, (size_t) l, NULL)) {
			failed = 1;
			break;
		}
	}

	gdk_pixbuf_loader_close (loader, NULL);

	if (!failed) pixbuf = gdk_pixbuf_loader_get_pixbuf (loader);

	return pixbuf;
}

#endif

