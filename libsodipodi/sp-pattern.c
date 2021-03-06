#define __SP_PATTERN_C__

/*
 * SVG <pattern> implementation
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 2002 Lauris Kaplinski
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#include <string.h>

#include <libarikkei/arikkei-strlib.h>

#include <libnr/nr-rect.h>
#include <libnr/nr-matrix.h>

#include <libnrarena/nr-arena-group.h>

#include "macros.h"
#include "attributes.h"
#include "document.h"
#include "factory.h"
#include "svg/svg.h"

#include "sp-item.h"

#include "sp-pattern.h"

#define SP_TYPE_PATTERN_PAINTER (sp_pattern_painter_get_type ())
#define SP_PATTERN_PAINTER(o) (ARIKKEI_CHECK_INSTANCE_CAST ((o), SP_TYPE_PATTERN_PAINTER, SPPatternPainter))
#define SP_IS_PATTERN_PAINTER(o) (ARIKKEI_CHECK_INSTANCE_TYPE ((o), SP_TYPE_PATTERN_PAINTER))

typedef struct _SPPatternRenderer SPPatternRenderer;
typedef struct _SPPatternPainter SPPatternPainter;
typedef struct _SPPatternPainterClass SPPatternPainterClass;

struct _SPPatternRenderer {
	NRMatrixF ps2px;
	NRMatrixF px2ps;
	NRMatrixF pcs2px;

	NRArena *arena;
	unsigned int dkey;
	NRArenaItem *root;
};

struct _SPPatternPainter {
	SPArenaPainter painter;

	SPPattern *pat;
};

struct _SPPatternPainterClass {
	SPArenaPainterClass parent_class;
};

static void sp_pattern_painter_class_init (SPPatternPainterClass *klass);
static void sp_pattern_painter_init (SPPatternPainter *painter);
static void sp_pattern_painter_finalize (ArikkeiObject *object);

static NRRenderer *sp_pattern_painter_new_renderer (NRPaintServer *server, const NRMatrixF *transform, const NRRectF *paintbox);
static void sp_pattern_painter_release_renderer (NRPaintServer *server, NRRenderer *renderer);

static SPArenaPainterClass *painter_parent_class;

unsigned int
sp_pattern_painter_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		type = arikkei_object_register_type (SP_TYPE_ARENA_PAINTER,
						(const unsigned char *) "SPPatternPainter",
						sizeof (SPPatternPainterClass),
						sizeof (SPPatternPainter),
						(void (*) (ArikkeiObjectClass *)) sp_pattern_painter_class_init,
						(void (*) (ArikkeiObject *)) sp_pattern_painter_init);
	}
	return type;
}

static void
sp_pattern_painter_class_init (SPPatternPainterClass *klass)
{
	ArikkeiObjectClass *object_class;
	NRPaintServerClass *pserver_class;

	object_class = (ArikkeiObjectClass *) klass;
	pserver_class = (NRPaintServerClass *) klass;

	painter_parent_class = (SPArenaPainterClass *) ((ArikkeiObjectClass *) klass)->parent;

	object_class->finalize = sp_pattern_painter_finalize;
	pserver_class->new_renderer = sp_pattern_painter_new_renderer;
	pserver_class->release_renderer = sp_pattern_painter_release_renderer;
}

static void
sp_pattern_painter_init (SPPatternPainter *painter)
{
}

static void
sp_pattern_painter_finalize (ArikkeiObject *object)
{
	if (((ArikkeiObjectClass *) painter_parent_class)->finalize)
		((ArikkeiObjectClass *) painter_parent_class)->finalize (object);
}

static NRRenderer *
sp_pattern_painter_new_renderer (NRPaintServer *server, const NRMatrixF *transform, const NRRectF *paintbox)
{
	SPArenaPainter *painter;
	SPPattern *pattern;
	SPPatternRenderer *ptr;

	painter = SP_ARENA_PAINTER(server);
	pattern = SP_PATTERN (painter->spserver);

	ptr = (SPPatternRenderer *) malloc (sizeof (SPPatternRenderer));

	/* fixme: set up */

	return (NRRenderer *) ptr;
}

