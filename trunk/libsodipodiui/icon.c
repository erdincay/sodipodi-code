#define __SP_ICON_C__

/*
 * Generic icon widget
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#define SODIPODI_PIXMAPDIR ""

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <sys/stat.h>

#include <libnr/nr-macros.h>
#include <libnr/nr-rect.h>
#include <libnr/nr-matrix.h>
#include <libnr/nr-pixblock-pattern.h>
#include <libnr/nr-pixops.h>
#include <libnr/nr-blit.h>
#include <libnr/nr-image.h>

#include <gtk/gtkiconfactory.h>

#include "icon.h"

static void sp_icon_class_init (SPIconClass *klass);
static void sp_icon_init (SPIcon *icon);
static void sp_icon_destroy (GtkObject *object);

static void sp_icon_size_request (GtkWidget *widget, GtkRequisition *requisition);
static void sp_icon_size_allocate (GtkWidget *widget, GtkAllocation *allocation);
static int sp_icon_expose (GtkWidget *widget, GdkEventExpose *event);

static void sp_icon_paint (SPIcon *icon, GdkRectangle *area);

static NRImage *sp_icon_image_load_pixblock (NRPixBlock *pxb, unsigned int size);
static NRImage *sp_icon_image_load_from_file (const unsigned char *path, unsigned int size);

static GtkWidgetClass *parent_class;

GtkType
sp_icon_get_type (void)
{
	static GtkType type = 0;
	if (!type) {
		GtkTypeInfo info = {
			"SPIcon",
			sizeof (SPIcon),
			sizeof (SPIconClass),
			(GtkClassInitFunc) sp_icon_class_init,
			(GtkObjectInitFunc) sp_icon_init,
			NULL, NULL, NULL
		};
		type = gtk_type_unique (GTK_TYPE_WIDGET, &info);
	}
	return type;
}

static void
sp_icon_class_init (SPIconClass *klass)
{
	GtkObjectClass *object_class;
	GtkWidgetClass *widget_class;

	object_class = (GtkObjectClass *) klass;
	widget_class = (GtkWidgetClass *) klass;

	parent_class = g_type_class_peek_parent (klass);

	object_class->destroy = sp_icon_destroy;

	widget_class->size_request = sp_icon_size_request;
	widget_class->size_allocate = sp_icon_size_allocate;
	widget_class->expose_event = sp_icon_expose;
}

static void
sp_icon_init (SPIcon *icon)
{
	GTK_WIDGET_FLAGS (icon) |= GTK_NO_WINDOW;
}

static void
sp_icon_destroy (GtkObject *object)
{
	SPIcon *icon;

	icon = SP_ICON (object);

	if (icon->image) {
		nr_image_unref (icon->image);
		icon->image = NULL;
	}

	((GtkObjectClass *) (parent_class))->destroy (object);
}

static void
sp_icon_size_request (GtkWidget *widget, GtkRequisition *requisition)
{
	SPIcon *icon;

	icon = SP_ICON (widget);

	requisition->width = icon->size;
	requisition->height = icon->size;
}

static void
sp_icon_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
	widget->allocation = *allocation;

	if (GTK_WIDGET_DRAWABLE (widget)) {
		gtk_widget_queue_draw (widget);
	}
}

static int
sp_icon_expose (GtkWidget *widget, GdkEventExpose *event)
{
	if (GTK_WIDGET_DRAWABLE (widget)) {
		sp_icon_paint (SP_ICON (widget), &event->area);
	}

	return TRUE;
}

GtkWidget *
sp_icon_new (const unsigned char *name, unsigned int size)
{
	SPIcon *icon;
	icon = g_object_new (SP_TYPE_ICON, NULL);
	icon->size = CLAMP (size, 1, 128);
	icon->image = sp_icon_image_load_gtk (GTK_WIDGET(icon), name, icon->size);
	return (GtkWidget *) icon;
}

GtkWidget *
sp_icon_new_from_image (NRImage *image)
{
	SPIcon *icon;
	g_return_val_if_fail (image != NULL, NULL);
	icon = g_object_new (SP_TYPE_ICON, NULL);
	icon->size = image->pixels.area.x1 - image->pixels.area.x0;
	icon->image = image;
	if (icon->image) nr_image_ref (icon->image);
	return (GtkWidget *) icon;
}

GtkWidget *
sp_icon_new_from_data (const unsigned char *name, unsigned int size, const unsigned char *px)
{
	SPIcon *icon;
	icon = g_object_new (SP_TYPE_ICON, NULL);
	icon->size = CLAMP (size, 1, 128);
	icon->image = nr_image_new ();
	nr_pixblock_setup_extern (&icon->image->pixels, NR_PIXBLOCK_MODE_R8G8B8A8N, 0, 0, size, size, (unsigned char *) px, 4 * size, 0, 0);
	return (GtkWidget *) icon;
}

int
sp_icon_get_gtk_size (int size)
{
	static int map[64] = {0};
	size = CLAMP (size, 4, 63);
	if (!map[size]) {
		static int count = 0;
		char c[64];
		g_snprintf (c, 64, "SodipodiIcon%d", count++);
		map[size] = gtk_icon_size_register (c, size, size);
	}
	return map[size];
}

static void
sp_icon_paint (SPIcon *icon, GdkRectangle *area)
{
	GtkWidget *widget;
	int padx, pady;
	int x0, y0, x1, y1, x, y;

	widget = GTK_WIDGET (icon);

	padx = (widget->allocation.width - icon->size) / 2;
	pady = (widget->allocation.height - icon->size) / 2;

	x0 = MAX (area->x, widget->allocation.x + padx);
	y0 = MAX (area->y, widget->allocation.y + pady);
	x1 = MIN (area->x + area->width, widget->allocation.x + padx + (int) icon->size);
	y1 = MIN (area->y + area->height, widget->allocation.y + pady + (int) icon->size);

	for (y = y0; y < y1; y += 64) {
		for (x = x0; x < x1; x += 64) {
			NRPixBlock bpb;
			int xe, ye;
			xe = MIN (x + 64, x1);
			ye = MIN (y + 64, y1);
			nr_pixblock_setup_fast (&bpb, NR_PIXBLOCK_MODE_R8G8B8, x, y, xe, ye, FALSE);

			if (icon->image) {
				GdkColor *color;
				unsigned int br, bg, bb;
				int xx, yy;

				/* fixme: We support only plain-color themes */
				/* fixme: But who needs other ones anyways? (Lauris) */
				color = &widget->style->bg[widget->state];
				br = (color->red & 0xff00) >> 8;
				bg = (color->green & 0xff00) >> 8;
				bb = (color->blue & 0xff00) >> 8;

				for (yy = y; yy < ye; yy++) {
					const unsigned char *s;
					unsigned char *d;
					d = NR_PIXBLOCK_PX (&bpb) + (yy - y) * bpb.rs;
					s = NR_PIXBLOCK_ROW(&icon->image->pixels, yy - pady - widget->allocation.y) + 4 * (x - padx - widget->allocation.x);
					for (xx = x; xx < xe; xx++) {
						d[0] = NR_COMPOSEN11 (s[0], s[3], br);
						d[1] = NR_COMPOSEN11 (s[1], s[3], bg);
						d[2] = NR_COMPOSEN11 (s[2], s[3], bb);
						s += 4;
						d += 3;
					}
				}
			} else {
				nr_pixblock_render_gray_noise (&bpb, NULL);
			}

			gdk_draw_rgb_image (widget->window, widget->style->black_gc,
					    x, y,
					    xe - x, ye - y,
					    GDK_RGB_DITHER_MAX,
					    NR_PIXBLOCK_PX (&bpb), bpb.rs);

			nr_pixblock_release (&bpb);
		}
	}
}

