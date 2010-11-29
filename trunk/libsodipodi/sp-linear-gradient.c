#define __SP_LINEAR_GRADIENT_C__

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

#define noSP_GRADIENT_VERBOSE

#include "attributes.h"
#include "macros.h"

#if 0
#include <string.h>

#include <libnr/nr-gradient.h>

#include "document.h"
#include "factory.h"
#endif

#include "sp-linear-gradient.h"

/* Has to be power of 2 */
#define NCOLORS NR_GRADIENT_VECTOR_LENGTH

#define SP_TYPE_LGRADIENT_PAINTER (sp_lgradient_painter_get_type ())
#define SP_LGRADIENT_PAINTER(o) (ARIKKEI_CHECK_INSTANCE_CAST ((o), SP_TYPE_LGRADIENT_PAINTER, SPLGradientPainter))
#define SP_IS_LGRADIENT_PAINTER(o) (ARIKKEI_CHECK_INSTANCE_TYPE ((o), SP_TYPE_LGRADIENT_PAINTER))

typedef struct _SPLGradientPainter SPLGradientPainter;
typedef struct _SPLGradientPainterClass SPLGradientPainterClass;

struct _SPLGradientPainter {
	SPArenaPainter painter;
};

struct _SPLGradientPainterClass {
	SPArenaPainterClass parent_class;
};

static void sp_lgradient_painter_class_init (SPLGradientPainterClass *klass);
static void sp_lgradient_painter_init (SPLGradientPainter *painter);
static void sp_lgradient_painter_finalize (ArikkeiObject *object);

static NRRenderer *sp_lgradient_painter_new_renderer (NRPaintServer *server, const NRMatrixF *transform, const NRRectF *paintbox);
static void sp_lgradient_painter_release_renderer (NRPaintServer *server, NRRenderer *renderer);

static void sp_lineargradient_setup_renderer (SPLinearGradient *lg, SPLGradientPainter *lgp, NRLGradientRenderer *lgr, const NRMatrixF *ctm, const NRRectF *bbox);

static SPArenaPainterClass *painter_parent_class;

unsigned int
sp_lgradient_painter_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		type = arikkei_object_register_type (SP_TYPE_ARENA_PAINTER,
						(const unsigned char *) "SPLGradientPainter",
						sizeof (SPLGradientPainterClass),
						sizeof (SPLGradientPainter),
						(void (*) (ArikkeiObjectClass *)) sp_lgradient_painter_class_init,
						(void (*) (ArikkeiObject *)) sp_lgradient_painter_init);
	}
	return type;
}

static void
sp_lgradient_painter_class_init (SPLGradientPainterClass *klass)
{
	ArikkeiObjectClass *object_class;
	NRPaintServerClass *pserver_class;

	object_class = (ArikkeiObjectClass *) klass;
	pserver_class = (NRPaintServerClass *) klass;

	painter_parent_class = (SPArenaPainterClass *) ((ArikkeiObjectClass *) klass)->parent;

	object_class->finalize = sp_lgradient_painter_finalize;
	pserver_class->new_renderer = sp_lgradient_painter_new_renderer;
	pserver_class->release_renderer = sp_lgradient_painter_release_renderer;
}

static void
sp_lgradient_painter_init (SPLGradientPainter *painter)
{
}

static void
sp_lgradient_painter_finalize (ArikkeiObject *object)
{
	if (((ArikkeiObjectClass *) painter_parent_class)->finalize)
		((ArikkeiObjectClass *) painter_parent_class)->finalize (object);
}

static NRRenderer *
sp_lgradient_painter_new_renderer (NRPaintServer *server, const NRMatrixF *transform, const NRRectF *paintbox)
{
	SPArenaPainter *painter;
	SPLinearGradient *lg;
	NRLGradientRenderer *lgr;

	painter = SP_ARENA_PAINTER(server);
	lg = SP_LINEARGRADIENT (painter->spserver);

	lgr = (NRLGradientRenderer *) malloc (sizeof (NRLGradientRenderer));

	sp_lineargradient_setup_renderer (lg, SP_LGRADIENT_PAINTER(painter), lgr, transform, paintbox);

	return (NRRenderer *) lgr;
}

static void
sp_lgradient_painter_release_renderer (NRPaintServer *server, NRRenderer *renderer)
{
}

