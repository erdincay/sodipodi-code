#define __SP_DEFS_C__

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

/*
 * fixme: We should really check childrens validity - currently everything
 * flips in
 */

#include "factory.h"

#include "sp-defs.h"

static void sp_defs_class_init (SPDefsClass * klass);
static void sp_defs_init (SPDefs * defs);

static void sp_defs_build (SPObject *object, SPDocument * document, TheraNode * node);
static void sp_defs_release (SPObject *object);
static SPObject *sp_defs_child_added (SPObject * object, TheraNode * child, TheraNode * ref);
static unsigned int sp_defs_remove_child (SPObject * object, TheraNode * child);
static void sp_defs_order_changed (SPObject * object, TheraNode * child, TheraNode * old, TheraNode * new);
static void sp_defs_update (SPObject *object, SPCtx *ctx, guint flags);
static void sp_defs_modified (SPObject *object, guint flags);
static unsigned int sp_defs_sequence (SPObject *object, SPObject *target, unsigned int *seq);
static TheraNode *sp_defs_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags);

static SPObject * sp_defs_get_child_by_repr (SPDefs * defs, TheraNode * node);

static SPObjectClass * parent_class;

GType
sp_defs_get_type (void)
{
	static GType defs_type = 0;
	if (!defs_type) {
		GTypeInfo defs_info = {
			sizeof (SPDefsClass),
			NULL,	/* base_init */
			NULL,	/* base_finalize */
			(GClassInitFunc) sp_defs_class_init,
			NULL,	/* class_finalize */
			NULL,	/* class_data */
			sizeof (SPDefs),
			16,	/* n_preallocs */
			(GInstanceInitFunc) sp_defs_init,
		};
		defs_type = g_type_register_static (SP_TYPE_OBJECT, "SPDefs", &defs_info, 0);
	}
	return defs_type;
}

static void
sp_defs_class_init (SPDefsClass * klass)
{
	GObjectClass * object_class;
	SPObjectClass * sp_object_class;

	object_class = (GObjectClass *) klass;
	sp_object_class = (SPObjectClass *) klass;

	parent_class = g_type_class_ref (SP_TYPE_OBJECT);

	sp_object_class->build = sp_defs_build;
	sp_object_class->release = sp_defs_release;
	sp_object_class->child_added = sp_defs_child_added;
	sp_object_class->remove_child = sp_defs_remove_child;
	sp_object_class->order_changed = sp_defs_order_changed;
	sp_object_class->update = sp_defs_update;
	sp_object_class->modified = sp_defs_modified;
	sp_object_class->sequence = sp_defs_sequence;
	sp_object_class->write = sp_defs_write;
}

static void
sp_defs_init (SPDefs *defs)
{
	SPObject *object;
	object = SP_OBJECT(defs);
	object->has_children = 1;
}

static void sp_defs_build (SPObject *object, SPDocument *doc, TheraNode * node)
{
#if 0
	SPDefs *defs;
	SPObject *last;
	TheraNode *rchild;

	defs = SP_DEFS (object);
#endif
	if (((SPObjectClass *) (parent_class))->build)
		(* ((SPObjectClass *) (parent_class))->build) (object, doc, node);
#if 0
	last = NULL;
	for (rchild = thera_node_get_first_child (node); rchild != NULL; rchild = thera_node_get_next_sibling (rchild)) {
		GType type;
		SPObject *child;
		type = sp_node_type_lookup (rchild);
		child = g_object_new (type, 0);
		if (last) {
			last->next = sp_object_attach_reref (object, child, NULL);
		} else {
			object->children = sp_object_attach_reref (object, child, NULL);
		}
		sp_object_invoke_build (child, doc, rchild, object, last, SP_OBJECT_IS_CLONED (object));
		last = child;
	}
#endif
}

static void
sp_defs_release (SPObject * object)
{
#if 0
	SPDefs *defs;

	defs = (SPDefs *) object;

	while (object->children) {
		object->children = sp_object_detach_unref (SP_OBJECT (defs), object->children);
	}
#endif
	if (((SPObjectClass *) (parent_class))->release)
		((SPObjectClass *) (parent_class))->release (object);
}

