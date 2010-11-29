#ifndef __SP_RADIAL_GRADIENT_H__
#define __SP_RADIAL_GRADIENT_H__

/*
 * SVG <radialGradient> implementation
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 1999-2010 Lauris Kaplinski
 * Copyright (C) 2000-2001 Ximian, Inc.
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#define SP_TYPE_RADIALGRADIENT (sp_radialgradient_get_type ())
#define SP_RADIALGRADIENT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), SP_TYPE_RADIALGRADIENT, SPRadialGradient))
#define SP_IS_RADIALGRADIENT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SP_TYPE_RADIALGRADIENT))

#include <libsodipodi/sp-gradient.h>

struct _SPRadialGradient {
	SPGradient gradient;

	SPSVGLength cx;
	SPSVGLength cy;
	SPSVGLength r;
	SPSVGLength fx;
	SPSVGLength fy;
};

struct _SPRadialGradientClass {
	SPGradientClass parent_class;
};

GType sp_radialgradient_get_type (void);

void sp_radialgradient_set_position (SPRadialGradient *rg, gdouble cx, gdouble cy, gdouble fx, gdouble fy, gdouble r);

/* Builds flattened node tree of gradient - i.e. no href */

TheraNode *sp_radialgradient_build_repr (SPRadialGradient *lg, TheraDocument *thedoc, gboolean vector);

#endif
