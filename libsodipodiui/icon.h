#ifndef __SP_ICON_H__
#define __SP_ICON_H__

/*
 * Generic icon widget
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 2003-2008 Lauris Kaplinski
 *
 */

typedef struct _SPIcon SPIcon;
typedef struct _SPIconClass SPIconClass;

#define SP_TYPE_ICON (sp_icon_get_type ())
#define SP_ICON(o) (GTK_CHECK_CAST ((o), SP_TYPE_ICON, SPIcon))
#define SP_IS_ICON(o) (GTK_CHECK_TYPE ((o), SP_TYPE_ICON))

#define SP_ICON_SIZE_BORDER 8
#define SP_ICON_SIZE_BUTTON 16
#define SP_ICON_SIZE_MENU 12
#define SP_ICON_SIZE_TITLEBAR 12
#define SP_ICON_SIZE_NOTEBOOK 20

#include <libnr/nr-types.h>
#include <gtk/gtkwidget.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _SPIcon {
	GtkWidget widget;
	unsigned int size;
	NRImage *image;
};

struct _SPIconClass {
	GtkWidgetClass parent_class;
};

GType sp_icon_get_type (void);

/* Size is the size in pixels */
GtkWidget *sp_icon_new (const unsigned char *name, unsigned int size);
GtkWidget *sp_icon_new_from_image (NRImage *image);
/* Pixels will NOT be copied */
GtkWidget *sp_icon_new_from_data (const unsigned char *name, unsigned int size, const unsigned char *px);

/* This is unrelated, but can as well be here */
/* Loader fetches a new reference to image */
void sp_icon_register_loader (const unsigned char *prefix, NRImage *(* loader) (const unsigned char *, unsigned int, void *), void *data);

NRImage *sp_icon_image_load (const unsigned char *name, unsigned int size);
/* This is the loader for widget implementations */
NRImage *sp_icon_image_load_gtk (GtkWidget *widget, const unsigned char *name, unsigned int size);

#ifdef __cplusplus
}
#endif

#endif
