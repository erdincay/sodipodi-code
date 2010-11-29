#ifndef __SP_LINEAR_GRADIENT_H__
#define __SP_LINEAR_GRADIENT_H__

/*
 * SVG <linearGradient> implementation
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 1999-2010 Lauris Kaplinski
 * Copyright (C) 2000-2001 Ximian, Inc.
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#define SP_TYPE_LINEARGRADIENT (sp_lineargradient_get_type ())
#define SP_LINEARGRADIENT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), SP_TYPE_LINEARGRADIENT, SPLinearGradient))
#define SP_IS_LINEARGRADIENT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SP_TYPE_LINEARGRADIENT))

#include <libsodipodi/sp-gradient.h>

struct _SPLinearGradient {
	SPGradient gradient;

	SPSVGLength x1;
	SPSVGLength y1;
	SPSVGLength x2;
	SPSVGLength y2;
};

struct _SPLinearGradientClass {
	SPGradientClass parent_class;
};

GType sp_lineargradient_get_type (void);

void sp_lineargradient_set_position (SPLinearGradient *lg, gdouble x1, gdouble y1, gdouble x2, gdouble y2);

/* Builds flattened node tree of gradient - i.e. no href */

TheraNode *sp_lineargradient_build_repr (SPLinearGradient *lg, TheraDocument *thedoc, gboolean vector);

#endif
