#define __SP_UI_INTERFACE_C__

/*
 * Action based interface elements
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 2003-2008 Lauris Kaplinski
 *
 */

#include <gtk/gtk.h>

#include "action.h"
#include "shortcuts.h"
#include "icon.h"
#include "button.h"

#include "interface.h"

static void
sp_ui_menu_activate (GtkItem *item, unsigned int verb)
{
	SPAction *action;
	void *data;
	action = sp_action_lookup_by_verb (verb);
	if (!action) return;
	data = g_object_get_data (G_OBJECT(item), "userdata");
	sp_action_perform (action, data);
}

static void
sp_ui_menu_key_press (GtkMenuItem *item, GdkEventKey *event, void *data)
{
	if (event->state & (GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK)) {
		unsigned int shortcut, verb;
		shortcut = event->keyval;
		if (event->state & GDK_SHIFT_MASK) shortcut |= SP_SHORTCUT_SHIFT_MASK;
		if (event->state & GDK_CONTROL_MASK) shortcut |= SP_SHORTCUT_CONTROL_MASK;
		if (event->state & GDK_MOD1_MASK) shortcut |= SP_SHORTCUT_ALT_MASK;
		verb = (unsigned int) data;
		sp_shortcut_set_verb (shortcut, verb, TRUE);
	}
}


GtkWidget *
sp_ui_menu_item_new_from_verb (unsigned int verb, void *data)
{
	SPAction *action;
	GtkWidget *item;

	if (verb == SP_VERB_NONE) {
		item = gtk_separator_menu_item_new ();
	} else {
		action = sp_action_lookup_by_verb (verb);
		g_return_val_if_fail (action != NULL, NULL);
		if (action->shortcut) {
			unsigned char c[256];
			unsigned char *as, *cs, *ss;
			GtkWidget *hb, *l;
			as = (action->shortcut & SP_SHORTCUT_ALT_MASK) ? "Alt+" : "";
			cs = (action->shortcut & SP_SHORTCUT_CONTROL_MASK) ? "Ctrl+" : "";
			ss = (action->shortcut & SP_SHORTCUT_SHIFT_MASK) ? "Shift+" : "";
			g_snprintf (c, 256, "%s%s%s%s", as, cs, ss, gdk_keyval_name (action->shortcut & 0xffffff));
			hb = gtk_hbox_new (FALSE, 16);
			l = gtk_label_new (action->name);
			gtk_misc_set_alignment ((GtkMisc *) l, 0.0, 0.5);
			gtk_box_pack_start ((GtkBox *) hb, l, TRUE, TRUE, 0);
			l = gtk_label_new (c);
			gtk_misc_set_alignment ((GtkMisc *) l, 1.0, 0.5);
			gtk_box_pack_end ((GtkBox *) hb, l, FALSE, FALSE, 0);
			gtk_widget_show_all (hb);
			item = gtk_image_menu_item_new ();
			gtk_container_add ((GtkContainer *) item, hb);
		} else {
			item = gtk_image_menu_item_new_with_label (action->name);
		}
		if (action->image) {
			GtkWidget *icon;
			icon = sp_icon_new (action->image, 16);
			gtk_widget_show (icon);
			gtk_image_menu_item_set_image ((GtkImageMenuItem *) item, icon);
		}
		gtk_widget_set_events (item, GDK_KEY_PRESS_MASK);
		g_object_set_data (G_OBJECT(item), "userdata", data);
		g_signal_connect (G_OBJECT(item), "activate", G_CALLBACK(sp_ui_menu_activate), (void *) verb);
		g_signal_connect (G_OBJECT(item), "key_press_event", G_CALLBACK(sp_ui_menu_key_press), (void *) verb);
	}

	return item;
}

GtkWidget *
sp_ui_menu_item_new_append_from_verb (GtkMenuShell *shell, unsigned int verb, void *data)
{
	GtkWidget *item;

	g_return_val_if_fail (shell != NULL, NULL);
	g_return_val_if_fail (GTK_IS_MENU(shell), NULL);

	item = sp_ui_menu_item_new_from_verb (verb, data);

	gtk_widget_show (item);
	gtk_menu_shell_append (shell, item);

	return item;
}

GtkWidget *
sp_button_new_from_verb (unsigned int buttonsize, unsigned int buttontype, unsigned int verb, void *params, GtkTooltips *tooltips)
{
	SPAction *action;
	GtkWidget *button;
	action = sp_action_lookup_by_verb (verb);
	g_return_val_if_fail (action != NULL, NULL);
	button = sp_button_new (buttonsize, buttontype, action, params, tooltips);
	return button;
}

GtkWidget *
sp_toolbar_button_new_append_from_verb (GtkToolbar *toolbar, unsigned int buttontype, unsigned int verb, void *params)
{
	SPAction *action;
	GtkWidget *button;
	int size, width, height;
	if (verb == SP_VERB_NONE) {
		gtk_toolbar_append_space (toolbar);
		return NULL;
	}
	action = sp_action_lookup_by_verb (verb);
	g_return_val_if_fail (action != NULL, NULL);
	size = gtk_toolbar_get_icon_size (toolbar);
	gtk_icon_size_lookup (size, &width, &height);
	button = sp_button_new (MAX (width, height), buttontype, action, params, NULL);
	gtk_toolbar_append_widget (toolbar, button, action->tip, action->tip);
	return button;
}