#if 0
static void
sp_lgradient_painter_render (NRRenderer *renderer, NRPixBlock *dest, NRPixBlock *mask)
{
	SPPainter *painter;
	SPLGradientPainter *lgpainter;

	painter = SP_PAINTER(renderer);
	lgpainter = SP_LGRADIENT_PAINTER(renderer);

	if (painter->server) {
		nr_lgradient_renderer_render (&lgpainter->renderer, dest, mask);
	} else {
		if (((NRRendererClass *) painter_parent_class)->render)
			((NRRendererClass *) painter_parent_class)->render (renderer, dest, mask);
	}
}
#endif

static void sp_lineargradient_class_init (SPLinearGradientClass * klass);
static void sp_lineargradient_init (SPLinearGradient * lg);

static void sp_lineargradient_build (SPObject *object, SPDocument * document, TheraNode * node);
static void sp_lineargradient_set (SPObject *object, unsigned int key, const unsigned char *value);
static TheraNode *sp_lineargradient_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags);

static SPArenaPainter *sp_lineargradient_show (SPPaintServer *ps);

static SPGradientClass *lg_parent_class;

GType
sp_lineargradient_get_type (void)
{
	static GType type = 0;
	if (!type) {
		GTypeInfo info = {
			sizeof (SPLinearGradientClass),
			NULL, NULL,
			(GClassInitFunc) sp_lineargradient_class_init,
			NULL, NULL,
			sizeof (SPLinearGradient),
			16,
			(GInstanceInitFunc) sp_lineargradient_init,
		};
		type = g_type_register_static (SP_TYPE_GRADIENT, "SPLinearGradient", &info, 0);
	}
	return type;
}

static void
sp_lineargradient_class_init (SPLinearGradientClass * klass)
{
	GObjectClass *gobject_class;
	SPObjectClass *sp_object_class;
	SPPaintServerClass *ps_class;
	SPGradientClass *gr_class;

	gobject_class = (GObjectClass *) klass;
	sp_object_class = (SPObjectClass *) klass;
	ps_class = (SPPaintServerClass *) klass;
	gr_class = (SPGradientClass *) klass;

	lg_parent_class = g_type_class_ref (SP_TYPE_GRADIENT);

	sp_object_class->build = sp_lineargradient_build;
	sp_object_class->set = sp_lineargradient_set;
	sp_object_class->write = sp_lineargradient_write;

	ps_class->show = sp_lineargradient_show;
}

static void
sp_lineargradient_init (SPLinearGradient * lg)
{
	sp_svg_length_unset (&lg->x1, SP_SVG_UNIT_PERCENT, 0.0, 0.0);
	sp_svg_length_unset (&lg->y1, SP_SVG_UNIT_PERCENT, 0.0, 0.0);
	sp_svg_length_unset (&lg->x2, SP_SVG_UNIT_PERCENT, 1.0, 1.0);
	sp_svg_length_unset (&lg->y2, SP_SVG_UNIT_PERCENT, 0.0, 0.0);
}

static void
sp_lineargradient_build (SPObject * object, SPDocument * document, TheraNode * node)
{
	SPLinearGradient * lg;

	lg = SP_LINEARGRADIENT (object);

	if (((SPObjectClass *) lg_parent_class)->build)
		(* ((SPObjectClass *) lg_parent_class)->build) (object, document, node);

	sp_object_read_attr (object, "x1");
	sp_object_read_attr (object, "y1");
	sp_object_read_attr (object, "x2");
	sp_object_read_attr (object, "y2");
}

