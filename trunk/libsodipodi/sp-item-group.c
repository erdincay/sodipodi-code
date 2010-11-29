#define __SP_GROUP_C__

/*
 * SVG <g> implementation
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 1999-2002 authors
 * Copyright (C) 2000-2001 Ximian, Inc.
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include <libnrarena/nr-arena-group.h>

#if 0
#include "xml/node-private.h"
#include "sp-object-node.h"
#include "document.h"
#include "style.h"
#endif

#include "intl.h"
#include "enums.h"
#include "attributes.h"
#include "svg/svg.h"
#include "factory.h"
#include "document.h"

#include "sp-root.h"
#include "sp-item-group.h"

static void sp_group_class_init (SPGroupClass *klass);
static void sp_group_init (SPGroup *group);

static void sp_group_build (SPObject * object, SPDocument * document, TheraNode * node);
static void sp_group_release (SPObject *object);
static SPObject *sp_group_child_added (SPObject * object, TheraNode * child, TheraNode * ref);
static unsigned int sp_group_remove_child (SPObject *object, TheraNode *child);
static void sp_group_order_changed (SPObject * object, TheraNode * child, TheraNode * old, TheraNode * new);
static void sp_group_update (SPObject *object, SPCtx *ctx, guint flags);
static void sp_group_modified (SPObject *object, guint flags);
static unsigned int sp_group_sequence (SPObject *object, SPObject *target, unsigned int *seq);
static TheraNode *sp_group_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags);

static void sp_group_bbox (SPItem *item, NRRectF *bbox, const NRMatrixF *transform, unsigned int flags);
static void sp_group_print (SPItem * item, SPPrintContext *ctx);
static gchar * sp_group_description (SPItem * item);
static NRArenaItem *sp_group_show (SPItem *item, NRArena *arena, unsigned int key, unsigned int flags);
static void sp_group_hide (SPItem * item, unsigned int key);

static SPItemClass * parent_class;

GType
sp_group_get_type (void)
{
	static GType type = 0;
	if (!type) {
		GTypeInfo info = {
			sizeof (SPGroupClass),
			NULL, NULL,
			(GClassInitFunc) sp_group_class_init,
			NULL, NULL,
			sizeof (SPGroup),
			16,
			(GInstanceInitFunc) sp_group_init,
		};
		type = g_type_register_static (SP_TYPE_ITEM, "SPGroup", &info, 0);
	}
	return type;
}

static void
sp_group_class_init (SPGroupClass *klass)
{
	GObjectClass * object_class;
	SPObjectClass * sp_object_class;
	SPItemClass * item_class;

	object_class = (GObjectClass *) klass;
	sp_object_class = (SPObjectClass *) klass;
	item_class = (SPItemClass *) klass;

	parent_class = g_type_class_peek_parent (klass);

	sp_object_class->build = sp_group_build;
	sp_object_class->release = sp_group_release;
	sp_object_class->child_added = sp_group_child_added;
	sp_object_class->remove_child = sp_group_remove_child;
	sp_object_class->order_changed = sp_group_order_changed;
	sp_object_class->update = sp_group_update;
	sp_object_class->modified = sp_group_modified;
	sp_object_class->sequence = sp_group_sequence;
	sp_object_class->write = sp_group_write;

	item_class->bbox = sp_group_bbox;
	item_class->print = sp_group_print;
	item_class->description = sp_group_description;
	item_class->show = sp_group_show;
	item_class->hide = sp_group_hide;
}

static void
sp_group_init (SPGroup *group)
{
	SPObject *object;
	object = SP_OBJECT(group);
	object->has_children = 1;
}

static void sp_group_build (SPObject *object, SPDocument *doc, TheraNode *node)
{
#if 0
	SPGroup *group;
	SPObject *last;
	TheraNode *rchild;

	group = SP_GROUP (object);
#endif

	if (((SPObjectClass *) (parent_class))->build)
		((SPObjectClass *) (parent_class))->build (object, doc, node);

#if 0
	last = NULL;
	for (rchild = thera_node_get_first_child (node); rchild != NULL; rchild = thera_node_get_next_sibling (rchild)) {
		GType type;
		SPObject * child;
		type = sp_node_type_lookup (rchild);
		child = g_object_new (type, 0);
		if (last) {
			last->next = sp_object_attach_reref (object, child, NULL);
		} else {
			object->children = sp_object_attach_reref (object, child, NULL);
		}
		/* fixme: This is bad because we emit object_added with unbuilt object */
		/* Demo, sikata nai, we have to have it done before ::build emits the same for children */
		if (doc->object_signals) {
			sp_document_invoke_object_added (doc, object, last);
		}
		sp_object_invoke_build (child, doc, rchild, SP_OBJECT_IS_CLONED (object));
		last = child;
	}