static void
sp_pattern_painter_release_renderer (NRPaintServer *server, NRRenderer *renderer)
{
}

/*
 * Pattern
 */


static void sp_pattern_class_init (SPPatternClass *klass);
static void sp_pattern_init (SPPattern *gr);

static void sp_pattern_build (SPObject *object, SPDocument *document, TheraNode *node);
static void sp_pattern_release (SPObject *object);
static void sp_pattern_set (SPObject *object, unsigned int key, const unsigned char *value);
static SPObject *sp_pattern_child_added (SPObject *object, TheraNode *child, TheraNode *ref);
static unsigned int sp_pattern_remove_child (SPObject *object, TheraNode *child);
static void sp_pattern_update (SPObject *object, SPCtx *ctx, unsigned int flags);
static void sp_pattern_modified (SPObject *object, unsigned int flags);
static unsigned int sp_pattern_sequence (SPObject *object, SPObject *target, unsigned int *seq);

static void sp_pattern_href_destroy (SPObject *href, SPPattern *pattern);
static void sp_pattern_href_modified (SPObject *href, guint flags, SPPattern *pattern);

#if 0
static SPPainter *sp_pattern_painter_new (SPPaintServer *ps, const NRMatrixF *affine, const NRRectF *bbox);
static void sp_pattern_painter_free (SPPaintServer *ps, SPPainter *painter);
#endif

static SPPaintServerClass * pattern_parent_class;

GType
sp_pattern_get_type (void)
{
	static GType pattern_type = 0;
	if (!pattern_type) {
		GTypeInfo pattern_info = {
			sizeof (SPPatternClass),
			NULL, NULL,
			(GClassInitFunc) sp_pattern_class_init,
			NULL, NULL,
			sizeof (SPPattern),
			16,
			(GInstanceInitFunc) sp_pattern_init,
		};
		pattern_type = g_type_register_static (SP_TYPE_PAINT_SERVER, "SPPattern", &pattern_info, 0);
	}
	return pattern_type;
}

static void
sp_pattern_class_init (SPPatternClass *klass)
{
	SPObjectClass *sp_object_class;
	SPPaintServerClass *ps_class;

	sp_object_class = (SPObjectClass *) klass;
	ps_class = (SPPaintServerClass *) klass;

	pattern_parent_class = g_type_class_ref (SP_TYPE_PAINT_SERVER);

	sp_object_class->build = sp_pattern_build;
	sp_object_class->release = sp_pattern_release;
	sp_object_class->set = sp_pattern_set;
	sp_object_class->child_added = sp_pattern_child_added;
	sp_object_class->remove_child = sp_pattern_remove_child;
	sp_object_class->update = sp_pattern_update;
	sp_object_class->modified = sp_pattern_modified;
	sp_object_class->sequence = sp_pattern_sequence;

#if 0
	ps_class->painter_new = sp_pattern_painter_new;
	ps_class->painter_free = sp_pattern_painter_free;
#endif
}

static void
sp_pattern_init (SPPattern *pat)
{
	SPObject *object;

	object = SP_OBJECT(pat);

	object->has_children = 1;

	pat->patternUnits = SP_PATTERN_UNITS_OBJECTBOUNDINGBOX;
	pat->patternUnits = SP_PATTERN_UNITS_USERSPACEONUSE;

	nr_matrix_d_set_identity (&pat->patternTransform);

	sp_svg_length_unset (&pat->x, SP_SVG_UNIT_NONE, 0.0, 0.0);
	sp_svg_length_unset (&pat->y, SP_SVG_UNIT_NONE, 0.0, 0.0);
	sp_svg_length_unset (&pat->width, SP_SVG_UNIT_NONE, 0.0, 0.0);
	sp_svg_length_unset (&pat->height, SP_SVG_UNIT_NONE, 0.0, 0.0);
}

