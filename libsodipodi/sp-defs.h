#ifndef __SP_DEFS_H__
#define __SP_DEFS_H__

/*
 * SVG <defs> implementation
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 2000-2009 authors
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#define SP_TYPE_DEFS            (sp_defs_get_type ())
#define SP_DEFS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), SP_TYPE_DEFS, SPDefs))
#define SP_IS_DEFS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SP_TYPE_DEFS))

#include "sp-object.h"

struct _SPDefs {
	SPObject object;
};

struct _SPDefsClass {
	SPObjectClass parent_class;
};

GType sp_defs_get_type (void);

#endif