#endif
}

static void
sp_group_release (SPObject *object)
{
#if 0
	SPGroup * group;

	group = SP_GROUP (object);

	while (object->children) {
		object->children = sp_object_detach_unref (object, object->children);
		if (object->document->object_signals) {
			sp_document_invoke_object_removed (object->document, object, NULL);
		}
	}
#endif

	if (((SPObjectClass *) parent_class)->release)
		((SPObjectClass *) parent_class)->release (object);
}

static SPObject *
sp_group_child_added (SPObject *object, TheraNode *thechild, TheraNode *theref)
{
	SPItem *item;
	SPObject *child, *ref;
	unsigned int position;
#if 0
	SPGroup *group;
	GType type;
	SPObject *ochild, *prev;
	gint position;

	group = SP_GROUP (object);
#endif

	item = SP_ITEM (object);

	child = NULL;
	if (((SPObjectClass *) (parent_class))->child_added)
		child = ((SPObjectClass *) (parent_class))->child_added (object, thechild, theref);

	/* Search for position reference */
	ref = NULL;
	position = 0;
	if (theref != NULL) {
		ref = object->children;
		while (ref && (ref->node != theref)) {
			if (SP_IS_ITEM (ref)) position += 1;
			ref = ref->next;
		}
		if (SP_IS_ITEM (ref)) position += 1;
	}

#if 0
	type = sp_node_type_lookup (child);
	ochild = g_object_new (type, 0);
	if (prev) {
		prev->next = sp_object_attach_reref (object, ochild, prev->next);
	} else {
		object->children = sp_object_attach_reref (object, ochild, object->children);
	}
	/* fixme: This is bad because we emit object_added with unbuilt object */
	/* Demo, sikata nai, we have to have it done before ::build emits the same for children */
	if (object->document->object_signals) {
		sp_document_invoke_object_added (object->document, object, prev);
	}
	sp_object_invoke_build (ochild, object->document, child, SP_OBJECT_IS_CLONED (object));
#endif

	if (SP_IS_ITEM (child)) {
		SPItemView *v;
		NRArenaItem *ac;
		for (v = item->display; v != NULL; v = v->view.next) {
			ac = sp_item_invoke_show (SP_ITEM (child), NR_ARENA_ITEM_ARENA (v), v->view.key, v->view.flags);
			if (ac) {
				nr_arena_item_add_child (v, ac, NULL);
				nr_arena_item_set_order (ac, position);
				nr_arena_item_unref (ac);
			}
		}
	}

	return child;
}

/* fixme: hide (Lauris) */

static unsigned int
sp_group_remove_child (SPObject * object, TheraNode * child)
{
#if 0
	SPGroup *group;
	SPObject *prev, *ochild;

	group = SP_GROUP (object);
#endif

	if (((SPObjectClass *) (parent_class))->remove_child) {
		unsigned int ret;
		ret = ((SPObjectClass *) (parent_class))->remove_child (object, child);
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
		prev->next = sp_object_detach_unref (object, ochild);
	} else {
		object->children = sp_object_detach_unref (object, ochild);
	}
	if (object->document->object_signals) {
		sp_document_invoke_object_removed (object->document, object, prev);
	}
	sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
#endif

	return TRUE;
}

