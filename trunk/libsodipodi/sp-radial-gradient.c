#define __SP_RADIAL_GRADIENT_C__

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

#define noSP_GRADIENT_VERBOSE

#include "attributes.h"
#include "macros.h"

#if 0
#include <string.h>

#include <libnr/nr-matrix.h>
#include <libnr/nr-gradient.h>

#include "macros.h"
#include "attributes.h"
#include "document.h"
#include "factory.h"
#endif

#include "sp-radial-gradient.h"

/* Has to be power of 2 */
#define NCOLORS NR_GRADIENT_VECTOR_LENGTH

#define SP_TYPE_RGRADIENT_PAINTER (sp_rgradient_painter_get_type ())
#define SP_RGRADIENT_PAINTER(o) (ARIKKEI_CHECK_INSTANCE_CAST ((o), SP_TYPE_RGRADIENT_PAINTER, SPRGradientPainter))
#define SP_IS_RGRADIENT_PAINTER(o) (ARIKKEI_CHECK_INSTANCE_TYPE ((o), SP_TYPE_RGRADIENT_PAINTER))

typedef struct _SPRGradientPainter SPRGradientPainter;
typedef struct _SPRGradientPainterClass SPRGradientPainterClass;

struct _SPRGradientPainter {
	SPArenaPainter painter;
};

struct _SPRGradientPainterClass {
	SPArenaPainterClass parent_class;
};

static void sp_rgradient_painter_class_init (SPRGradientPainterClass *klass);
static void sp_rgradient_painter_init (SPRGradientPainter *painter);
static void sp_rgradient_painter_finalize (ArikkeiObject *object);

static NRRenderer *sp_rgradient_painter_new_renderer (NRPaintServer *server, const NRMatrixF *transform, const NRRectF *paintbox);
static void sp_rgradient_painter_release_renderer (NRPaintServer *server, NRRenderer *renderer);

static void sp_radialgradient_setup_renderer (SPRadialGradient *lg, SPRGradientPainter *rgp, NRRGradientRenderer *rgr, const NRMatrixF *ctm, const NRRectF *bbox);

static SPArenaPainterClass *painter_parent_class;

unsigned int
sp_rgradient_painter_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		type = arikkei_object_register_type (SP_TYPE_ARENA_PAINTER,
						(const unsigned char *) "SPRGradientPainter",
						sizeof (SPRGradientPainterClass),
						sizeof (SPRGradientPainter),
						(void (*) (ArikkeiObjectClass *)) sp_rgradient_painter_class_init,
						(void (*) (ArikkeiObject *)) sp_rgradient_painter_init);
	}
	return type;
}

static void
sp_rgradient_painter_class_init (SPRGradientPainterClass *klass)
{
	ArikkeiObjectClass *object_class;
	NRPaintServerClass *pserver_class;

	object_class = (ArikkeiObjectClass *) klass;
	pserver_class = (NRPaintServerClass *) klass;

	painter_parent_class = (SPArenaPainterClass *) ((ArikkeiObjectClass *) klass)->parent;

	object_class->finalize = sp_rgradient_painter_finalize;
	pserver_class->new_renderer = sp_rgradient_painter_new_renderer;
	pserver_class->release_renderer = sp_rgradient_painter_release_renderer;
}

static void
sp_rgradient_painter_init (SPRGradientPainter *painter)
{
}

static void
sp_rgradient_painter_finalize (ArikkeiObject *object)
{
	if (((ArikkeiObjectClass *) painter_parent_class)->finalize)
		((ArikkeiObjectClass *) painter_parent_class)->finalize (object);
}

static NRRenderer *
sp_rgradient_painter_new_renderer (NRPaintServer *server, const NRMatrixF *transform, const NRRectF *paintbox)
{
	SPArenaPainter *painter;
	SPRadialGradient *rg;
	NRRGradientRenderer *rgr;

	painter = SP_ARENA_PAINTER(server);
	rg = SP_RADIALGRADIENT (painter->spserver);

	rgr = (NRRGradientRenderer *) malloc (sizeof (NRRGradientRenderer));

	sp_radialgradient_setup_renderer (rg, SP_RGRADIENT_PAINTER(painter), rgr, transform, paintbox);

	return (NRRenderer *) rgr;
}

static void
sp_rgradient_painter_release_renderer (NRPaintServer *server, NRRenderer *renderer)
{
}

static void sp_radialgradient_class_init (SPRadialGradientClass *klass);
static void sp_radialgradient_init (SPRadialGradient *rg);

static void sp_radialgradient_build (SPObject *object, SPDocument *document, TheraNode *node);
static void sp_radialgradient_set (SPObject *object, unsigned int key, const unsigned char *value);
static TheraNode *sp_radialgradient_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags);

static SPArenaPainter *sp_radialgradient_show (SPPaintServer *ps);