static void
sp_pattern_build (SPObject *object, SPDocument *document, TheraNode *node)
{
#if 0
	SPPattern *pat;
	SPObject *last;
	TheraNode *rchild;

	pat = SP_PATTERN (object);
#endif

	if (((SPObjectClass *) pattern_parent_class)->build)
		(* ((SPObjectClass *) pattern_parent_class)->build) (object, document, node);

	sp_object_read_attr (object, "patternUnits");
	sp_object_read_attr (object, "patternContentUnits");
	sp_object_read_attr (object, "patternTransform");
	sp_object_read_attr (object, "x");
	sp_object_read_attr (object, "y");
	sp_object_read_attr (object, "width");
	sp_object_read_attr (object, "height");
	sp_object_read_attr (object, "viewBox");
	sp_object_read_attr (object, "xlink:href");

#if 0
	last = NULL;
	for (rchild = thera_node_get_first_child (node); rchild != NULL; rchild = thera_node_get_next_sibling (rchild)) {
		GType type;
		SPObject * child;
		type = sp_node_type_lookup (rchild);
		child = g_object_new (type, NULL);
		if (last) {
			last->next = sp_object_attach_reref (object, child, NULL);
		} else {
			object->children = sp_object_attach_reref (object, child, NULL);
		}
		sp_object_invoke_build (child, document, rchild, SP_OBJECT_IS_CLONED (object));
		last = child;
	}
#endif

	/* Register ourselves */
	sp_document_add_resource (document, "pattern", object);
}

static void
sp_pattern_release (SPObject *object)
{
	SPPattern *pat;

	pat = (SPPattern *) object;

	sp_document_remove_resource (SP_OBJECT_DOCUMENT (object), "pattern", SP_OBJECT (object));

	if (pat->href) {
		sp_signal_disconnect_by_data (pat->href, pat);
		sp_object_hunref (SP_OBJECT (pat->href), object);
	}

	if (((SPObjectClass *) pattern_parent_class)->release)
		((SPObjectClass *) pattern_parent_class)->release (object);
}