static GHashTable *loaderdict = NULL;

struct IconClassLoader {
	char *prefix;
	GSList *loaders;
};

struct IconLoader {
	NRImage *(* loader) (const unsigned char *, unsigned int, void *);
	void *data;
};

void
sp_icon_register_loader (const unsigned char *prefix, NRImage *(* loader) (const unsigned char *, unsigned int, void *), void *data)
{
	struct IconClassLoader *lc;
	struct IconLoader *l;
	if (!loaderdict) loaderdict = g_hash_table_new (g_str_hash, g_str_equal);
	if (!prefix) prefix = "*";
	lc = g_hash_table_lookup (loaderdict, prefix);
	if (!lc) {
		lc = (struct IconClassLoader *) malloc (sizeof (struct IconClassLoader));
		lc->prefix = g_strdup (prefix);
		lc->loaders = NULL;
		g_hash_table_insert (loaderdict, lc->prefix, lc);
	}
	l = (struct IconLoader *) malloc (sizeof (struct IconLoader));
	l->loader = loader;
	l->data = data;
	lc->loaders = g_slist_append (lc->loaders, l);
}

NRImage *
sp_icon_image_load_gtk (GtkWidget *widget, const unsigned char *name, unsigned int size)
{
	if (!name || !*name) return NULL;
	/* fixme: Make stock/nonstock configurable */
	if (!strncmp (name, "gtk-", 4)) {
		GdkPixbuf *pb;
		NRImage *dpx;
		const unsigned char *spx;
		int gtksize, srs;
		unsigned int y;
		gtksize = sp_icon_get_gtk_size (size);
		pb = gtk_widget_render_icon (widget, name, gtksize, NULL);
		if (!gdk_pixbuf_get_has_alpha (pb)) gdk_pixbuf_add_alpha (pb, FALSE, 0, 0, 0);
		spx = gdk_pixbuf_get_pixels (pb);
		srs = gdk_pixbuf_get_rowstride (pb);
		dpx = nr_image_new ();
		nr_pixblock_setup (&dpx->pixels, NR_PIXBLOCK_MODE_R8G8B8A8N, 0, 0, size, size, 0);
		for (y = 0; y < size; y++) {
			memcpy (NR_PIXBLOCK_ROW (&dpx->pixels, y), spx + y * srs, 4 * size);
		}
		g_object_unref (G_OBJECT(pb));
		return dpx;
	} else {
		return sp_icon_image_load (name, size);
	}
}

