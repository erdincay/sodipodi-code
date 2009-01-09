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
#include <libnr/nr-pixblock.h>
#include <libnr/nr-pixblock-pattern.h>
#include <libnr/nr-pixops.h>

#include <gtk/gtkiconfactory.h>

#include "icon.h"

static void sp_icon_class_init (SPIconClass *klass);
static void sp_icon_init (SPIcon *icon);
static void sp_icon_destroy (GtkObject *object);

static void sp_icon_size_request (GtkWidget *widget, GtkRequisition *requisition);
static void sp_icon_size_allocate (GtkWidget *widget, GtkAllocation *allocation);
static int sp_icon_expose (GtkWidget *widget, GdkEventExpose *event);

static void sp_icon_paint (SPIcon *icon, GdkRectangle *area);

static unsigned char *sp_icon_image_load_pixmap (const unsigned char *name, unsigned int size, unsigned int scale);
static unsigned char *sp_icon_image_load_svg (const unsigned char *name, unsigned int size, unsigned int scale);

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

	if (icon->px) {
		if (!(GTK_OBJECT_FLAGS (icon) & SP_ICON_FLAG_STATIC_DATA)) {
			nr_free (icon->px);
		}
		icon->px = NULL;
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

static GtkWidget *
sp_icon_new_full (unsigned int size, unsigned int scale, const unsigned char *name)
{
	static GHashTable *iconlib = NULL;
	char c[256];
	SPIcon *icon;

	if (!iconlib) iconlib = g_hash_table_new (g_str_hash, g_str_equal);

	icon = g_object_new (SP_TYPE_ICON, NULL);

	icon->size = CLAMP (size, 1, 128);
	g_snprintf (c, 256, "%d:%d:%s", icon->size, scale, name);
	icon->px = g_hash_table_lookup (iconlib, c);
	if (!icon->px) {
		icon->px = sp_icon_image_load_gtk ((GtkWidget *) icon, name, icon->size, scale);
		g_hash_table_insert (iconlib, g_strdup (c), icon->px);
	}

	GTK_OBJECT_FLAGS (icon) |= SP_ICON_FLAG_STATIC_DATA;

	return (GtkWidget *) icon;
}

GtkWidget *
sp_icon_new (unsigned int size, const unsigned char *name)
{
	return sp_icon_new_full (size, size, name);
}

GtkWidget *
sp_icon_new_scaled (unsigned int size, const unsigned char *name)
{
	return sp_icon_new_full (size, 24, name);
}

GtkWidget *
sp_icon_new_from_data (unsigned int size, const unsigned char *px)
{
	SPIcon *icon;

	icon = g_object_new (SP_TYPE_ICON, NULL);

	icon->size = CLAMP (size, 1, 128);

	GTK_OBJECT_FLAGS (icon) |= SP_ICON_FLAG_STATIC_DATA;
	icon->px = (unsigned char *) px;

	return (GtkWidget *) icon;
}

unsigned char *
sp_icon_image_load (const unsigned char *name, unsigned int size, unsigned int scale)
{
	unsigned char *px;

	px = sp_icon_image_load_pixmap (name, size, scale);

	return px;
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

unsigned char *
sp_icon_image_load_gtk (GtkWidget *widget, const unsigned char *name, unsigned int size, unsigned int scale)
{
	if (!name || !*name) return NULL;
	/* fixme: Make stock/nonstock configurable */
	if (!strncmp (name, "gtk-", 4)) {
		GdkPixbuf *pb;
		unsigned char *px, *spx;
		int gtksize, srs;
		unsigned int y;
		gtksize = sp_icon_get_gtk_size (size);
		pb = gtk_widget_render_icon (widget, name, gtksize, NULL);
		if (!gdk_pixbuf_get_has_alpha (pb)) gdk_pixbuf_add_alpha (pb, FALSE, 0, 0, 0);
		spx = gdk_pixbuf_get_pixels (pb);
		srs = gdk_pixbuf_get_rowstride (pb);
		px = nr_new (unsigned char, 4 * size * size);
		for (y = 0; y < size; y++) {
			memcpy (px + 4 * y * size, spx + y * srs, 4 * size);
		}
		g_object_unref ((GObject *) pb);
		return px;
	} else {
		return sp_icon_image_load (name, size, scale);
	}
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

			if (icon->px) {
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
					s = icon->px + 4 * (yy - pady - widget->allocation.y) * icon->size + 4 * (x - padx - widget->allocation.x);
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

static unsigned char *
sp_icon_image_load_pixmap (const unsigned char *name, unsigned int size, unsigned int scale)
{
	unsigned char *path, *fn;
	unsigned char *px;
	GdkPixbuf *pb;

	fn = g_strdup_printf ("%s.png", name);
	path = (unsigned char *) g_build_filename (SODIPODI_PIXMAPDIR, fn, NULL);
	g_free (fn);
	pb = gdk_pixbuf_new_from_file ((const char *) path, NULL);
	g_free (path);
	if (!pb) {
		fn = g_strdup_printf ("%s.xpm", name);
		path = (unsigned char *) g_build_filename (SODIPODI_PIXMAPDIR, fn, NULL);
		g_free (fn);
		pb = gdk_pixbuf_new_from_file ((const char *) path, NULL);
		g_free (path);
	}
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