static void
sp_group_order_changed (SPObject *object, TheraNode *child, TheraNode *old, TheraNode *new)
{
	SPObject *childobj, *oldobj, *newobj, *o;
	gint childpos, oldpos, newpos;
#if 0
	SPGroup *group;

	group = SP_GROUP (object);
#endif

	if (((SPObjectClass *) (parent_class))->order_changed)
		((SPObjectClass *) (parent_class))->order_changed (object, child, old, new);

	childobj = oldobj = newobj = NULL;
	oldpos = newpos = 0;

	/* Scan children list */
	childpos = 0;
	/* fixme: This is very suspicious */
	/* for (o = object->children; !childobj || (old && !oldobj) || (new && !newobj); o = o->next) { */
	for (o = object->children; !childobj || (old && !oldobj) || (new && !newobj); o = o->next) {
		/* fixme: Make this thing to work */
		if (!o) break;
		if (o->node == child) {
			childobj = o;
		} else {
			if (SP_IS_ITEM (o)) childpos += 1;
		}
		if (old && o->node == old) {
			oldobj = o;
			oldpos = childpos;
		}
		if (new && o->node == new) {
			newobj = o;
			newpos = childpos;
		}
	}

#if 0
	if (oldobj) {
		oldobj->next = childobj->next;
	} else {
		object->children = childobj->next;
	}
	if (newobj) {
		childobj->next = newobj->next;
		newobj->next = childobj;
	} else {
		childobj->next = object->children;
		object->children = childobj;
	}
	if (object->document->object_signals) {
		sp_document_invoke_order_changed (object->document, object, oldobj, newobj);
	}
#endif

	if (SP_IS_ITEM (childobj)) {
		SPItemView *v;
		for (v = SP_ITEM (childobj)->display; v != NULL; v = v->view.next) {
			nr_arena_item_set_order (v, newpos);
		}
	}

	sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
}

static void
sp_group_update (SPObject *object, SPCtx *ctx, unsigned int flags)
{
	SPGroup *group;
	SPObject *child;
	SPItemCtx *ictx, cctx;
	GSList *l;

	group = SP_GROUP (object);
	ictx = (SPItemCtx *) ctx;
	cctx = *ictx;

	if (((SPObjectClass *) (parent_class))->update)
		((SPObjectClass *) (parent_class))->update (object, ctx, flags);

	if (flags & SP_OBJECT_MODIFIED_FLAG) flags |= SP_OBJECT_PARENT_MODIFIED_FLAG;
	flags &= SP_OBJECT_MODIFIED_CASCADE;

	l = NULL;
	for (child = object->children; child != NULL; child = child->next) {
		g_object_ref (G_OBJECT (child));
		l = g_slist_prepend (l, child);
	}
	l = g_slist_reverse (l);
	while (l) {
		child = SP_OBJECT (l->data);
		l = g_slist_remove (l, child);
		if (flags || (child->uflags & (SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_CHILD_MODIFIED_FLAG))) {
			if (SP_IS_ITEM (child)) {
				SPItem *chi;
				chi = SP_ITEM (child);
				nr_matrix_multiply_dfd (&cctx.i2doc, &chi->transform, &ictx->i2doc);
				nr_matrix_multiply_dfd (&cctx.i2vp, &chi->transform, &ictx->i2vp);
				sp_object_invoke_update (child, (SPCtx *) &cctx, flags);
			} else {
				sp_object_invoke_update (child, ctx, flags);
			}
		}
		g_object_unref (G_OBJECT (child));
	}
}

static void
sp_group_modified (SPObject *object, guint flags)
{
	SPGroup *group;
	SPObject *child;
	GSList *l;

	group = SP_GROUP (object);

	if (flags & SP_OBJECT_MODIFIED_FLAG) flags |= SP_OBJECT_PARENT_MODIFIED_FLAG;
	flags &= SP_OBJECT_MODIFIED_CASCADE;

	l = NULL;
	for (child = object->children; child != NULL; child = child->next) {
		g_object_ref (G_OBJECT (child));
		l = g_slist_prepend (l, child);
	}
	l = g_slist_reverse (l);
	while (l) {
		child = SP_OBJECT (l->data);
		l = g_slist_remove (l, child);
		if (flags || (child->mflags & (SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_CHILD_MODIFIED_FLAG))) {
			sp_object_invoke_modified (child, flags);
		}
		g_object_unref (G_OBJECT (child));
	}
}

static unsigned int
sp_group_sequence (SPObject *object, SPObject *target, unsigned int *seq)
{
	SPGroup *group;
	SPObject *child;

	group = (SPGroup *) object;

	for (child = object->children; child != NULL; child = child->next) {
		*seq += 1;
		if (sp_object_invoke_sequence (child, target, seq)) return TRUE;
	}

	return FALSE;
}