static void
sp_pattern_set (SPObject *object, unsigned int key, const unsigned char *value)
{
	SPPattern *pat;

	pat = SP_PATTERN (object);

	/* fixme: We should unset properties, if val == NULL */
	switch (key) {
	case SP_ATTR_PATTERNUNITS:
		if (value) {
			if (!strcmp (value, "userSpaceOnUse")) {
				pat->patternUnits = SP_PATTERN_UNITS_USERSPACEONUSE;
			} else {
				pat->patternUnits = SP_PATTERN_UNITS_OBJECTBOUNDINGBOX;
			}
			pat->patternUnits_set = TRUE;
		} else {
			pat->patternUnits_set = FALSE;
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_PATTERNCONTENTUNITS:
		if (value) {
			if (!strcmp (value, "userSpaceOnUse")) {
				pat->patternContentUnits = SP_PATTERN_UNITS_USERSPACEONUSE;
			} else {
				pat->patternContentUnits = SP_PATTERN_UNITS_OBJECTBOUNDINGBOX;
			}
			pat->patternContentUnits_set = TRUE;
		} else {
			pat->patternContentUnits_set = FALSE;
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_PATTERNTRANSFORM: {
		NRMatrixF t;
		if (value && sp_svg_transform_read (value, &t)) {
			int i;
			for (i = 0; i < 6; i++) pat->patternTransform.c[i] = t.c[i];
			pat->patternTransform_set = TRUE;
		} else {
			nr_matrix_d_set_identity (&pat->patternTransform);
			pat->patternTransform_set = FALSE;
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	}
	case SP_ATTR_X:
		if (!sp_svg_length_read (value, &pat->x)) {
			sp_svg_length_unset (&pat->x, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_Y:
		if (!sp_svg_length_read (value, &pat->y)) {
			sp_svg_length_unset (&pat->y, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_WIDTH:
		if (!sp_svg_length_read (value, &pat->width)) {
			sp_svg_length_unset (&pat->width, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_HEIGHT:
		if (!sp_svg_length_read (value, &pat->height)) {
			sp_svg_length_unset (&pat->height, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_VIEWBOX: {
		/* fixme: Think (Lauris) */
		double x, y, width, height;
		char *eptr;

		if (value) {
			eptr = (gchar *) value;
			eptr += arikkei_strtod_exp (eptr, 1024, &x);
			while (*eptr && ((*eptr == ',') || (*eptr == ' '))) eptr++;
			eptr += arikkei_strtod_exp (eptr, 1024, &y);
			while (*eptr && ((*eptr == ',') || (*eptr == ' '))) eptr++;
			eptr += arikkei_strtod_exp (eptr, 1024, &width);
			while (*eptr && ((*eptr == ',') || (*eptr == ' '))) eptr++;
			eptr += arikkei_strtod_exp (eptr, 1024, &height);
			while (*eptr && ((*eptr == ',') || (*eptr == ' '))) eptr++;
			if ((width > 0) && (height > 0)) {
				pat->viewBox.x0 = x;
				pat->viewBox.y0 = y;
				pat->viewBox.x1 = x + width;
				pat->viewBox.y1 = y + height;
				pat->viewBox_set = TRUE;
			} else {
				pat->viewBox_set = FALSE;
			}
		} else {
			pat->viewBox_set = FALSE;
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_VIEWPORT_MODIFIED_FLAG);
		break;
	}
	case SP_ATTR_XLINK_HREF:
		if (pat->href) {
			sp_signal_disconnect_by_data (pat->href, pat);
			pat->href = (SPPattern *) sp_object_hunref (SP_OBJECT (pat->href), object);
		}
		if (value && *value == '#') {
			SPObject *href;
			href = sp_document_lookup_id (object->document, value + 1);
			if (SP_IS_PATTERN (href)) {
				pat->href = (SPPattern *) sp_object_href (href, object);
				g_signal_connect (G_OBJECT (href), "destroy", G_CALLBACK (sp_pattern_href_destroy), pat);
				g_signal_connect (G_OBJECT (href), "modified", G_CALLBACK (sp_pattern_href_modified), pat);
			}
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	default:
		if (((SPObjectClass *) pattern_parent_class)->set)
			((SPObjectClass *) pattern_parent_class)->set (object, key, value);
		break;
	}
}

static SPObject *
sp_pattern_child_added (SPObject *object, TheraNode *child, TheraNode *ref)
{
	SPPattern *pat;
	SPObject *ochild, *prev;
	gint position;
#if 0
	GType type;

#endif
	pat = SP_PATTERN (object);

	ochild = NULL;
	if (((SPObjectClass *) (pattern_parent_class))->child_added)
		ochild = ((SPObjectClass *) (pattern_parent_class))->child_added (object, child, ref);

	/* Search for position reference */
	prev = NULL;
	position = 0;
	if (ref != NULL) {
		prev = object->children;
		while (prev && (prev->node != ref)) {
			if (SP_IS_ITEM (prev)) position += 1;
			prev = prev->next;
		}
		if (SP_IS_ITEM (prev)) position += 1;
	}

#if 0
	type = sp_node_type_lookup (child);
	ochild = g_object_new (type, NULL);
	if (prev) {
		prev->next = sp_object_attach_reref (object, ochild, prev->next);
	} else {
		object->children = sp_object_attach_reref (object, ochild, object->children);
	}

	sp_object_invoke_build (ochild, object->document, child, SP_OBJECT_IS_CLONED (object));
#endif

#if 0
	if (SP_IS_ITEM (ochild)) {
		SPPaintServer *ps;
		SPPainter *p;
		/* Huh (Lauris) */
		ps = SP_PAINT_SERVER (pat);
		for (p = ps->painters; p != NULL; p = p->next) {
			SPPatPainter *pp;
			NRArenaItem *ai;
			pp = (SPPatPainter *) p;
			ai = sp_item_invoke_show (SP_ITEM (ochild), pp->arena, pp->dkey, SP_ITEM_REFERENCE_FLAGS);
			if (ai) {
				nr_arena_item_add_child (pp->root, ai, NULL);
				nr_arena_item_set_order (ai, position);
				nr_arena_item_unref (ai);
			}
		}
	}
#endif

	return ochild;
}

static unsigned int
sp_pattern_remove_child (SPObject *object, TheraNode *child)
{
#if 0
	SPPattern *pat;
	SPObject *prev, *ochild;

	pat = SP_PATTERN (object);
#endif

	if (((SPObjectClass *) (pattern_parent_class))->remove_child) {
		unsigned int ret;
		ret = ((SPObjectClass *) (pattern_parent_class))->remove_child (object, child);
		if (!ret) return ret;
	}

#if 0
	prev = NULL;
	ochild = object->children;
	while (ochild->node != child) {
		prev = ochild;
		ochild = ochild->next;
	}

	if (prev) {
		prev->next = sp_object_detach_unref (object, prev->next);
	} else {
		object->children = sp_object_detach_unref (object, object->children);
	}
#endif

	return TRUE;
}

/* fixme: We need ::order_changed handler too (Lauris) */

/* fixme: Transformation and stuff (Lauris) */

static void
sp_pattern_update (SPObject *object, SPCtx *ctx, unsigned int flags)
{
	SPPattern *pat;
	SPObject *child;
	GSList *l;

	pat = SP_PATTERN (object);

	if (flags & SP_OBJECT_MODIFIED_FLAG) flags |= SP_OBJECT_PARENT_MODIFIED_FLAG;
	flags &= SP_OBJECT_MODIFIED_CASCADE;

	l = NULL;
	for (child = object->children; child != NULL; child = child->next) {
		sp_object_ref (child, NULL);
		l = g_slist_prepend (l, child);
	}
	l = g_slist_reverse (l);
	while (l) {
		child = SP_OBJECT (l->data);
		l = g_slist_remove (l, child);
		if (flags || (child->mflags & (SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_CHILD_MODIFIED_FLAG))) {
			sp_object_invoke_update (child, ctx, flags);
		}
		sp_object_unref (child, NULL);
	}
}

static void
sp_pattern_modified (SPObject *object, guint flags)
{
	SPPattern *pat;
	SPObject *child;
	GSList *l;

	pat = SP_PATTERN (object);

	if (flags & SP_OBJECT_MODIFIED_FLAG) flags |= SP_OBJECT_PARENT_MODIFIED_FLAG;
	flags &= SP_OBJECT_MODIFIED_CASCADE;

	l = NULL;
	for (child = object->children; child != NULL; child = child->next) {
		sp_object_ref (child, NULL);
		l = g_slist_prepend (l, child);
	}
	l = g_slist_reverse (l);
	while (l) {
		child = SP_OBJECT (l->data);
		l = g_slist_remove (l, child);
		if (flags || (child->mflags & (SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_CHILD_MODIFIED_FLAG))) {
			sp_object_invoke_modified (child, flags);
		}
		sp_object_unref (child, NULL);
	}
}

static unsigned int
sp_pattern_sequence (SPObject *object, SPObject *target, unsigned int *seq)
{
	SPPattern *pat;
	SPObject *child;

	pat = (SPPattern *) object;

	for (child = object->children; child != NULL; child = child->next) {
		*seq += 1;
		if (sp_object_invoke_sequence (child, target, seq)) return TRUE;
	}

	return FALSE;
}

static void
sp_pattern_href_destroy (SPObject *href, SPPattern *pattern)
{
	pattern->href = (SPPattern *) sp_object_hunref (href, pattern);
	sp_object_request_modified (SP_OBJECT (pattern), SP_OBJECT_MODIFIED_FLAG);
}

static void
sp_pattern_href_modified (SPObject *href, guint flags, SPPattern *pattern)
{
	sp_object_request_modified (SP_OBJECT (pattern), SP_OBJECT_MODIFIED_FLAG);
}

#if 0
/* Painter */

static void sp_pat_fill (SPPainter *painter, NRPixBlock *pb);

static SPPainter *
sp_pattern_painter_new (SPPaintServer *ps, const NRMatrixF *ctm, const NRRectF *bbox)
{
	SPPattern *pat;
	SPPatPainter *pp;
	SPObject *child;
	NRGC gc;

	pat = SP_PATTERN (ps);

	pp = g_new (SPPatPainter, 1);

#if 0
	pp->painter.type = SP_PAINTER_IND;
	pp->painter.fill = sp_pat_fill;
#endif

	pp->pat = pat;

	/*
	 * The order should be:
	 * CTM x [BBOX] x PTRANS
	 */

	if (pat->patternUnits == SP_PATTERN_UNITS_OBJECTBOUNDINGBOX) {
		NRMatrixF bbox2user;
		NRMatrixF ps2user;

		/* patternTransform goes here (Lauris) */

		/* BBox to user coordinate system */
		bbox2user.c[0] = bbox->x1 - bbox->x0;
		bbox2user.c[1] = 0.0;
		bbox2user.c[2] = 0.0;
		bbox2user.c[3] = bbox->y1 - bbox->y0;
		bbox2user.c[4] = bbox->x0;
		bbox2user.c[5] = bbox->y0;

		/* fixme: (Lauris) */
		nr_matrix_multiply_fdf (&ps2user, &pat->patternTransform, &bbox2user);
		nr_matrix_multiply_ffd (&pp->ps2px, &ps2user, (NRMatrixD *) ctm);
	} else {
		/* Problem: What to do, if we have mixed lengths and percentages? */
		/* Currently we do ignore percentages at all, but that is not good (lauris) */

		/* fixme: We may try to normalize here too, look at linearGradient (Lauris) */

		/* fixme: (Lauris) */
		nr_matrix_multiply_fdd (&pp->ps2px, &pat->patternTransform, (NRMatrixD *) ctm);
	}

	nr_matrix_f_invert (&pp->px2ps, &pp->ps2px);

	/*
	 * The order should be:
	 * CTM x [BBOX] x PTRANS x VIEWBOX | UNITS
	 */

	if (pat->viewBox_set) {
		NRMatrixF vb2ps, vb2us;
		/* Forget content units at all */
		vb2ps.c[0] = pat->width.computed / (pat->viewBox.x1 - pat->viewBox.x0);
		vb2ps.c[1] = 0.0;
		vb2ps.c[2] = 0.0;
		vb2ps.c[3] = pat->width.computed / (pat->viewBox.y1 - pat->viewBox.y0);
		vb2ps.c[4] = -pat->viewBox.x0 * vb2ps.c[0];
		vb2ps.c[5] = -pat->viewBox.y0 * vb2ps.c[3];
		/* Problem: What to do, if we have mixed lengths and percentages? (Lauris) */
		/* Currently we do ignore percentages at all, but that is not good (Lauris) */
		/* fixme: (Lauris) */
		nr_matrix_multiply_ffd (&vb2us, &vb2ps, &pat->patternTransform);
		nr_matrix_multiply_ffd (&pp->pcs2px, &vb2us, (NRMatrixD *) ctm);
	} else {
		NRMatrixF t;

		/* No viewbox, have to parse units */
		if (pat->patternContentUnits == SP_PATTERN_UNITS_OBJECTBOUNDINGBOX) {
			NRMatrixF bbox2user;
			NRMatrixF pcs2user;

			/* patternTransform goes here (Lauris) */

			/* BBox to user coordinate system */
			bbox2user.c[0] = bbox->x1 - bbox->x0;
			bbox2user.c[1] = 0.0;
			bbox2user.c[2] = 0.0;
			bbox2user.c[3] = bbox->y1 - bbox->y0;
			bbox2user.c[4] = bbox->x0;
			bbox2user.c[5] = bbox->y0;

			/* fixme: (Lauris) */
			nr_matrix_multiply_fdf (&pcs2user, &pat->patternTransform, &bbox2user);
			nr_matrix_multiply_ffd (&pp->pcs2px, &pcs2user, (NRMatrixD *) ctm);
		} else {
			/* Problem: What to do, if we have mixed lengths and percentages? */
			/* Currently we do ignore percentages at all, but that is not good (lauris) */
			/* fixme: (Lauris) */
			nr_matrix_multiply_fdd (&pp->pcs2px, &pat->patternTransform, (NRMatrixD *) ctm);
		}

		nr_matrix_f_set_translate (&t, pat->x.computed, pat->y.computed);
		/* fixme: Think about it (Lauris) */
		nr_matrix_multiply_fff (&pp->pcs2px, &t, &pp->pcs2px);
	}

	/* fixme: Create arena */
	/* fixme: Actually we need some kind of constructor function */
	/* fixme: But to do that, we need actual arena implementaion */
	pp->arena = (NRArena *) arikkei_object_new (NR_TYPE_ARENA);

	pp->dkey = sp_item_display_key_new (1);

	/* fixme: Create group */
	pp->root = nr_arena_item_new (pp->arena, NR_TYPE_ARENA_GROUP);

	/* fixme: Show items */
	/* fixme: Among other thing we want to traverse href here */
	for (child = ((SPObject *) pat)->children; child != NULL; child = child->next) {
		if (SP_IS_ITEM (child)) {
			NRArenaItem *cai;
			cai = sp_item_invoke_show (SP_ITEM (child), pp->arena, pp->dkey, SP_ITEM_REFERENCE_FLAGS);
			nr_arena_item_append_child (pp->root, cai);
			nr_arena_item_unref (cai);
		}
	}

	/* nr_matrix_d_from_f (&gc.transform, &pp->pcs2px); */
	gc.transform = pp->pcs2px;

	nr_arena_item_invoke_update (pp->root, NULL, &gc, NR_ARENA_ITEM_STATE_ALL, NR_ARENA_ITEM_STATE_ALL);

	return (SPPainter *) pp;
}

static void
sp_pattern_painter_free (SPPaintServer *ps, SPPainter *painter)
{
	SPPatPainter *pp;
	SPPattern *pat;
	SPObject *child;

	pp = (SPPatPainter *) painter;
	pat = pp->pat;

	/* fixme: Among other thing we want to traverse href here */
	for (child = ((SPObject *) pat)->children; child != NULL; child = child->next) {
		if (SP_IS_ITEM (child)) {
			sp_item_invoke_hide (SP_ITEM (child), pp->dkey);
		}
	}

	if (pp->root) {
		nr_arena_item_unref (pp->root);
	}

	if (pp->arena) {
		arikkei_object_unref ((ArikkeiObject *) pp->arena);
	}

	g_free (pp);
}

static void
sp_pat_fill (SPPainter *painter, NRPixBlock *pb)
{
	SPPatPainter *pp;
	NRRectF ba, psa;
	NRRectL area;
	float x, y;

	pp = (SPPatPainter *) painter;

	if (pp->pat->width.computed < NR_EPSILON_F) return;
	if (pp->pat->height.computed < NR_EPSILON_F) return;

	/* Find buffer area in gradient space */
	/* fixme: This is suboptimal (Lauris) */

	ba.x0 = pb->area.x0;
	ba.y0 = pb->area.y0;
	ba.x1 = pb->area.x1;
	ba.y1 = pb->area.y1;
	nr_rect_f_matrix_f_transform (&psa, &ba, &pp->px2ps);

	psa.x0 = floor ((psa.x0 - pp->pat->x.computed) / pp->pat->width.computed);
	psa.y0 = floor ((psa.y0 - pp->pat->y.computed) / pp->pat->height.computed);
	psa.x1 = ceil ((psa.x1 - pp->pat->x.computed) / pp->pat->width.computed);
	psa.y1 = ceil ((psa.y1 - pp->pat->y.computed) / pp->pat->height.computed);

	for (y = psa.y0; y < psa.y1; y++) {
		for (x = psa.x0; x < psa.x1; x++) {
			NRPixBlock ppb;
			float psx, psy;

			psx = x * pp->pat->width.computed;
			psy = y * pp->pat->height.computed;

			area.x0 = pb->area.x0 - (pp->ps2px.c[0] * psx + pp->ps2px.c[2] * psy);
			area.y0 = pb->area.y0 - (pp->ps2px.c[1] * psx + pp->ps2px.c[3] * psy);
			area.x1 = area.x0 + pb->area.x1 - pb->area.x0;
			area.y1 = area.y0 + pb->area.y1 - pb->area.y0;

			/* We do not update here anymore */

			/* Set up buffer */
			/* fixme: (Lauris) */
			nr_pixblock_setup_extern (&ppb, pb->mode, area.x0, area.y0, area.x1, area.y1, NR_PIXBLOCK_PX (pb), pb->rs, FALSE, FALSE);

			nr_arena_item_invoke_render (pp->root, &area, &ppb, 0);

			nr_pixblock_release (&ppb);
		}
	}
}

#endif