static void
sp_lineargradient_set (SPObject *object, unsigned int key, const unsigned char *value)
{
	SPLinearGradient * lg;

	lg = SP_LINEARGRADIENT (object);

	switch (key) {
	case SP_ATTR_X1:
		if (!sp_svg_length_read (value, &lg->x1)) {
			sp_svg_length_unset (&lg->x1, SP_SVG_UNIT_PERCENT, 0.0, 0.0);
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_Y1:
		if (!sp_svg_length_read (value, &lg->y1)) {
			sp_svg_length_unset (&lg->y1, SP_SVG_UNIT_PERCENT, 0.0, 0.0);
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_X2:
		if (!sp_svg_length_read (value, &lg->x2)) {
			sp_svg_length_unset (&lg->x2, SP_SVG_UNIT_PERCENT, 1.0, 1.0);
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_Y2:
		if (!sp_svg_length_read (value, &lg->y2)) {
			sp_svg_length_unset (&lg->y2, SP_SVG_UNIT_PERCENT, 0.0, 0.0);
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	default:
		if (((SPObjectClass *) lg_parent_class)->set)
			(* ((SPObjectClass *) lg_parent_class)->set) (object, key, value);
		break;
	}
}

static TheraNode *
sp_lineargradient_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags)
{
	SPLinearGradient * lg;

	lg = SP_LINEARGRADIENT (object);

	if ((flags & SP_OBJECT_WRITE_BUILD) && !node) {
		node = thera_document_new_node (thedoc, THERA_NODE_ELEMENT, "linearGradient");
	}

	if ((flags & SP_OBJECT_WRITE_ALL) || lg->x1.set) thera_node_set_double (node, "x1", lg->x1.computed);
	if ((flags & SP_OBJECT_WRITE_ALL) || lg->y1.set) thera_node_set_double (node, "y1", lg->y1.computed);
	if ((flags & SP_OBJECT_WRITE_ALL) || lg->x2.set) thera_node_set_double (node, "x2", lg->x2.computed);
	if ((flags & SP_OBJECT_WRITE_ALL) || lg->y2.set) thera_node_set_double (node, "y2", lg->y2.computed);

	if (((SPObjectClass *) lg_parent_class)->write)
		(* ((SPObjectClass *) lg_parent_class)->write) (object, thedoc, node, flags);

	return node;
}

static SPArenaPainter *
sp_lineargradient_show (SPPaintServer *ps)
{
	SPLGradientPainter *lgp;

	lgp = (SPLGradientPainter *) arikkei_object_new (SP_TYPE_LGRADIENT_PAINTER);

	return SP_ARENA_PAINTER(lgp);
}

void
sp_lineargradient_set_position (SPLinearGradient *lg, gdouble x1, gdouble y1, gdouble x2, gdouble y2)
{
	g_return_if_fail (lg != NULL);
	g_return_if_fail (SP_IS_LINEARGRADIENT (lg));

	/* fixme: units? (Lauris)  */
	sp_svg_length_set (&lg->x1, SP_SVG_UNIT_NONE, x1, x1);
	sp_svg_length_set (&lg->y1, SP_SVG_UNIT_NONE, y1, y1);
	sp_svg_length_set (&lg->x2, SP_SVG_UNIT_NONE, x2, x2);
	sp_svg_length_set (&lg->y2, SP_SVG_UNIT_NONE, y2, y2);

	sp_object_request_modified (SP_OBJECT (lg), SP_OBJECT_MODIFIED_FLAG);
}

/* Builds flattened node tree of gradient - i.e. no href */

TheraNode *
sp_lineargradient_build_repr (SPLinearGradient *lg, TheraDocument *thedoc, gboolean vector)
{
	TheraNode *node;

	g_return_val_if_fail (lg != NULL, NULL);
	g_return_val_if_fail (SP_IS_LINEARGRADIENT (lg), NULL);

	node = thera_document_new_node (thedoc, THERA_NODE_ELEMENT, "linearGradient");

	sp_object_invoke_write ((SPObject *) lg, thedoc, node, SP_OBJECT_WRITE_SODIPODI | SP_OBJECT_WRITE_ALL);

	if (vector) {
		sp_gradient_ensure_vector ((SPGradient *) lg);
		sp_gradient_repr_set_vector ((SPGradient *) lg, node, ((SPGradient *) lg)->vector);
	}

	return node;
}

/*
 * Basically we have to deal with transformations
 *
 * 1) color2norm - maps point in (0,NCOLORS) vector to (0,1) vector
 *    fixme: I do not know, how to deal with start > 0 and end < 1
 * 2) norm2pos - maps (0,1) vector to x1,y1 - x2,y2
 * 2) gradientTransform
 * 3) bbox2user
 * 4) ctm == userspace to pixel grid
 */

static void
sp_lineargradient_setup_renderer (SPLinearGradient *lg, SPLGradientPainter *lgp, NRLGradientRenderer *lgr, const NRMatrixF *ctm, const NRRectF *bbox)
{
	SPGradient *gr;
	NRMatrixF color2norm, color2px;
	NRMatrixF v2px;

	gr = SP_GRADIENT(lg);

	if (!gr->color) sp_gradient_ensure_colors (gr);

#if 0
	lgp->painter.type = SP_PAINTER_IND;
	lgp->painter.fill = sp_lg_fill;

	lgp->lg = lg;
#endif

	/* fixme: Technically speaking, we map NCOLORS on line [start,end] onto line [0,1] (Lauris) */
	/* fixme: I almost think, we should fill color array start and end in that case (Lauris) */
	/* fixme: The alternative would be to leave these just empty garbage or something similar (Lauris) */
	/* fixme: Originally I had 1023.9999 here - not sure, whether we have really to cut out ceil int (Lauris) */
#if 0
	art_affine_scale (color2norm, gr->len / (gdouble) NCOLORS, gr->len / (gdouble) NCOLORS);
	SP_PRINT_TRANSFORM ("color2norm", color2norm);
	/* Now we have normalized vector */
#else
	nr_matrix_f_set_identity (&color2norm);
#endif

	if (gr->units == SP_GRADIENT_UNITS_OBJECTBOUNDINGBOX) {
		NRMatrixF norm2pos, bbox2user;
		NRMatrixF color2pos, color2tpos, color2user;

#if 0
		/* This is easy case, as we can just ignore percenting here */
		/* fixme: Still somewhat tricky, but I think I got it correct (lauris) */
		norm2pos[0] = lg->x2.computed - lg->x1.computed;
		norm2pos[1] = lg->y2.computed - lg->y1.computed;
		norm2pos[2] = lg->y2.computed - lg->y1.computed;
		norm2pos[3] = lg->x1.computed - lg->x2.computed;
		norm2pos[4] = lg->x1.computed;
		norm2pos[5] = lg->y1.computed;
		SP_PRINT_TRANSFORM ("norm2pos", norm2pos);
#else
		nr_matrix_f_set_identity (&norm2pos);
#endif

		/* gradientTransform goes here (Lauris) */
		SP_PRINT_TRANSFORM ("gradientTransform", gr->transform.c);

		/* BBox to user coordinate system */
		bbox2user.c[0] = bbox->x1 - bbox->x0;
		bbox2user.c[1] = 0.0;
		bbox2user.c[2] = 0.0;
		bbox2user.c[3] = bbox->y1 - bbox->y0;
		bbox2user.c[4] = bbox->x0;
		bbox2user.c[5] = bbox->y0;
		SP_PRINT_TRANSFORM ("bbox2user", bbox2user.c);

		/* CTM goes here */
		SP_PRINT_TRANSFORM ("ctm", ctm);

		nr_matrix_multiply_fff (&color2pos, &color2norm, &norm2pos);
		SP_PRINT_TRANSFORM ("color2pos", color2pos.c);
		nr_matrix_multiply_fff (&color2tpos, &color2pos, &gr->transform);
		SP_PRINT_TRANSFORM ("color2tpos", color2tpos.c);
		nr_matrix_multiply_fff (&color2user, &color2tpos, &bbox2user);
		SP_PRINT_TRANSFORM ("color2user", color2user.c);
		nr_matrix_multiply_fff (&color2px, &color2user, ctm);
		SP_PRINT_TRANSFORM ("color2px", color2px.c);
	} else {
		NRMatrixF norm2pos;
		NRMatrixF color2pos, color2tpos;
		/* Problem: What to do, if we have mixed lengths and percentages? */
		/* Currently we do ignore percentages at all, but that is not good (lauris) */

#if 0
		/* fixme: Do percentages (Lauris) */
		norm2pos[0] = lg->x2.computed - lg->x1.computed;
		norm2pos[1] = lg->y2.computed - lg->y1.computed;
		norm2pos[2] = lg->y2.computed - lg->y1.computed;
		norm2pos[3] = lg->x1.computed - lg->x2.computed;
		norm2pos[4] = lg->x1.computed;
		norm2pos[5] = lg->y1.computed;
		SP_PRINT_TRANSFORM ("norm2pos", norm2pos);
#else
		nr_matrix_f_set_identity (&norm2pos);
#endif

		/* gradientTransform goes here (Lauris) */
		SP_PRINT_TRANSFORM ("gradientTransform", gr->transform.c);

		/* CTM goes here */
		SP_PRINT_TRANSFORM ("ctm", ctm->c);

		nr_matrix_multiply_fff (&color2pos, &color2norm, &norm2pos);
		SP_PRINT_TRANSFORM ("color2pos", color2pos.c);
		nr_matrix_multiply_fff (&color2tpos, &color2pos, &gr->transform);
		SP_PRINT_TRANSFORM ("color2tpos", color2tpos.c);
		nr_matrix_multiply_fff (&color2px, &color2tpos, ctm);
		SP_PRINT_TRANSFORM ("color2px", color2px.c);
	}

	v2px = color2px;

	nr_lgradient_renderer_setup (lgr, NR_PAINT_SERVER(lgp), gr->color, gr->spread, &v2px,
				     lg->x1.computed, lg->y1.computed, lg->x2.computed, lg->y2.computed);
}