static SPObject *
sp_defs_child_added (SPObject *object, TheraNode *thechild, TheraNode *ref)
{
	SPObject *child;
#if 0
	SPDefs * defs;
	SPObject * ochild, * prev;
	GType type;

	defs = SP_DEFS (object);
#endif

	child = NULL;
	if (((SPObjectClass *) (parent_class))->child_added)
		child =((SPObjectClass *) (parent_class))->child_added (object, thechild, ref);
#if 0
	type = sp_node_type_lookup (child);
	ochild = g_object_new (type, 0);
	ochild->parent = object;

	prev = sp_defs_get_child_by_repr (defs, ref);

	if (!prev) {
		ochild->next = object->children;
		object->children = ochild;
	} else {
		ochild->next = prev->next;
		prev->next = ochild;
	}

	sp_object_invoke_build (ochild, object->document, child, object, last, SP_OBJECT_IS_CLONED (object));
#endif
	return child;
}

static unsigned int
sp_defs_remove_child (SPObject * object, TheraNode * child)
{
#if 0
	SPDefs *defs;
	SPObject *ref, *oc;

	defs = SP_DEFS (object);

#endif
	if (((SPObjectClass *) (parent_class))->remove_child) {
		unsigned int ret;
		ret = ((SPObjectClass *) (parent_class))->remove_child (object, child);
		if (!ret) return ret;
	}
#if 0
	ref = NULL;
	oc = object->children;
	while (oc && (SP_OBJECT_REPR (oc) != child)) {
		ref = oc;
		oc = oc->next;
	}

	if (oc) {
		if (ref) {
			ref->next = sp_object_detach_unref (object, oc);
		} else {
			object->children = sp_object_detach_unref (object, oc);
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
	}
#endif
	return TRUE;
}

static void
sp_defs_order_changed (SPObject * object, TheraNode * child, TheraNode * old, TheraNode * new)
{
#if 0
	SPDefs * defs;
	SPObject * ochild, * oold, * onew;

	defs = SP_DEFS (object);
#endif
	if (((SPObjectClass *) (parent_class))->order_changed)
		(* ((SPObjectClass *) (parent_class))->order_changed) (object, child, old, new);
#if 0
	ochild = sp_defs_get_child_by_repr (defs, child);
	oold = sp_defs_get_child_by_repr (defs, old);
	onew = sp_defs_get_child_by_repr (defs, new);

	if (oold) {
		oold->next = ochild->next;
	} else {
		object->children = ochild->next;
	}
	if (onew) {
		ochild->next = onew->next;
		onew->next = ochild;
	} else {
		ochild->next = object->children;
		object->children = ochild;
	}
#endif
}

static void
sp_defs_update (SPObject *object, SPCtx *ctx, guint flags)
{
	SPDefs *defs;
	SPObject *child;
	GSList *l;

	defs = SP_DEFS (object);

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
		if (flags || (child->uflags & SP_OBJECT_MODIFIED_FLAG)) {
			sp_object_invoke_update (child, ctx, flags);
		}
		g_object_unref (G_OBJECT (child));
	}
}

static void
sp_defs_modified (SPObject *object, guint flags)
{
	SPDefs *defs;
	SPObject *child;
	GSList *l;

	defs = SP_DEFS (object);

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
sp_defs_sequence (SPObject *object, SPObject *target, unsigned int *seq)
{
	SPDefs *defs;
	SPObject *child;

	defs = SP_DEFS (object);

	for (child = object->children; child; child = child->next) {
		*seq += 1;
		if (sp_object_invoke_sequence (child, target, seq)) return TRUE;
	}

	return FALSE;
}

static TheraNode *
sp_defs_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags)
{
	SPDefs *defs;
	SPObject *child;
	TheraNode *crepr;


	defs = SP_DEFS (object);

	if (flags & SP_OBJECT_WRITE_BUILD) {
		GSList *l;
		if (!node) node = thera_document_new_node (thedoc, THERA_NODE_ELEMENT, "defs");
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
		(* ((SPObjectClass *) (parent_class))->write) (object, thedoc, node, flags);

	return node;
}

static SPObject *
sp_defs_get_child_by_repr (SPDefs * defs, TheraNode * node)
{
	SPObject * o;

	if (!node) return NULL;

	o = ((SPObject *) defs)->children;
	while (o->node != node) o = o->next;

	return o;
}