static SPGradientClass *rg_parent_class;

GType
sp_radialgradient_get_type (void)
{
	static GType type = 0;
	if (!type) {
		GTypeInfo info = {
			sizeof (SPRadialGradientClass),
			NULL, NULL,
			(GClassInitFunc) sp_radialgradient_class_init,
			NULL, NULL,
			sizeof (SPRadialGradient),
			16,
			(GInstanceInitFunc) sp_radialgradient_init,
		};
		type = g_type_register_static (SP_TYPE_GRADIENT, "SPRadialGradient", &info, 0);
	}
	return type;
}

static void
sp_radialgradient_class_init (SPRadialGradientClass * klass)
{
	GObjectClass *gobject_class;
	SPObjectClass *sp_object_class;
	SPPaintServerClass *ps_class;
	SPGradientClass *gr_class;

	gobject_class = (GObjectClass *) klass;
	sp_object_class = (SPObjectClass *) klass;
	ps_class = (SPPaintServerClass *) klass;
	gr_class = (SPGradientClass *) klass;

	rg_parent_class = g_type_class_ref (SP_TYPE_GRADIENT);

	sp_object_class->build = sp_radialgradient_build;
	sp_object_class->set = sp_radialgradient_set;
	sp_object_class->write = sp_radialgradient_write;

	ps_class->show = sp_radialgradient_show;
}

static void
sp_radialgradient_init (SPRadialGradient *rg)
{
	sp_svg_length_unset (&rg->cx, SP_SVG_UNIT_PERCENT, 0.5, 0.5);
	sp_svg_length_unset (&rg->cy, SP_SVG_UNIT_PERCENT, 0.5, 0.5);
	sp_svg_length_unset (&rg->r, SP_SVG_UNIT_PERCENT, 0.5, 0.5);
	sp_svg_length_unset (&rg->fx, SP_SVG_UNIT_PERCENT, 0.5, 0.5);
	sp_svg_length_unset (&rg->fy, SP_SVG_UNIT_PERCENT, 0.5, 0.5);
}

static void
sp_radialgradient_build (SPObject *object, SPDocument *document, TheraNode *node)
{
	SPRadialGradient *rg;

	rg = SP_RADIALGRADIENT (object);

	if (((SPObjectClass *) rg_parent_class)->build)
		(* ((SPObjectClass *) rg_parent_class)->build) (object, document, node);

	sp_object_read_attr (object, "cx");
	sp_object_read_attr (object, "cy");
	sp_object_read_attr (object, "r");
	sp_object_read_attr (object, "fx");
	sp_object_read_attr (object, "fy");
}

