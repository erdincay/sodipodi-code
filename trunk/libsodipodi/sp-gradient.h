#ifndef __SP_GRADIENT_H__
#define __SP_GRADIENT_H__

/*
 * SVG <stop> and gradient superclass implementation
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 1999-2002 Lauris Kaplinski
 * Copyright (C) 2000-2001 Ximian, Inc.
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#define SP_TYPE_STOP (sp_stop_get_type ())
#define SP_STOP(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), SP_TYPE_STOP, SPStop))
#define SP_IS_STOP(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), SP_TYPE_STOP))

#define SP_TYPE_GRADIENT (sp_gradient_get_type ())
#define SP_GRADIENT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), SP_TYPE_GRADIENT, SPGradient))
#define SP_IS_GRADIENT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SP_TYPE_GRADIENT))

#include <libnr/nr-matrix.h>
#include <libnr/nr-gradient.h>

#include <libsodipodiui/color.h>

#include <libsodipodi/svg/svg.h>
#include <libsodipodi/sp-paint-server.h>

/*
 * Gradient Stop
 */

struct _SPStop {
	SPObject object;

	/* fixme: Should be SPSVGPercentage */
	gfloat offset;

	SPColor color;
	/* fixme: Implement SPSVGNumber or something similar */
	gfloat opacity;
};

struct _SPStopClass {
	SPObjectClass parent_class;
};

GType sp_stop_get_type (void);

/*
 * Gradient
 *
 * Implement spread, stops list
 * fixme: Implement more here (Lauris)
 */

typedef struct _SPGradientStop SPGradientStop;
typedef struct _SPGradientVector SPGradientVector;

struct _SPGradientStop {
	gdouble offset;
	SPColor color;
	gfloat opacity;
};

struct _SPGradientVector {
	gint nstops;
	gdouble start, end;
	SPGradientStop stops[1];
};

typedef enum {
	SP_GRADIENT_STATE_UNKNOWN,
	SP_GRADIENT_STATE_VECTOR,
	SP_GRADIENT_STATE_PRIVATE
} SPGradientState;

typedef enum {
	SP_GRADIENT_UNITS_OBJECTBOUNDINGBOX,
	SP_GRADIENT_UNITS_USERSPACEONUSE
} SPGradientUnits;

#define SP_GRADIENT_STATE_IS_SET(g) (SP_GRADIENT(g)->state != SP_GRADIENT_STATE_UNKNOWN)
#define SP_GRADIENT_IS_VECTOR(g) (SP_GRADIENT(g)->state == SP_GRADIENT_STATE_VECTOR)
#define SP_GRADIENT_IS_PRIVATE(g) (SP_GRADIENT(g)->state == SP_GRADIENT_STATE_PRIVATE)
#define SP_GRADIENT_HAS_STOPS(g) (SP_GRADIENT(g)->has_stops)
#define SP_GRADIENT_SPREAD(g) (SP_GRADIENT (g)->spread)
#define SP_GRADIENT_UNITS(g) (SP_GRADIENT (g)->units)

struct _SPGradient {
	SPPaintServer paint_server;
	/* Reference (href) */
	SPGradient *href;
	/* State in Sodipodi gradient system */
	guint state : 2;
	/* gradientUnits attribute */
	SPGradientUnits units;
	guint units_set : 1;
	/* gradientTransform attribute */
	NRMatrixF transform;
	unsigned int transform_set : 1;
	/* spreadMethod attribute */
	unsigned int spread : 4;
	unsigned int spread_set : 1;
	/* Gradient stops */
	guint has_stops : 1;
	/* Composed vector */
	SPGradientVector *vector;
	/* Rendered color array (4 * 1024 bytes at moment) */
	guchar *color;
	/* Length of vector */
	gdouble len;
};

struct _SPGradientClass {
	SPPaintServerClass parent_class;
};

GType sp_gradient_get_type (void);

/* Forces vector to be built, if not present (i.e. changed) */
void sp_gradient_ensure_vector (SPGradient *gradient);
/* Ensures that color array is populated */
void sp_gradient_ensure_colors (SPGradient *gradient);
/* Sets gradient vector to given value, does not update reprs */
void sp_gradient_set_vector (SPGradient *gradient, SPGradientVector *vector);

void sp_gradient_set_units (SPGradient *gr, unsigned int units);
void sp_gradient_set_spread (SPGradient *gr, unsigned int spread);

/* Gradient node methods */
void sp_gradient_repr_set_vector (SPGradient *gradient, TheraNode *node, SPGradientVector *vector);

/*
 * Renders gradient vector to buffer
 *
 * len, width, height, rowstride - buffer parameters (1 or 2 dimensional)
 * span - full integer width of requested gradient
 * pos - buffer starting position in span
 *
 * RGB buffer background should be set up before
 */
void sp_gradient_render_vector_line_rgba (SPGradient *gr, guchar *px, gint len, gint pos, gint span);
void sp_gradient_render_vector_line_rgb (SPGradient *gr, guchar *px, gint len, gint pos, gint span);
void sp_gradient_render_vector_block_rgba (SPGradient *gr, guchar *px, gint w, gint h, gint rs, gint pos, gint span, gboolean horizontal);
void sp_gradient_render_vector_block_rgb (SPGradient *gr, guchar *px, gint w, gint h, gint rs, gint pos, gint span, gboolean horizontal);

/* Transforms to/from gradient position space in given environment */
NRMatrixF *sp_gradient_get_g2d_matrix_f (SPGradient *gr, NRMatrixF *ctm, NRRectF *bbox, NRMatrixF *g2d);
NRMatrixF *sp_gradient_get_gs2d_matrix_f (SPGradient *gr, NRMatrixF *ctm, NRRectF *bbox, NRMatrixF *gs2d);
void sp_gradient_set_gs2d_matrix_f (SPGradient *gr, NRMatrixF *ctm, NRRectF *bbox, NRMatrixF *gs2d);
void sp_gradient_from_position_xy (SPGradient *gr, NRMatrixF *ctm, NRRectF *bbox, NRPointF *p, float x, float y);
void sp_gradient_to_position_xy (SPGradient *gr, NRMatrixF *ctm, NRRectF *bbox, NRPointF *p, float x, float y);

#endif