static TheraNode *
sp_group_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags)
{
	SPGroup *group;
	SPObject *child;
	TheraNode *crepr;

	group = SP_GROUP (object);

	if (flags & SP_OBJECT_WRITE_BUILD) {
		GSList *l;
		if (!node) node = thera_document_new_node (thedoc, THERA_NODE_ELEMENT, "g");
		l = NULL;
		for (child = object->children; child != NULL; child = child->next) {
			crepr = sp_object_invoke_write (child, thedoc, NULL, flags);
			if (crepr) l = g_slist_prepend (l, crepr);
		}
		while (l) {
			thera_node_add_child (node, (TheraNode *) l->data, NULL);
			/* sp_repr_unref ((TheraNode *) l->data); */
			l = g_slist_remove (l, l->data);
		}
	} else {
		for (child = object->children; child != NULL; child = child->next) {
			sp_object_invoke_write (child, thedoc, child->node, flags);
		}
	}

	if (((SPObjectClass *) (parent_class))->write)
		((SPObjectClass *) (parent_class))->write (object, thedoc, node, flags);

	return node;
}

static void
sp_group_bbox (SPItem *item, NRRectF *bbox, const NRMatrixF *transform, unsigned int flags)
{
	SPGroup *group;
	SPItem *child;
	SPObject *o;

	group = SP_GROUP (item);

	for (o = ((SPObject *) item)->children; o != NULL; o = o->next) {
		if (SP_IS_ITEM (o)) {
			NRMatrixF ct;
			child = SP_ITEM (o);
			nr_matrix_multiply_fff (&ct, &child->transform, transform);
			sp_item_invoke_bbox_full (child, bbox, &ct, flags, FALSE);
		}
	}
}

static void
sp_group_print (SPItem * item, SPPrintContext *ctx)
{
	SPGroup * group;
	SPItem * child;
	SPObject * o;

	group = SP_GROUP (item);

	for (o = ((SPObject *) item)->children; o != NULL; o = o->next) {
		if (SP_IS_ITEM (o)) {
			child = SP_ITEM (o);
			sp_item_invoke_print (SP_ITEM (o), ctx);
		}
	}
}

static gchar *
sp_group_description (SPItem * item)
{
	SPGroup * group;
	SPObject * o;
	gint len;
	static char c[128];

	group = SP_GROUP (item);

	len = 0;
	for (o = ((SPObject *) item)->children; o != NULL; o = o->next) len += 1;

	g_snprintf (c, 128, _("Group of %d objects"), len);

	return g_strdup (c);
}

static NRArenaItem *
sp_group_show (SPItem *item, NRArena *arena, unsigned int key, unsigned int flags)
{
	SPGroup *group;
	NRArenaItem *ai, *ac, *ar;
	SPItem * child;
	SPObject * o;

	group = (SPGroup *) item;

	ai = nr_arena_item_new (arena, NR_TYPE_ARENA_GROUP);
	nr_arena_group_set_transparent (NR_ARENA_GROUP (ai), group->transparent);

	ar = NULL;
	for (o = ((SPObject *) item)->children; o != NULL; o = o->next) {
		if (SP_IS_ITEM (o)) {
			child = SP_ITEM (o);
			ac = sp_item_invoke_show (child, arena, key, flags);
			if (ac) {
				nr_arena_item_add_child (ai, ac, ar);
				ar = ac;
				nr_arena_item_unref (ac);
			}
		}
	}

	return ai;
}

static void
sp_group_hide (SPItem *item, unsigned int key)
{
	SPGroup * group;
	SPItem * child;
	SPObject * o;

	group = (SPGroup *) item;

	for (o = ((SPObject *) item)->children; o != NULL; o = o->next) {
		if (SP_IS_ITEM (o)) {
			child = SP_ITEM (o);
			sp_item_invoke_hide (child, key);
		}
	}

	if (((SPItemClass *) parent_class)->hide)
		((SPItemClass *) parent_class)->hide (item, key);
}

/* fixme: This is potentially dangerous (Lauris) */
/* Be extra careful what happens, if playing with <svg> */
void
sp_group_set_transparent (SPGroup *group, unsigned int transparent)
{
	SPItem *item;
	SPItemView *v;
	item = (SPItem *) group;
	group->transparent = transparent;
	for (v = item->display; v != NULL; v = v->view.next) {
		nr_arena_group_set_transparent (NR_ARENA_GROUP (v), group->transparent);
	}
}