static void
sp_radialgradient_set (SPObject *object, unsigned int key, const unsigned char *value)
{
	SPRadialGradient *rg;

	rg = SP_RADIALGRADIENT (object);

	switch (key) {
	case SP_ATTR_CX:
		if (!sp_svg_length_read (value, &rg->cx)) {
			sp_svg_length_unset (&rg->cx, SP_SVG_UNIT_PERCENT, 0.5, 0.5);
		}
		if (!rg->fx.set) {
			rg->fx.value = rg->cx.value;
			rg->fx.computed = rg->cx.computed;
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_CY:
		if (!sp_svg_length_read (value, &rg->cy)) {
			sp_svg_length_unset (&rg->cy, SP_SVG_UNIT_PERCENT, 0.5, 0.5);
		}
		if (!rg->fy.set) {
			rg->fy.value = rg->cy.value;
			rg->fy.computed = rg->cy.computed;
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_R:
		if (!sp_svg_length_read (value, &rg->r)) {
			sp_svg_length_unset (&rg->r, SP_SVG_UNIT_PERCENT, 0.5, 0.5);
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_FX:
		if (!sp_svg_length_read (value, &rg->fx)) {
			sp_svg_length_unset (&rg->fx, rg->cx.unit, rg->cx.value, rg->cx.computed);
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_FY:
		if (!sp_svg_length_read (value, &rg->fy)) {
			sp_svg_length_unset (&rg->fy, rg->cy.unit, rg->cy.value, rg->cy.computed);
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	default:
		if (((SPObjectClass *) rg_parent_class)->set)
			((SPObjectClass *) rg_parent_class)->set (object, key, value);
		break;
	}
}

static TheraNode *
sp_radialgradient_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags)
{
	SPRadialGradient *rg;

	rg = SP_RADIALGRADIENT (object);

	if ((flags & SP_OBJECT_WRITE_BUILD) && !node) {
		node = thera_document_new_node (thedoc, THERA_NODE_ELEMENT, "radialGradient");
	}

	if ((flags & SP_OBJECT_WRITE_ALL) || rg->cx.set) thera_node_set_double (node, "cx", rg->cx.computed);
	if ((flags & SP_OBJECT_WRITE_ALL) || rg->cy.set) thera_node_set_double (node, "cy", rg->cy.computed);
	if ((flags & SP_OBJECT_WRITE_ALL) || rg->r.set) thera_node_set_double (node, "r", rg->r.computed);
	if ((flags & SP_OBJECT_WRITE_ALL) || rg->fx.set) thera_node_set_double (node, "fx", rg->fx.computed);
	if ((flags & SP_OBJECT_WRITE_ALL) || rg->fy.set) thera_node_set_double (node, "fy", rg->fy.computed);

	if (((SPObjectClass *) rg_parent_class)->write)
		(* ((SPObjectClass *) rg_parent_class)->write) (object, thedoc, node, flags);

	return node;
}

static SPArenaPainter *
sp_radialgradient_show (SPPaintServer *ps)
{
	SPRGradientPainter *rgp;

	rgp = (SPRGradientPainter *) arikkei_object_new (SP_TYPE_RGRADIENT_PAINTER);

	return SP_ARENA_PAINTER(rgp);
}


void
sp_radialgradient_set_position (SPRadialGradient *rg, gdouble cx, gdouble cy, gdouble fx, gdouble fy, gdouble r)
{
	g_return_if_fail (rg != NULL);
	g_return_if_fail (SP_IS_RADIALGRADIENT (rg));

	/* fixme: units? (Lauris)  */
	sp_svg_length_set (&rg->cx, SP_SVG_UNIT_NONE, cx, cx);
	sp_svg_length_set (&rg->cy, SP_SVG_UNIT_NONE, cy, cy);
	sp_svg_length_set (&rg->fx, SP_SVG_UNIT_NONE, fx, fx);
	sp_svg_length_set (&rg->fy, SP_SVG_UNIT_NONE, fy, fy);
	sp_svg_length_set (&rg->r, SP_SVG_UNIT_NONE, r, r);

	sp_object_request_modified (SP_OBJECT (rg), SP_OBJECT_MODIFIED_FLAG);
}


/* Builds flattened node tree of gradient - i.e. no href */

TheraNode *
sp_radialgradient_build_repr (SPRadialGradient *rg, TheraDocument *thedoc, gboolean vector)
{
	TheraNode *node;

	g_return_val_if_fail (rg != NULL, NULL);
	g_return_val_if_fail (SP_IS_RADIALGRADIENT (rg), NULL);

	node = node = thera_document_new_node (thedoc, THERA_NODE_ELEMENT, "radialGradient");

	sp_object_invoke_write ((SPObject *) rg, thedoc, node, SP_OBJECT_WRITE_SODIPODI | SP_OBJECT_WRITE_ALL);

	if (vector) {
		sp_gradient_ensure_vector ((SPGradient *) rg);
		sp_gradient_repr_set_vector ((SPGradient *) rg, node, ((SPGradient *) rg)->vector);
	}

	return node;
}

static void
sp_radialgradient_setup_renderer (SPRadialGradient *rg, SPRGradientPainter *rgp, NRRGradientRenderer *rgr, const NRMatrixF *ctm, const NRRectF *bbox)
{
	SPGradient *gr;
	NRMatrixF gs2px;

	gr = SP_GRADIENT(rg);

	if (!gr->color) sp_gradient_ensure_colors (gr);

	/* fixme: We may try to normalize here too, look at linearGradient (Lauris) */

	if (gr->units == SP_GRADIENT_UNITS_OBJECTBOUNDINGBOX) {
		NRMatrixF bbox2user;
		NRMatrixF gs2user;

		/* fixme: We may try to normalize here too, look at linearGradient (Lauris) */

		/* gradientTransform goes here (Lauris) */

		/* BBox to user coordinate system */
		bbox2user.c[0] = bbox->x1 - bbox->x0;
		bbox2user.c[1] = 0.0;
		bbox2user.c[2] = 0.0;
		bbox2user.c[3] = bbox->y1 - bbox->y0;
		bbox2user.c[4] = bbox->x0;
		bbox2user.c[5] = bbox->y0;

		/* fixme: (Lauris) */
		nr_matrix_multiply_fff (&gs2user, &gr->transform, &bbox2user);
		nr_matrix_multiply_fff (&gs2px, &gs2user, ctm);
	} else {
		/* Problem: What to do, if we have mixed lengths and percentages? */
		/* Currently we do ignore percentages at all, but that is not good (lauris) */

		/* fixme: We may try to normalize here too, look at linearGradient (Lauris) */

		/* fixme: (Lauris) */
		nr_matrix_multiply_fff (&gs2px, &gr->transform, ctm);
	}

	nr_rgradient_renderer_setup (rgr, NR_PAINT_SERVER(rgp), gr->color, gr->spread,
				     &gs2px,
				     rg->cx.computed, rg->cy.computed,
				     rg->fx.computed, rg->fy.computed,
				     rg->r.computed);
}

