#define __SP_OBJECTGROUP_C__

/*
 * Abstract base class for non-item groups
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 1999-2003 Authors
 * Copyright (C) 2001-2002 Ximian, Inc.
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#include "factory.h"

#include "sp-object-group.h"

static void sp_objectgroup_class_init (SPObjectGroupClass *klass);
static void sp_objectgroup_init (SPObjectGroup *objectgroup);

static void sp_objectgroup_build (SPObject *object, SPDocument *document, TheraNode *node);
static void sp_objectgroup_release (SPObject *object);
static SPObject *sp_objectgroup_child_added (SPObject *object, TheraNode *child, TheraNode *ref);
static unsigned int sp_objectgroup_remove_child (SPObject *object, TheraNode *child);
static void sp_objectgroup_order_changed (SPObject *object, TheraNode *child, TheraNode *oldref, TheraNode *newref);
static unsigned int sp_objectgroup_sequence (SPObject *object, SPObject *target, unsigned int *seq);
static TheraNode *sp_objectgroup_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, unsigned int flags);

static SPObject *sp_objectgroup_get_le_child_by_repr (SPObjectGroup *group, TheraNode *ref);

static SPObjectClass *parent_class;

GType
sp_objectgroup_get_type (void)
{
	static GType objectgroup_type = 0;
	if (!objectgroup_type) {
		GTypeInfo objectgroup_info = {
			sizeof (SPObjectGroupClass),
			NULL,	/* base_init */
			NULL,	/* base_finalize */
			(GClassInitFunc) sp_objectgroup_class_init,
			NULL,	/* class_finalize */
			NULL,	/* class_data */
			sizeof (SPObjectGroup),
			16,	/* n_preallocs */
			(GInstanceInitFunc) sp_objectgroup_init,
		};
		objectgroup_type = g_type_register_static (SP_TYPE_OBJECT, "SPObjectGroup", &objectgroup_info, 0);
	}
	return objectgroup_type;
}

static void
sp_objectgroup_class_init (SPObjectGroupClass *klass)
{
	GObjectClass * object_class;
	SPObjectClass * sp_object_class;

	object_class = (GObjectClass *) klass;
	sp_object_class = (SPObjectClass *) klass;

	parent_class = g_type_class_ref (SP_TYPE_OBJECT);

	sp_object_class->build = sp_objectgroup_build;
	sp_object_class->release = sp_objectgroup_release;
	sp_object_class->child_added = sp_objectgroup_child_added;
	sp_object_class->remove_child = sp_objectgroup_remove_child;
	sp_object_class->order_changed = sp_objectgroup_order_changed;
	sp_object_class->sequence = sp_objectgroup_sequence;
	sp_object_class->write = sp_objectgroup_write;
}

static void
sp_objectgroup_init (SPObjectGroup *objectgroup)
{
	SPObject *object;
	object = SP_OBJECT(objectgroup);
	object->has_children = 1;
}

static void sp_objectgroup_build (SPObject *object, SPDocument *document, TheraNode *node)
{
#if 0
	SPObjectGroup *og;
	SPObject *last;
	TheraNode *rchild;

	og = SP_OBJECTGROUP (object);
#endif

	if (((SPObjectClass *) (parent_class))->build)
		(* ((SPObjectClass *) (parent_class))->build) (object, document, node);

#if 0
	last = NULL;
	for (rchild = thera_node_get_first_child (node); rchild != NULL; rchild = thera_node_get_next_sibling (rchild)) {
		GType type;
		SPObject *child;
		type = sp_node_type_lookup (rchild);
		if (g_type_is_a (type, SP_TYPE_OBJECT)) {
			child = g_object_new (type, 0);
			if (last) {
				last->next = sp_object_attach_reref (object, child, NULL);
			} else {
				object->children = sp_object_attach_reref (object, child, NULL);
			}
			sp_object_invoke_build (child, document, rchild, SP_OBJECT_IS_CLONED (object));
			last = child;
		}
	}
#endif
}

static void
sp_objectgroup_release (SPObject *object)
{
#if 0
	SPObjectGroup *og;

	og = SP_OBJECTGROUP (object);

	while (object->children) {
		object->children = sp_object_detach_unref (object, object->children);
	}
#endif

	if (((SPObjectClass *) parent_class)->release)
		((SPObjectClass *) parent_class)->release (object);
}