/*
 * some API for list aspect of SPGroup
 */

GSList * 
sp_item_group_item_list (SPGroup * group)
{
        GSList *s;
	SPObject *o;

	g_return_val_if_fail (group != NULL, NULL);
	g_return_val_if_fail (SP_IS_GROUP (group), NULL);

	s = NULL;

	for (o = ((SPObject *) group)->children; o != NULL; o = o->next) {
		if (SP_IS_ITEM (o)) s = g_slist_prepend (s, o);
	}

	return g_slist_reverse (s);
}

/* fixme: Make SPObject method (Lauris) */

SPObject *
sp_item_group_get_child_by_name (SPGroup *group, SPObject *ref, const unsigned char *name)
{
	SPObject *child;
	child = (ref) ? ref->next : ((SPObject *) group)->children;
	while (child && strcmp (thera_node_get_name (child->node), name)) child = child->next;
	return child;
}

/* SPVPGroup */

static void sp_vpgroup_class_init (SPVPGroupClass *klass);
static void sp_vpgroup_init (SPVPGroup *vpgroup);

static void sp_vpgroup_build (SPObject *object, SPDocument *document, TheraNode *node);
static void sp_vpgroup_set (SPObject *object, unsigned int key, const unsigned char *value);
static void sp_vpgroup_update (SPObject *object, SPCtx *ctx, guint flags);

static SPGroupClass *vpgroup_parent_class;

GType
sp_vpgroup_get_type (void)
{
	static GType type = 0;
	if (!type) {
		GTypeInfo info = {
			sizeof (SPVPGroupClass),
			NULL, NULL,
			(GClassInitFunc) sp_vpgroup_class_init,
			NULL, NULL,
			sizeof (SPVPGroup),
			16,
			(GInstanceInitFunc) sp_vpgroup_init,
		};
		type = g_type_register_static (SP_TYPE_GROUP, "SPVPGroup", &info, 0);
	}
	return type;
}

static void
sp_vpgroup_class_init (SPVPGroupClass *klass)
{
	SPObjectClass *sp_object_class;
	SPItemClass *item_class;

	sp_object_class = (SPObjectClass *) klass;
	item_class = (SPItemClass *) klass;

	vpgroup_parent_class = g_type_class_peek_parent (klass);

	sp_object_class->build = sp_vpgroup_build;
	sp_object_class->set = sp_vpgroup_set;
	sp_object_class->update = sp_vpgroup_update;
}

static void
sp_vpgroup_init (SPVPGroup *vpgroup)
{
	SPItem *item;

	item = (SPItem *) vpgroup;

	item->has_viewport = 1;
	item->has_extra_transform = 1;

	sp_svg_length_unset (&vpgroup->x, SP_SVG_UNIT_NONE, 0.0, 0.0);
	sp_svg_length_unset (&vpgroup->height, SP_SVG_UNIT_NONE, 0.0, 0.0);
	sp_svg_length_unset (&vpgroup->width, SP_SVG_UNIT_PERCENT, 1.0, 1.0);
	sp_svg_length_unset (&vpgroup->height, SP_SVG_UNIT_PERCENT, 1.0, 1.0);

	nr_matrix_d_set_identity (&vpgroup->c2p);
}

static void
sp_vpgroup_build (SPObject *object, SPDocument *document, TheraNode *node)
{
	/* It is important to parse these here, so objects will have viewport build-time */
	sp_object_read_attr (object, "x");
	sp_object_read_attr (object, "y");
	sp_object_read_attr (object, "width");
	sp_object_read_attr (object, "height");
	sp_object_read_attr (object, "viewBox");
	sp_object_read_attr (object, "preserveAspectRatio");

	if (((SPObjectClass *) vpgroup_parent_class)->build)
		(* ((SPObjectClass *) vpgroup_parent_class)->build) (object, document, node);
}

