#ifndef __SP_UI_INTERFACE_H__
#define __SP_UI_INTERFACE_H__

/*
 * Action based interface elements
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 2003-2008 Lauris Kaplinski
 *
 */

#include <gtk/gtkmenushell.h>
#include <gtk/gtktoolbar.h>

#ifdef __cplusplus
extern "C" {
#endif

GtkWidget *sp_ui_menu_item_new_from_verb (unsigned int verb, void *data);
GtkWidget *sp_ui_menu_item_new_append_from_verb (GtkMenuShell *shell, unsigned int verb, void *data);

GtkWidget *sp_button_new_from_verb (unsigned int buttonsize, unsigned int buttontype, unsigned int verb, void *params, GtkTooltips *tooltips);
GtkWidget *sp_toolbar_button_new_append_from_verb (GtkToolbar *toolbar, unsigned int buttontype, unsigned int verb, void *params);

#ifdef __cplusplus
}
#endif

#endif