NRImage *
sp_icon_image_load (const unsigned char *name, unsigned int size)
{
	const unsigned char *p;
	NRImage *px;
	if (!name || !*name) return NULL;
	if (loaderdict) {
		struct IconClassLoader *lc;
		p = strchr (name, ':');
		if (p) {
			char *prefix;
			prefix = g_strndup (name, p - name);
			lc = g_hash_table_lookup (loaderdict, prefix);
			g_free (prefix);
			p = p + 1;
		} else {
			lc = g_hash_table_lookup (loaderdict, "*");
			p = name;
		}
		if (lc) {
			GSList *ll;
			for (ll = lc->loaders; ll; ll = ll->next) {
				struct IconLoader *l;
				l = (struct IconLoader *) ll->data;
				px = l->loader (p, size, l->data);
				if (px) return px;
			}
		}
	}
	px = sp_icon_image_load_from_file (name, size);
	if (!px) {
		guchar *path;
		path = (guchar *) g_strdup_printf ("%s.png", name);
		px = sp_icon_image_load_from_file (path, size);
		g_free (path);
	}
	if (!px) {
		guchar *path;
		path = (guchar *) g_strdup_printf ("%s.xpm", name);
		px = sp_icon_image_load_from_file (path, size);
		g_free (path);
	}
	return px;
}

#if 0
unsigned char *
sp_icon_image_load_pixblock (NRPixBlock *pxb, unsigned int size)
{
	NRPixBlock np;
	unsigned char *px;
	GdkPixbuf *pb;

	if (pxb->mode != NR_PIXBLOCK_MODE_R8G8B8A8N) {
		nr_pixblock_setup_fast (&np, NR_PIXBLOCK_MODE_R8G8B8A8N, pxb->area.x0, pxb->area.y0, pxb->area.x1, pxb->area.y1, FALSE);
		nr_blit_pixblock_pixblock_alpha (&np, pxb, 255);
		np.empty = FALSE;
		pxb = &np;
	}
	pb = gdk_pixbuf_new_from_data (NR_PIXBLOCK_PX(pxb), GDK_COLORSPACE_RGB, TRUE, 8, pxb->area.x1 - pxb->area.x0, pxb->area.y1 - pxb->area.y0, pxb->rs, NULL, NULL);

	if (pb) {
		unsigned char *spx;
		int srs;
		unsigned int y;
		if (!gdk_pixbuf_get_has_alpha (pb)) gdk_pixbuf_add_alpha (pb, FALSE, 0, 0, 0);
		if ((gdk_pixbuf_get_width (pb) != size) || (gdk_pixbuf_get_height (pb) != size)) {
			GdkPixbuf *spb;
			spb = gdk_pixbuf_scale_simple (pb, size, size, GDK_INTERP_HYPER);
			g_object_unref (G_OBJECT (pb));
			pb = spb;
		}
		spx = gdk_pixbuf_get_pixels (pb);
		srs = gdk_pixbuf_get_rowstride (pb);
		px = nr_new (unsigned char, 4 * size * size);
		for (y = 0; y < size; y++) {
			memcpy (px + 4 * y * size, spx + y * srs, 4 * size);
		}
		g_object_unref (G_OBJECT (pb));

		return px;
	}

	return NULL;
}
#endif

static NRImage *
sp_icon_image_load_from_file (const unsigned char *path, unsigned int size)
{
	GdkPixbuf *pb;
	pb = gdk_pixbuf_new_from_file ((const char *) path, NULL);
	if (pb) {
		NRImage *dpx;
		unsigned char *spx;
		int srs;
		unsigned int y;
		if (!gdk_pixbuf_get_has_alpha (pb)) gdk_pixbuf_add_alpha (pb, FALSE, 0, 0, 0);
		if ((gdk_pixbuf_get_width (pb) != size) || (gdk_pixbuf_get_height (pb) != size)) {
			GdkPixbuf *spb;
			spb = gdk_pixbuf_scale_simple (pb, size, size, GDK_INTERP_HYPER);
			g_object_unref (G_OBJECT (pb));
			pb = spb;
		}
		spx = gdk_pixbuf_get_pixels (pb);
		srs = gdk_pixbuf_get_rowstride (pb);
		dpx = nr_image_new ();
		nr_pixblock_setup (&dpx->pixels, NR_PIXBLOCK_MODE_R8G8B8A8N, 0, 0, size, size, 0);
		for (y = 0; y < size; y++) {
			memcpy (NR_PIXBLOCK_ROW (&dpx->pixels, y), spx + y * srs, 4 * size);
		}
		g_object_unref (G_OBJECT (pb));

		return dpx;
	}

	return NULL;
}