static SPObject *
sp_objectgroup_child_added (SPObject *object, TheraNode *thechild, TheraNode *theref)
{
	SPObject *child;
#if 0
	SPObjectGroup * og;
	GType type;

	og = SP_OBJECTGROUP (object);
#endif

	child = NULL;
	if (((SPObjectClass *) (parent_class))->child_added)
		child = ((SPObjectClass *) (parent_class))->child_added (object, thechild, theref);

#if 0
	type = sp_node_type_lookup (child);
	if (g_type_is_a (type, SP_TYPE_OBJECT)) {
		SPObject *ochild, *prev;
		ochild = g_object_new (type, 0);
		prev = sp_objectgroup_get_le_child_by_repr (og, ref);
		if (prev) {
			prev->next = sp_object_attach_reref (object, ochild, prev->next);
		} else {
			object->children = sp_object_attach_reref (object, ochild, object->children);
		}
		sp_object_invoke_build (ochild, object->document, child, SP_OBJECT_IS_CLONED (object));
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
	}
#endif
	return child;
}

static unsigned int
sp_objectgroup_remove_child (SPObject *object, TheraNode *child)
{
#if 0
	SPObjectGroup *og;
	SPObject *ref, *oc;

	og = SP_OBJECTGROUP (object);
#endif

	if (((SPObjectClass *) (parent_class))->remove_child) {
		unsigned int ret;
		ret = ((SPObjectClass *) (parent_class))->remove_child (object, child);
		if (!ret) return ret;
	}

#if 0
	ref = NULL;
	oc = object->children;
	while (oc && (oc->node != child)) {
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
sp_objectgroup_order_changed (SPObject *object, TheraNode *child, TheraNode *oldref, TheraNode *newref)
{
#if 0
	SPObjectGroup *og;
	SPObject *ochild, *oold, *onew;

	og = SP_OBJECTGROUP (object);
#endif

	if (((SPObjectClass *) (parent_class))->order_changed)
		(* ((SPObjectClass *) (parent_class))->order_changed) (object, child, oldref, newref);

#if 0
	ochild = sp_objectgroup_get_le_child_by_repr (og, child);
	oold = sp_objectgroup_get_le_child_by_repr (og, oldref);
	onew = sp_objectgroup_get_le_child_by_repr (og, newref);

	if (ochild && (oold != onew)) {
		if (oold) {
			oold->next = sp_object_detach (object, ochild);
		} else {
			object->children = sp_object_detach (object, ochild);
		}
		if (onew) {
			onew->next = sp_object_attach_reref (object, ochild, onew->next);
		} else {
			object->children = sp_object_attach_reref (object, ochild, object->children);
		}
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
	}
#endif
}

static unsigned int
sp_objectgroup_sequence (SPObject *object, SPObject *target, unsigned int *seq)
{
	SPObjectGroup *og;
	SPObject *child;

	og = (SPObjectGroup *) object;

	for (child = object->children; child != NULL; child = child->next) {
		*seq += 1;
		if (sp_object_invoke_sequence (child, target, seq)) return TRUE;
	}

	return FALSE;
}

static TheraNode *
sp_objectgroup_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags)
{
	SPObjectGroup *group;
	SPObject *child;
	TheraNode *childnode;

	group = SP_OBJECTGROUP (object);

	if (flags & SP_OBJECT_WRITE_BUILD) {
		GSList *l;
		if (!node) node = thera_document_new_node (thedoc, THERA_NODE_ELEMENT, "g");
		l = NULL;
		for (child = object->children; child != NULL; child = child->next) {
			childnode = sp_object_invoke_write (child, thedoc, NULL, flags);
			if (childnode) l = g_slist_prepend (l, childnode);
		}
		while (l) {
			thera_node_add_child (node, (TheraNode *) l->data, NULL);
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

static SPObject *
sp_objectgroup_get_le_child_by_repr (SPObjectGroup *og, TheraNode *ref)
{
	SPObject *o, *oc;
	TheraNode *r, *rc;

	if (!ref) return NULL;

	o = NULL;
	r = ((SPObject *) og)->node;
	rc = thera_node_get_first_child (r);
	oc = ((SPObject *) og)->children;

	while (rc) {
		if (rc == ref) return o;
		if (oc && (oc->node == rc)) {
			/* Rewing object */
			o = oc;
			oc = oc->next;
		}
		/* Rewind repr */
		rc = thera_node_get_next_sibling (rc);
	}

	g_assert_not_reached ();

	return NULL;
}