static void
sp_vpgroup_set (SPObject *object, unsigned int key, const unsigned char *value)
{
	SPVPGroup *vpgroup;
	unsigned long unit;

	vpgroup = (SPVPGroup *) object;

	switch (key) {
	case SP_ATTR_X:
		if (sp_svg_length_read_uff (value, &unit, &vpgroup->x.value, &vpgroup->x.computed) &&
		    /* fixme: These are probably valid, but require special treatment (Lauris) */
		    (unit != SP_SVG_UNIT_EM) &&
		    (unit != SP_SVG_UNIT_EX)) {
			vpgroup->x.set = TRUE;
			vpgroup->x.unit = unit;
		} else {
			sp_svg_length_unset (&vpgroup->x, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		/* fixme: I am almost sure these do not require viewport flag (Lauris) */
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_VIEWPORT_MODIFIED_FLAG);
		break;
	case SP_ATTR_Y:
		if (sp_svg_length_read_uff (value, &unit, &vpgroup->y.value, &vpgroup->y.computed) &&
		    /* fixme: These are probably valid, but require special treatment (Lauris) */
		    (unit != SP_SVG_UNIT_EM) &&
		    (unit != SP_SVG_UNIT_EX)) {
			vpgroup->y.set = TRUE;
			vpgroup->y.unit = unit;
		} else {
			sp_svg_length_unset (&vpgroup->y, SP_SVG_UNIT_NONE, 0.0, 0.0);
		}
		/* fixme: I am almost sure these do not require viewport flag (Lauris) */
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_VIEWPORT_MODIFIED_FLAG);
		break;
	case SP_ATTR_WIDTH:
		if (sp_svg_length_read_uff (value, &unit, &vpgroup->width.value, &vpgroup->width.computed) &&
		    /* fixme: These are probably valid, but require special treatment (Lauris) */
		    (unit != SP_SVG_UNIT_EM) &&
		    (unit != SP_SVG_UNIT_EX) &&
		    (vpgroup->width.computed > 0.0)) {
			vpgroup->width.set = TRUE;
			vpgroup->width.unit = unit;
		} else {
			sp_svg_length_unset (&vpgroup->width, SP_SVG_UNIT_PERCENT, 1.0, 1.0);
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_VIEWPORT_MODIFIED_FLAG);
		break;
	case SP_ATTR_HEIGHT:
		if (sp_svg_length_read_uff (value, &unit, &vpgroup->height.value, &vpgroup->height.computed) &&
		    /* fixme: These are probably valid, but require special treatment (Lauris) */
		    (unit != SP_SVG_UNIT_EM) &&
		    (unit != SP_SVG_UNIT_EX) &&
		    (vpgroup->height.computed >= 0.0)) {
			vpgroup->height.set = TRUE;
			vpgroup->height.unit = unit;
		} else {
			sp_svg_length_unset (&vpgroup->height, SP_SVG_UNIT_PERCENT, 1.0, 1.0);
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_VIEWPORT_MODIFIED_FLAG);
		break;
	case SP_ATTR_VIEWBOX:
		if (sp_svg_viewbox_read (value, &vpgroup->viewBox)) {
			vpgroup->viewBox.set = 1;
		} else {
			vpgroup->viewBox.set = 0;
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_VIEWPORT_MODIFIED_FLAG);
		break;
	case SP_ATTR_PRESERVEASPECTRATIO:
		/* Do setup before, so we can use break to escape */
		vpgroup->aspect_set = FALSE;
		vpgroup->aspect_align = SP_ASPECT_XMID_YMID;
		vpgroup->aspect_clip = SP_ASPECT_MEET;
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_VIEWPORT_MODIFIED_FLAG);
		if (value) {
			int len;
			unsigned char c[256];
			const unsigned char *p, *e;
			unsigned int align, clip;
			p = value;
			while (*p && *p == 32) p += 1;
			if (!*p) break;
			e = p;
			while (*e && *e != 32) e += 1;
			len = e - p;
			if (len > 8) break;
			memcpy (c, value, len);
			c[len] = 0;
			/* Now the actual part */
			if (!strcmp (c, "none")) {
				align = SP_ASPECT_NONE;
			} else if (!strcmp (c, "xMinYMin")) {
				align = SP_ASPECT_XMIN_YMIN;
			} else if (!strcmp (c, "xMidYMin")) {
				align = SP_ASPECT_XMID_YMIN;
			} else if (!strcmp (c, "xMaxYMin")) {
				align = SP_ASPECT_XMAX_YMIN;
			} else if (!strcmp (c, "xMinYMid")) {
				align = SP_ASPECT_XMIN_YMID;
			} else if (!strcmp (c, "xMidYMid")) {
				align = SP_ASPECT_XMID_YMID;
			} else if (!strcmp (c, "xMaxYMin")) {
				align = SP_ASPECT_XMAX_YMID;
			} else if (!strcmp (c, "xMinYMax")) {
				align = SP_ASPECT_XMIN_YMAX;
			} else if (!strcmp (c, "xMidYMax")) {
				align = SP_ASPECT_XMID_YMAX;
			} else if (!strcmp (c, "xMaxYMax")) {
				align = SP_ASPECT_XMAX_YMAX;
			} else {
				break;
			}
			clip = SP_ASPECT_MEET;
			while (*e && *e == 32) e += 1;
			if (*e) {
				if (!strcmp (e, "meet")) {
					clip = SP_ASPECT_MEET;
				} else if (!strcmp (e, "slice")) {
					clip = SP_ASPECT_SLICE;
				} else {
					break;
				}
			}
			vpgroup->aspect_set = TRUE;
			vpgroup->aspect_align = align;
			vpgroup->aspect_clip = clip;
		}
		break;
	default:
		if (((SPObjectClass *) vpgroup_parent_class)->set)
			((SPObjectClass *) vpgroup_parent_class)->set (object, key, value);
		break;
	}
}

