#ifndef __SP_OBJECT_REPR_H__
#define __SP_OBJECT_REPR_H__

/*
 * Object type dictionary and build frontend
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 1999-2003 Lauris Kaplinski
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#include <glib-object.h>

#include <thera/thera-node.h>

#include "sodipodi.h"

/* SPObject *sp_object_new_from_node (SPDocument *doc, TheraNode *node); */

GType sp_node_type_lookup (TheraNode *node);
GType sp_object_type_lookup (const unsigned char *name);

/* Return TRUE on success */

unsigned int sp_object_type_register (const unsigned char *name, GType type);

#endif