static void
sp_vpgroup_update (SPObject *object, SPCtx *ctx, guint flags)
{
	SPItem *item;
	SPVPGroup *vpgroup;
	SPItemCtx *ictx, rctx;
	SPItemView *v;

	item = (SPItem *) object;
	vpgroup = (SPVPGroup *) object;
	ictx = (SPItemCtx *) ctx;

	/* fixme: This will be invoked too often (Lauris) */
	/* fixme: We should calculate only if parent viewport has changed (Lauris) */
	/* If position is specified as percentage, calculate actual values */
	if (vpgroup->x.unit == SP_SVG_UNIT_PERCENT) {
		vpgroup->x.computed = vpgroup->x.value * (float) (ictx->vp.x1 - ictx->vp.x0);
	}
	if (vpgroup->y.unit == SP_SVG_UNIT_PERCENT) {
		vpgroup->y.computed = vpgroup->y.value * (float) (ictx->vp.y1 - ictx->vp.y0);
	}
	if (vpgroup->width.unit == SP_SVG_UNIT_PERCENT) {
		vpgroup->width.computed = vpgroup->width.value * (float) (ictx->vp.x1 - ictx->vp.x0);
	}
	if (vpgroup->height.unit == SP_SVG_UNIT_PERCENT) {
		vpgroup->height.computed = vpgroup->height.value * (float) (ictx->vp.y1 - ictx->vp.y0);
	}

#if 0
	g_print ("<svg> raw %g %g %g %g\n",
		 vpgroup->x.value, vpgroup->y.value,
		 vpgroup->width.value, vpgroup->height.value);

	g_print ("<svg> computed %g %g %g %g\n",
		 vpgroup->x.computed, vpgroup->y.computed,
		 vpgroup->width.computed, vpgroup->height.computed);
#endif

	/* Create copy of item context */
	rctx = *ictx;

	/* Calculate child to parent transformation */
	nr_matrix_d_set_identity (&vpgroup->c2p);

	if (object->parent) {
		/*
		 * fixme: I am not sure whether setting x and y does or does not
		 * fixme: translate the content of inner SVG.
		 * fixme: Still applying translation and setting viewport to width and
		 * fixme: height seems natural, as this makes the inner svg element
		 * fixme: self-contained. The spec is vague here.
		 */
		nr_matrix_d_set_translate (&vpgroup->c2p, vpgroup->x.computed, vpgroup->y.computed);
	}

	if (vpgroup->viewBox.set) {
		double x, y, width, height;
		NRMatrixD q;
		/* Determine actual viewbox in viewport coordinates */
		if (vpgroup->aspect_align == SP_ASPECT_NONE) {
			x = 0.0;
			y = 0.0;
			width = vpgroup->width.computed;
			height = vpgroup->height.computed;
		} else {
			double scalex, scaley, scale;
			/* Things are getting interesting */
			scalex = vpgroup->width.computed / (vpgroup->viewBox.x1 - vpgroup->viewBox.x0);
			scaley = vpgroup->height.computed / (vpgroup->viewBox.y1 - vpgroup->viewBox.y0);
			scale = (vpgroup->aspect_clip == SP_ASPECT_MEET) ? MIN (scalex, scaley) : MAX (scalex, scaley);
			width = (vpgroup->viewBox.x1 - vpgroup->viewBox.x0) * scale;
			height = (vpgroup->viewBox.y1 - vpgroup->viewBox.y0) * scale;
			/* Now place viewbox to requested position */
			switch (vpgroup->aspect_align) {
			case SP_ASPECT_XMIN_YMIN:
				x = 0.0;
				y = 0.0;
				break;
			case SP_ASPECT_XMID_YMIN:
				x = 0.5 * (vpgroup->width.computed - width);
				y = 0.0;
				break;
			case SP_ASPECT_XMAX_YMIN:
				x = 1.0 * (vpgroup->width.computed - width);
				y = 0.0;
				break;
			case SP_ASPECT_XMIN_YMID:
				x = 0.0;
				y = 0.5 * (vpgroup->height.computed - height);
				break;
			case SP_ASPECT_XMID_YMID:
				x = 0.5 * (vpgroup->width.computed - width);
				y = 0.5 * (vpgroup->height.computed - height);
				break;
			case SP_ASPECT_XMAX_YMID:
				x = 1.0 * (vpgroup->width.computed - width);
				y = 0.5 * (vpgroup->height.computed - height);
				break;
			case SP_ASPECT_XMIN_YMAX:
				x = 0.0;
				y = 1.0 * (vpgroup->height.computed - height);
				break;
			case SP_ASPECT_XMID_YMAX:
				x = 0.5 * (vpgroup->width.computed - width);
				y = 1.0 * (vpgroup->height.computed - height);
				break;
			case SP_ASPECT_XMAX_YMAX:
				x = 1.0 * (vpgroup->width.computed - width);
				y = 1.0 * (vpgroup->height.computed - height);
				break;
			default:
				x = 0.0;
				y = 0.0;
				break;
			}
		}
		/* Compose additional transformation from scale and position */
		q.c[0] = width / (vpgroup->viewBox.x1 - vpgroup->viewBox.x0);
		q.c[1] = 0.0;
		q.c[2] = 0.0;
		q.c[3] = height / (vpgroup->viewBox.y1 - vpgroup->viewBox.y0);
		q.c[4] = -vpgroup->viewBox.x0 * q.c[0] + x;
		q.c[5] = -vpgroup->viewBox.y0 * q.c[3] + y;
		/* Append viewbox transformation */
		nr_matrix_multiply_ddd (&vpgroup->c2p, &q, &vpgroup->c2p);
	}

	nr_matrix_multiply_ddd (&rctx.i2doc, &vpgroup->c2p, &rctx.i2doc);

	/* Initialize child viewport */
	if (vpgroup->viewBox.set) {
		rctx.vp.x0 = vpgroup->viewBox.x0;
		rctx.vp.y0 = vpgroup->viewBox.y0;
		rctx.vp.x1 = vpgroup->viewBox.x1;
		rctx.vp.y1 = vpgroup->viewBox.y1;
	} else {
		/* fixme: I wonder whether this logic is correct (Lauris) */
		if (object->parent) {
			rctx.vp.x0 = vpgroup->x.computed;
			rctx.vp.y0 = vpgroup->y.computed;
		} else {
			rctx.vp.x0 = 0.0;
			rctx.vp.y0 = 0.0;
		}
		rctx.vp.x1 = vpgroup->width.computed;
		rctx.vp.y1 = vpgroup->height.computed;
	}

	nr_matrix_d_set_identity (&rctx.i2vp);

	/* And invoke parent method */
	if (((SPObjectClass *) (vpgroup_parent_class))->update)
		((SPObjectClass *) (vpgroup_parent_class))->update (object, (SPCtx *) &rctx, flags);

	/* As last step set additional transform of arena group */
	for (v = item->display; v != NULL; v = v->view.next) {
		NRMatrixF vbf;
		nr_matrix_f_from_d (&vbf, &vpgroup->c2p);
		nr_arena_group_set_child_transform (NR_ARENA_GROUP (v), &vbf);
	}
}
