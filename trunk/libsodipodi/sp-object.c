#define __SP_OBJECT_C__
/*
 * Abstract base class for all nodes
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 1999-2002 authors
 * Copyright (C) 2001-2002 Ximian, Inc.
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#include <stdlib.h>
#include <string.h>

#include "marshal.h"
#include "macros.h"
#include "document.h"
#include "factory.h"
#include "attributes.h"
#include "style.h"

#include "sp-object.h"

#define noSP_OBJECT_DEBUG
#define noSP_OBJECT_DEBUG_CASCADE

static void sp_object_class_init (SPObjectClass * klass);
static void sp_object_init (SPObject * object);
static void sp_object_finalize (GObject * object);

static void sp_object_build (SPObject *object, SPDocument *document, TheraNode* node);
static void sp_object_release (SPObject *object);
static SPObject *sp_object_child_added (SPObject *object, TheraNode *child, TheraNode *ref);
static unsigned int sp_object_remove_child (SPObject *object, TheraNode *child);
static void sp_object_order_changed (SPObject *object, TheraNode *child, TheraNode *oldref, TheraNode *newref);

static void sp_object_private_set (SPObject *object, unsigned int key, const unsigned char *value);
static TheraNode *sp_object_private_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, guint flags);

/* Real handlers of TheraNode signals */
static unsigned int sp_object_node_change_attr (TheraNode *node, const char *key, const char *oldval, const char *newval, void *data);
static void sp_object_node_attr_changed (TheraNode *node, const char *key, const char *oldval, const char *newval, void *data);
static void sp_object_node_content_changed (TheraNode *repr, const char *oldcontent, const char *newcontent, void *data);
static void sp_object_node_child_added (TheraNode *node, TheraNode *child, TheraNode *ref, void *data);
static unsigned int sp_object_node_remove_child (TheraNode *node, TheraNode *child, TheraNode *ref, void *data);
static void sp_object_node_child_removed (TheraNode *node, TheraNode *child, TheraNode *ref, void *data);
static void sp_object_node_order_changed (TheraNode *node, TheraNode *child, TheraNode *oldref, TheraNode *newref, void *data);

static gchar *sp_object_get_unique_id (SPObject *object, const char *defid);

enum {RELEASE, MODIFIED, LAST_SIGNAL};

TheraNodeEventVector object_event_vector = {
	NULL, /* Destroy */
	NULL, /* Add child */
	sp_object_node_child_added,
	sp_object_node_remove_child,
	sp_object_node_child_removed,
	sp_object_node_change_attr,
	sp_object_node_attr_changed,
	NULL, /* Change content */
	sp_object_node_content_changed,
	NULL, /* change_order */
	sp_object_node_order_changed
};

static GObjectClass *parent_class;
static guint object_signals[LAST_SIGNAL] = {0};

GType
sp_object_get_type (void)
{
	static GType type = 0;
	if (!type) {
		GTypeInfo info = {
			sizeof (SPObjectClass),
			NULL, NULL,
			(GClassInitFunc) sp_object_class_init,
			NULL, NULL,
			sizeof (SPObject),
			16,
			(GInstanceInitFunc) sp_object_init,
		};
		type = g_type_register_static (G_TYPE_OBJECT, "SPObject", &info, 0);
	}
	return type;
}

static void
sp_object_class_init (SPObjectClass * klass)
{
	GObjectClass * object_class;

	object_class = (GObjectClass *) klass;

	parent_class = g_type_class_ref (G_TYPE_OBJECT);

	object_signals[RELEASE] =  g_signal_new ("release",
						 G_TYPE_FROM_CLASS (klass),
						 G_SIGNAL_RUN_CLEANUP | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
						 G_STRUCT_OFFSET (SPObjectClass, release),
						 NULL, NULL,
						 sp_marshal_VOID__VOID,
						 G_TYPE_NONE, 0);
	object_signals[MODIFIED] = g_signal_new ("modified",
                                                 G_TYPE_FROM_CLASS (klass),
                                                 G_SIGNAL_RUN_FIRST,
                                                 G_STRUCT_OFFSET (SPObjectClass, modified),
                                                 NULL, NULL,
                                                 sp_marshal_NONE__UINT,
                                                 G_TYPE_NONE, 1, G_TYPE_UINT);

	object_class->finalize = sp_object_finalize;

	klass->build = sp_object_build;
	klass->release = sp_object_release;
	klass->child_added = sp_object_child_added;
	klass->remove_child = sp_object_remove_child;
	klass->order_changed = sp_object_order_changed;

	klass->set = sp_object_private_set;
	klass->write = sp_object_private_write;
}

static void
sp_object_init (SPObject * object)
{
#ifdef SP_OBJECT_DEBUG
	g_print("sp_object_init: id=%x, typename=%s\n",
		object, g_type_name_from_instance((GTypeInstance*)object));
#endif
}

static void
sp_object_finalize (GObject * object)
{
	((GObjectClass *) (parent_class))->finalize (object);
}

/*
 * Refcounting
 *
 * Owner is here for debug reasons, you can set it to NULL safely
 * Ref should return object, NULL is error, unref return always NULL
 */

SPObject *
sp_object_ref (SPObject *object, SPObject *owner)
{
	g_return_val_if_fail (object != NULL, NULL);
	g_return_val_if_fail (SP_IS_OBJECT (object), NULL);
	g_return_val_if_fail (!owner || SP_IS_OBJECT (owner), NULL);

	g_object_ref (G_OBJECT (object));

	return object;
}

SPObject *
sp_object_unref (SPObject *object, SPObject *owner)
{
	g_return_val_if_fail (object != NULL, NULL);
	g_return_val_if_fail (SP_IS_OBJECT (object), NULL);
	g_return_val_if_fail (!owner || SP_IS_OBJECT (owner), NULL);

	g_object_unref (G_OBJECT (object));

	return NULL;
}

SPObject *
sp_object_href (SPObject *object, gpointer owner)
{
	g_return_val_if_fail (object != NULL, NULL);
	g_return_val_if_fail (SP_IS_OBJECT (object), NULL);

	object->hrefcount++;

	return object;
}

SPObject *
sp_object_hunref (SPObject *object, gpointer owner)
{
	g_return_val_if_fail (object != NULL, NULL);
	g_return_val_if_fail (SP_IS_OBJECT (object), NULL);
	g_return_val_if_fail (object->hrefcount > 0, NULL);

	object->hrefcount--;

	return NULL;
}

static void
sp_object_attached_object_release (SPObject *object, SPObject *client)
{
	if (((SPObjectClass *) G_OBJECT_GET_CLASS (client))->attached_object_release)
		((SPObjectClass *) G_OBJECT_GET_CLASS (client))->attached_object_release (client, object);
}

static void
sp_object_attached_object_modified (SPObject *object, unsigned int flags, SPObject *client)
{
	if (((SPObjectClass *) G_OBJECT_GET_CLASS (client))->attached_object_modified)
		((SPObjectClass *) G_OBJECT_GET_CLASS (client))->attached_object_modified (client, object, flags);
}

void
sp_object_attach (SPObject *object, SPObject *client)
{
	g_return_if_fail (object != NULL);
	g_return_if_fail (SP_IS_OBJECT (object));
	g_return_if_fail (client != NULL);
	g_return_if_fail (SP_IS_OBJECT (client));

	object->hrefcount += 1;

	g_signal_connect (G_OBJECT(object), "release", G_CALLBACK(sp_object_attached_object_release), client);
	g_signal_connect (G_OBJECT(object), "modified", G_CALLBACK(sp_object_attached_object_modified), client);
}

void
sp_object_detach (SPObject *object, SPObject *client)
{
	g_return_if_fail (object != NULL);
	g_return_if_fail (SP_IS_OBJECT (object));
	g_return_if_fail (client != NULL);
	g_return_if_fail (SP_IS_OBJECT (client));

	object->hrefcount -= 1;

	sp_signal_disconnect_by_data (G_OBJECT(object), client);
}

/*
 * SPObject specific build method
 */

static void
sp_object_build (SPObject *object, SPDocument *doc, TheraNode *node)
{
	/* Nothing specific here */
#ifdef SP_OBJECT_DEBUG
	g_print("sp_object_build: id=%x, typename=%s\n", object, g_type_name_from_instance((GTypeInstance*)object));
#endif

	if (object->has_children) {
		TheraNode *thechild;
		SPObject *ref;

		ref = NULL;
		for (thechild = thera_node_get_first_child (node); thechild; thechild = thera_node_get_next_sibling (thechild)) {
			SPObject *child;
			GType type;
			type = sp_node_type_lookup (thechild);
			if (g_type_is_a (type, SP_TYPE_OBJECT)) {
				child = g_object_new (type, 0);
				sp_object_ref (child, object);
				if (ref) {
					child->next = ref->next;
					ref->next = child;
				} else {
					child->next = object->children;
					object->children = child;
				}
				g_object_unref (G_OBJECT (child));
				child->parent = object;
				sp_object_invoke_build (child, doc, thechild, object, ref, SP_OBJECT_IS_CLONED (object));
				ref = child;
			}
		}
	}

	sp_object_read_attribute (object, SP_ATTR_XML_SPACE);
}

void
sp_object_invoke_build (SPObject *object, SPDocument *doc, TheraNode *node, SPObject *parent, SPObject *ref, unsigned int cloned)
{
#ifdef SP_OBJECT_DEBUG
	g_print("sp_object_invoke_build: id=%x, typename=%s\n", object, g_type_name_from_instance((GTypeInstance*)object));
#endif
	g_assert (object != NULL);
	g_assert (SP_IS_OBJECT (object));
	g_assert (doc != NULL);
	g_assert (SP_IS_DOCUMENT (doc));
	g_assert (node != NULL);

	g_assert (object->document == NULL);
	g_assert (object->node == NULL);
	g_assert (object->id == NULL);

	/* Bookkeeping */

	object->document = doc;
	object->node = node;
	object->cloned = cloned;

	/* If we are not cloned, assign unique id, otherwise id is NULL */
	if (!SP_OBJECT_IS_CLONED (object)) {
		const gchar * id;
		gchar *realid;

		id = thera_node_get_attribute (object->node, "id");
		realid = sp_object_get_unique_id (object, id);
		g_assert (realid != NULL);

		sp_document_def_id (object->document, realid, object);
		object->id = realid;

		/* Redefine ID, if required */
		if (!id || strcmp (id, realid)) {
			if (!thera_node_set_attribute (object->node, "id", realid)) {
				g_error ("Cannot change id %s -> %s - probably there is stale ref", id, realid);
			}
		}
	}

	/* emit ::object_added signal before children are created */
	sp_document_invoke_object_added (doc, object, parent, ref);

	/* Invoke derived methods, if any */
	if (((SPObjectClass *) G_OBJECT_GET_CLASS (object))->build)
		((SPObjectClass *) G_OBJECT_GET_CLASS (object))->build (object, object->document, object->node);

	/* Inform document about new identity */
	if (object->id) sp_document_invoke_identity_added (object->document, object->id, object);

	/* Signalling (should be connected AFTER processing derived methods */
	thera_node_add_listener (object->node, &object_event_vector, object);
}

static void
sp_object_release (SPObject *object)
{
	if (object->has_children) {
		while (object->children) {
			SPObject *child;
			child = object->children;
			object->children = child->next;
			child->parent = NULL;
			child->next = NULL;
			sp_object_invoke_release (child, object, NULL);
			sp_object_unref (child, object);
		}
	}
}

void
sp_object_invoke_release (SPObject *object, SPObject *parent, SPObject *ref)
{
	g_assert (object != NULL);
	g_assert (SP_IS_OBJECT (object));

	/* Parent refcount us, so there shouldn't be any */
	g_assert (!object->parent);
	g_assert (!object->next);
	g_assert (object->document);
	g_assert (object->node);

	/* This invokes virtual method as well */
	g_signal_emit (G_OBJECT (object), object_signals[RELEASE], 0);

	/* href holders HAVE to release it in signal handler */
	g_assert (object->hrefcount == 0);

	/* Inform document about loss of identity */
	if (object->id) sp_document_invoke_identity_removed (object->document, object->id, object);

	/* Emit ::object_removed signal before actual destruction */
	sp_document_invoke_object_removed (object->document, object, parent, ref);

	if (object->style) {
		object->style = sp_style_unref (object->style);
	}

	if (!SP_OBJECT_IS_CLONED (object)) {
		g_assert (object->id);
		sp_document_undef_id (object->document, object->id);
		g_free (object->id);
		object->id = NULL;
	} else {
		g_assert (!object->id);
	}

	thera_node_remove_listener (object->node, object);

	object->document = NULL;
	object->node = NULL;
}

static void
sp_object_node_child_added (TheraNode *node, TheraNode *child, TheraNode *ref, void *data)
{
	SPObject * object; 
	object = SP_OBJECT (data);
	if (((SPObjectClass *) G_OBJECT_GET_CLASS (object))->child_added)
		((SPObjectClass *) G_OBJECT_GET_CLASS (object))->child_added (object, child, ref);
	object->document->modified = 1;
}

static SPObject *
sp_object_child_added (SPObject *object, TheraNode *child, TheraNode *ref)
{
	SPObject *ochild;

	ochild = NULL;
	if (object->has_children) {
		GType type;
		type = sp_node_type_lookup (child);
		if (g_type_is_a (type, SP_TYPE_OBJECT)) {
			SPObject *objref;
			ochild = g_object_new (type, 0);
			objref = sp_object_get_ref_from_thera_child (object, child);
			sp_object_ref (ochild, object);
			if (objref) {
				ochild->next = objref->next;
				objref->next = ochild;
			} else {
				ochild->next = object->children;
				object->children = ochild;
			}
			g_object_unref (G_OBJECT (ochild));
			ochild->parent = object;
			sp_object_invoke_build (ochild, object->document, child, object, objref, SP_OBJECT_IS_CLONED (object));
			sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
		}
	}

	return ochild;
}


static unsigned int
sp_object_node_remove_child (TheraNode *node, TheraNode *child, TheraNode *ref, void *data)
{
	SPObject *object, *ochild;
	const unsigned char *id;
	object = SP_OBJECT (data);
	id = thera_node_get_attribute (child, "id");
	ochild = sp_document_lookup_id (object->document, id);
	if (ochild->blocked) return FALSE;
	if (((SPObjectClass *) G_OBJECT_GET_CLASS (object))->remove_child)
		return ((SPObjectClass *)G_OBJECT_GET_CLASS (object))->remove_child (object, child);
	return TRUE;
}

static unsigned int
sp_object_remove_child (SPObject *object, TheraNode *child)
{
	SPObject *objref, *objchild;

	objref = NULL;
	objchild = object->children;
	while (objchild && (objchild->node != child)) {
		objref = objchild;
		objchild = objchild->next;
	}

	if (objchild) {
		if (objref) {
			objref->next = objchild->next;
		} else {
			object->children = objchild->next;
		}
		objchild->parent = NULL;
		objchild->next = NULL;
		sp_object_invoke_release (objchild, object, objref);
		sp_object_unref (objchild, object);
		sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
	}

	return TRUE;
}

static void
sp_object_node_child_removed (TheraNode *node, TheraNode *child, TheraNode *ref, void *data)
{
	SPObject * object; 
	object = SP_OBJECT (data);
	object->document->modified = 1;
}

/* fixme: */

static void
sp_object_node_order_changed (TheraNode *node, TheraNode *child, TheraNode *oldref, TheraNode *newref, void *data)
{
	SPObject * object;
	object = SP_OBJECT (data);
	if (((SPObjectClass *) G_OBJECT_GET_CLASS(object))->order_changed)
		((SPObjectClass *) G_OBJECT_GET_CLASS(object))->order_changed (object, child, oldref, newref);
	object->document->modified = 1;
}

static void
sp_object_order_changed (SPObject *object, TheraNode *child, TheraNode *oldref, TheraNode *newref)
{
	SPObject *objchild, *oldobjref, *newobjref;

	/* ochild = sp_object_get_ref_from_thera_child (object, child); */
	for (objchild = object->children; objchild; objchild = objchild->next) {
		if (objchild->node == child) break;
	}
	if (!objchild) return;
	oldobjref = sp_object_get_ref_from_thera_ref (object, oldref, objchild);
	newobjref = sp_object_get_ref_from_thera_ref (object, newref, objchild);
	if (newobjref == oldobjref) return;

	if (oldobjref) {
		oldobjref->next = objchild->next;
	} else {
		object->children = objchild->next;
	}
	objchild->next = NULL;
	sp_object_ref (objchild, object);
	g_object_unref (G_OBJECT (objchild));
	if (newobjref) {
		objchild->next = newobjref->next;
		newobjref->next = objchild;
	} else {
		objchild->next = object->children;
		object->children = objchild;
	}
	/* fixme: Is this good place to emit ::order_changed? (Lauris) */
	sp_document_invoke_object_relocated (object->document, objchild, object, oldobjref, newobjref);
	sp_object_request_modified (object, SP_OBJECT_MODIFIED_FLAG);
}

static void
sp_object_private_set (SPObject *object, unsigned int key, const unsigned char *value)
{
	g_assert (SP_IS_DOCUMENT (object->document));
	/* fixme: rething that cloning issue */
	g_assert (SP_OBJECT_IS_CLONED (object) || object->id != NULL);
	g_assert (key != SP_ATTR_INVALID);

	switch (key) {
	case SP_ATTR_ID:
		if (!SP_OBJECT_IS_CLONED (object)) {
			g_assert (value != NULL);
			g_assert (strcmp (value, object->id));
			g_assert (!sp_document_lookup_id (object->document, value));
			sp_document_undef_id (object->document, object->id);
			sp_document_invoke_identity_removed (object->document, object->id, object);
			g_free (object->id);
			object->id = g_strdup (value);
			sp_document_def_id (object->document, object->id, object);
			sp_document_invoke_identity_added (object->document, object->id, object);
		} else {
			g_warning ("ID of cloned object changed, so document is out of sync");
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG);
		break;
	case SP_ATTR_XML_SPACE:
		if (value && !strcmp (value, "preserve")) {
			object->xml_space.value = SP_XML_SPACE_PRESERVE;
			object->xml_space.set = TRUE;
		} else if (value && !strcmp (value, "default")) {
			object->xml_space.value = SP_XML_SPACE_DEFAULT;
			object->xml_space.set = TRUE;
		} else {
			/* unset value */
			object->xml_space.value = object->parent ? object->parent->xml_space.value : SP_XML_SPACE_DEFAULT;
			object->xml_space.set = FALSE;
		}
		sp_object_request_update (object, SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_STYLE_MODIFIED_FLAG);
		break;
	}
}

static void
sp_object_invoke_set (SPObject *object, unsigned int key, const unsigned char *value)
{
	g_assert (object != NULL);
	g_assert (SP_IS_OBJECT (object));

	if (((SPObjectClass *) G_OBJECT_GET_CLASS (object))->set)
		((SPObjectClass *) G_OBJECT_GET_CLASS(object))->set (object, key, value);
}

void
sp_object_read_attribute (SPObject *object, unsigned int key)
{
	const unsigned char *keystr, *value;

	g_return_if_fail (object != NULL);
	g_return_if_fail (SP_IS_OBJECT (object));

	keystr = sp_attribute_name (key);
	g_return_if_fail (keystr != NULL);

	value = thera_node_get_attribute (object->node, keystr);
	sp_object_invoke_set (object, key, value);
}

#if 1
void
sp_object_read_attr (SPObject *object, const char *key)
{
	unsigned int keyid;

	g_assert (object != NULL);
	g_assert (SP_IS_OBJECT (object));
	g_assert (key != NULL);

	g_assert (SP_IS_DOCUMENT (object->document));
	g_assert (object->node != NULL);
	/* fixme: rething that cloning issue */
	g_assert (SP_OBJECT_IS_CLONED (object) || object->id != NULL);

	keyid = sp_attribute_lookup (key);
	if (keyid != SP_ATTR_INVALID) {
		const unsigned char *value;
		value = thera_node_get_attribute (object->node, key);
		sp_object_invoke_set (object, keyid, value);
	}
}
#endif

static unsigned int
sp_object_node_change_attr (TheraNode *node, const char *key, const char *oldval, const char *newval, void *data)
{
	SPObject *object;
	gpointer defid;

	object = SP_OBJECT (data);

	if (strcmp (key, "id") == 0) {
		if (!newval) return FALSE;
		defid = sp_document_lookup_id (object->document, newval);
		if (defid == object) return TRUE;
		if (defid) return FALSE;
	}

	return TRUE;
}

static void
sp_object_node_attr_changed (TheraNode *node, const char *key, const char *oldval, const char *newval, void *data)
{
	SPObject *object;
	unsigned int keyid;
	object = SP_OBJECT (data);
	keyid = sp_attribute_lookup (key);
	if (keyid != SP_ATTR_INVALID) {
		sp_object_invoke_set (object, keyid, newval);
	}
	object->document->modified = 1;
}

static void
sp_object_node_content_changed (TheraNode *node, const char *oldcontent, const char *newcontent, void *data)
{
	SPObject * object;
	object = SP_OBJECT (data);
	if (((SPObjectClass *) G_OBJECT_GET_CLASS (object))->read_content)
		((SPObjectClass *) G_OBJECT_GET_CLASS (object))->read_content (object);
	object->document->modified = 1;
}

void
sp_object_invoke_identity_added (SPObject *object, const unsigned char *id, SPObject *other)
{
	if (((SPObjectClass *) G_OBJECT_GET_CLASS(object))->identity_added)
		((SPObjectClass *) G_OBJECT_GET_CLASS(object))->identity_added (object, id, other);
}

void
sp_object_invoke_identity_removed (SPObject *object, const unsigned char *id, SPObject *other)
{
	if (((SPObjectClass *) G_OBJECT_GET_CLASS(object))->identity_removed)
		((SPObjectClass *) G_OBJECT_GET_CLASS(object))->identity_removed (object, id, other);
}

void
sp_object_invoke_forall (SPObject *object, SPObjectMethod func, void *data)
{
	TheraNode *child;

	g_return_if_fail (object != NULL);
	g_return_if_fail (SP_IS_OBJECT (object));
	g_return_if_fail (func != NULL);

	func (object, data);

	/* fixme: Why we are doing this indirectly? (Lauris) */
	for (child = thera_node_get_first_child (object->node); child != NULL; child = thera_node_get_next_sibling (child)) {
		const unsigned char *id;
		SPObject *cho;
		id = thera_node_get_attribute (child, "id");
		cho = sp_document_lookup_id (SP_OBJECT_DOCUMENT (object), id);
		if (cho) sp_object_invoke_forall (cho, func, data);
	}
}

static const gchar*
sp_xml_get_space_string(unsigned int space)
{
	switch(space)
	{
	case SP_XML_SPACE_DEFAULT:
		return "default";
	case SP_XML_SPACE_PRESERVE:
		return "preserve";
	default:
		return NULL;
	}
}

static TheraNode *
sp_object_private_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, unsigned int flags)
{
	if (!node && (flags & SP_OBJECT_WRITE_BUILD)) {
		node = thera_node_clone (object->node, thedoc, TRUE);
	} else {
		thera_node_set_attribute (node, "id", object->id);

		if (object->xml_space.set) {
			const char *xml_space;
			xml_space = sp_xml_get_space_string (object->xml_space.value);
			thera_node_set_attribute (node, "xml:space", xml_space);
		}
	}
	
	return node;
}

TheraNode *
sp_object_invoke_write (SPObject *object, TheraDocument *thedoc, TheraNode *node, unsigned int flags)
{
	g_return_val_if_fail (object != NULL, NULL);
	g_return_val_if_fail (SP_IS_OBJECT (object), NULL);

	if (((SPObjectClass *) G_OBJECT_GET_CLASS(object))->write) {
		if (flags & SP_OBJECT_WRITE_BUILD) {
			g_return_val_if_fail (thedoc != NULL, NULL);
			return ((SPObjectClass *) G_OBJECT_GET_CLASS(object))->write (object, thedoc, node, flags);
		} else {
			return ((SPObjectClass *) G_OBJECT_GET_CLASS(object))->write (object, thera_node_get_document (object->node), object->node, flags);
		}
	} else {
		g_warning ("Class %s does not implement ::write", G_OBJECT_TYPE_NAME (object));
		if (flags & SP_OBJECT_WRITE_BUILD) {
			g_return_val_if_fail (thedoc != NULL, NULL);
			return thera_node_clone (object->node, thedoc, TRUE);
		} else {
			thera_node_merge (node, object->node, "id", TRUE);
			return node;
		}
	}
}

/* Modification */

void
sp_object_request_update (SPObject *object, unsigned int flags)
{
	unsigned int propagate;

	g_return_if_fail (object != NULL);
	g_return_if_fail (SP_IS_OBJECT (object));
	g_return_if_fail (!(flags & SP_OBJECT_PARENT_MODIFIED_FLAG));
	g_return_if_fail ((flags & SP_OBJECT_MODIFIED_FLAG) || (flags & SP_OBJECT_CHILD_MODIFIED_FLAG));
	g_return_if_fail (!((flags & SP_OBJECT_MODIFIED_FLAG) && (flags & SP_OBJECT_CHILD_MODIFIED_FLAG)));

	/* Check for propagate before we set any flags */
	/* Propagate means, that object is not passed through by modification request cascade yet */
	propagate = (!(object->uflags & (SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_CHILD_MODIFIED_FLAG)));

	/* Just set object flags safe even if some have been set before */
	object->uflags |= flags;

	if (propagate) {
		if (object->parent) {
			sp_object_request_update (object->parent, SP_OBJECT_CHILD_MODIFIED_FLAG);
		} else {
			sp_document_request_update (object->document);
		}
	}
}

void
sp_object_invoke_update (SPObject *object, SPCtx *ctx, unsigned int flags)
{
	g_return_if_fail (object != NULL);
	g_return_if_fail (SP_IS_OBJECT (object));
	g_return_if_fail (!(flags & ~SP_OBJECT_MODIFIED_CASCADE));

#ifdef SP_OBJECT_DEBUG_CASCADE
	g_print("Update %s:%s %x %x\n", g_type_name_from_instance ((GTypeInstance *) object), SP_OBJECT_ID (object), flags, object->flags);
#endif

	/* Get object flags */
	flags |= object->uflags;
	/* Copy flags to modified cascade for later processing */
	object->mflags |= object->uflags;

	/* Merge style if we have good reasons to think that parent style is changed */
	/* I am not sure, whether we should check only propagated flag */
	/* We are currently assuming, that style parsing is done immediately */
	/* I think this is correct (Lauris) */
	if ((flags & SP_OBJECT_STYLE_MODIFIED_FLAG) && (flags & SP_OBJECT_PARENT_MODIFIED_FLAG)) {
		if (object->style && object->parent) {
			sp_style_merge_from_parent (object->style, object->parent->style);
		}
		/* attribute */
		sp_object_read_attr (object, "xml:space");
	}

	/* We have to clear flags here to allow rescheduling update */
	object->uflags = 0;

	if (((SPObjectClass *) G_OBJECT_GET_CLASS (object))->update)
		((SPObjectClass *) G_OBJECT_GET_CLASS (object))->update (object, ctx, flags);
}

void
sp_object_invoke_children_update (SPObject *object, SPCtx *ctx, unsigned int flags)
{
	SPObject *child;
	GSList *l;

	if (flags & SP_OBJECT_MODIFIED_FLAG) flags |= SP_OBJECT_PARENT_MODIFIED_FLAG;
	flags &= SP_OBJECT_MODIFIED_CASCADE;

	l = NULL;
	for (child = object->children; child != NULL; child = child->next) {
		g_object_ref ((GObject *) child);
		l = g_slist_prepend (l, child);
	}
	l = g_slist_reverse (l);
	while (l) {
		child = (SPObject *) l->data;
		l = g_slist_remove (l, child);
		if (flags || (child->uflags & SP_OBJECT_MODIFIED_FLAG)) {
			sp_object_invoke_update (child, ctx, flags);
		}
		g_object_unref ((GObject *) child);
	}
}

void
sp_object_request_modified (SPObject *object, unsigned int flags)
{
	unsigned int propagate;

	g_return_if_fail (object != NULL);
	g_return_if_fail (SP_IS_OBJECT (object));
	g_return_if_fail (!(flags & SP_OBJECT_PARENT_MODIFIED_FLAG));
	g_return_if_fail ((flags & SP_OBJECT_MODIFIED_FLAG) || (flags & SP_OBJECT_CHILD_MODIFIED_FLAG));
	g_return_if_fail (!((flags & SP_OBJECT_MODIFIED_FLAG) && (flags & SP_OBJECT_CHILD_MODIFIED_FLAG)));

	/* Check for propagate before we set any flags */
	/* Propagate means, that object is not passed through by modification request cascade yet */
	propagate = (!(object->mflags & (SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_CHILD_MODIFIED_FLAG)));

	/* Just set object flags safe even if some have been set before */
	object->mflags |= flags;

	if (propagate) {
		if (object->parent) {
			sp_object_request_modified (object->parent, SP_OBJECT_CHILD_MODIFIED_FLAG);
		} else {
			sp_document_request_update (object->document);
		}
	}
}

void
sp_object_invoke_modified (SPObject *object, unsigned int flags)
{
	g_return_if_fail (object != NULL);
	g_return_if_fail (SP_IS_OBJECT (object));
	g_return_if_fail (!(flags & ~SP_OBJECT_MODIFIED_CASCADE));

#ifdef SP_OBJECT_DEBUG_CASCADE
	g_print("Modified %s:%s %x %x\n", g_type_name_from_instance ((GTypeInstance *) object), SP_OBJECT_ID (object), flags, object->flags);
#endif

	/* Get object flags */
	flags |= object->mflags;
	/* We have to clear flags here to allow rescheduling modified */
	object->mflags = 0;

	g_object_ref (G_OBJECT (object));
	g_signal_emit (G_OBJECT (object), object_signals[MODIFIED], 0, flags);
	g_object_unref (G_OBJECT (object));

	sp_document_invoke_object_modified (object->document, object, flags);

#if 0
	/* If style is modified, invoke style_modified virtual method */
	/* It is pure convenience, and should be used with caution */
	/* The cascade is created solely by modified method plus appropriate flag */
	/* Also, it merely signals, that actual style object has changed */
	if (flags & SP_OBJECT_STYLE_MODIFIED_FLAG) {
		if (((SPObjectClass *) G_OBJECT_GET_CLASS(object))->style_modified)
			(*((SPObjectClass *) G_OBJECT_GET_CLASS(object))->style_modified) (object, flags);
	}
#endif
}

void
sp_object_invoke_children_modified (SPObject *object, unsigned int flags)
{
	SPObject *child;
	GSList *l;

	if (flags & SP_OBJECT_MODIFIED_FLAG) flags |= SP_OBJECT_PARENT_MODIFIED_FLAG;
	flags &= SP_OBJECT_MODIFIED_CASCADE;

	l = NULL;
	for (child = object->children; child != NULL; child = child->next) {
		g_object_ref ((GObject *) child);
		l = g_slist_prepend (l, child);
	}
	l = g_slist_reverse (l);
	while (l) {
		child = (SPObject *) l->data;
		l = g_slist_remove (l, child);
		if (flags || (child->mflags & SP_OBJECT_MODIFIED_FLAG)) {
			sp_object_invoke_modified (child, flags);
		}
		g_object_unref ((GObject *) child);
	}
}

/* Calculate sequence number of target */

unsigned int
sp_object_invoke_sequence (SPObject *object, SPObject *target, unsigned int *seq)
{
	if (object == target) return TRUE;

	if (((SPObjectClass *) G_OBJECT_GET_CLASS (object))->sequence)
		return (*((SPObjectClass *) G_OBJECT_GET_CLASS(object))->sequence) (object, target, seq);

	return FALSE;
}

/*
 * Get and set descriptive parameters
 *
 * These are inefficent, so they are not intended to be used interactively
 */

const unsigned char *
sp_object_title_get (SPObject *object)
{
	return NULL;
}

const unsigned char *
sp_object_description_get (SPObject *object)
{
	return NULL;
}

unsigned int
sp_object_title_set (SPObject *object, const unsigned char *title)
{
	return FALSE;
}

unsigned int
sp_object_description_set (SPObject *object, const unsigned char *desc)
{
	return FALSE;
}

const unsigned char *
sp_object_tagName_get (const SPObject *object, SPException *ex)
{
	/* If exception is not clear, return */
	if (!SP_EXCEPTION_IS_OK (ex)) return NULL;

	/* fixme: Exception if object is NULL? */
	return thera_node_get_name (object->node);
}

const unsigned char *
sp_object_getAttribute (const SPObject *object, const unsigned char *key, SPException *ex)
{
	/* If exception is not clear, return */
	if (!SP_EXCEPTION_IS_OK (ex)) return NULL;

	/* fixme: Exception if object is NULL? */
	return (const unsigned char *) thera_node_get_attribute (object->node, key);
}

void
sp_object_setAttribute (SPObject *object, const unsigned char *key, const unsigned char *value, SPException *ex)
{
	/* If exception is not clear, return */
	g_return_if_fail (SP_EXCEPTION_IS_OK (ex));

	/* fixme: Exception if object is NULL? */
	if (!thera_node_set_attribute (object->node, key, value)) {
		ex->code = SP_NO_MODIFICATION_ALLOWED_ERR;
	}
}

void
sp_object_removeAttribute (SPObject *object, const unsigned char *key, SPException *ex)
{
	/* If exception is not clear, return */
	g_return_if_fail (SP_EXCEPTION_IS_OK (ex));

	/* fixme: Exception if object is NULL? */
	if (!thera_node_set_attribute (object->node, key, NULL)) {
		ex->code = SP_NO_MODIFICATION_ALLOWED_ERR;
	}
}

/* Helper */

static gchar *
sp_object_get_unique_id (SPObject *object, const char *id)
{
	static gint count = 0;
	const char *name;
	const char *local;
	gchar * realid;
	gchar * b;
	gint len;

	g_assert (SP_IS_OBJECT (object));
	g_assert (SP_IS_DOCUMENT (object->document));

	count++;

	name = thera_node_get_name (object->node);
	/* Unnamed elements are TEXT nodes */
	if (!name) {
		name = "TEXT";
	} else {
		local = strchr (name, ':');
		if (local) name = local + 1;
	}

	len = strlen (name) + 17;
	b = alloca (len);
	g_assert (b != NULL);
	realid = NULL;

	if (id != NULL) {
		if (sp_document_lookup_id (object->document, id) == NULL) {
			realid = g_strdup (id);
			g_assert (realid != NULL);
		}
	}

	while (realid == NULL) {
		g_snprintf (b, len, "%s%d", name, count);
		if (sp_document_lookup_id (object->document, b) == NULL) {
			realid = g_strdup (b);
			g_assert (realid != NULL);
		} else {
			count++;
		}
	}

	return realid;
}

/* Style */

const guchar *
sp_object_get_style_property (SPObject *object, const char *key, const char *def)
{
	const char *style;
	const char *val;

	g_return_val_if_fail (object != NULL, NULL);
	g_return_val_if_fail (SP_IS_OBJECT (object), NULL);
	g_return_val_if_fail (key != NULL, NULL);

	style = thera_node_get_attribute (object->node, "style");
	if (style) {
		gchar *p;
		gint len;
		len = strlen (key);
		for (p = strstr (style, key); p; p = strstr (style, key)) {
			p += len;
			while ((*p <= ' ') && *p) p++;
			if (*p++ != ':') break;
			while ((*p <= ' ') && *p) p++;
			if (*p) return p;
		}
	}
	val = thera_node_get_attribute (object->node, key);
	if (val) return val;
	if (object->parent) {
		return sp_object_get_style_property (object->parent, key, def);
	}

	return def;
}

SPObject *
sp_object_get_ref_from_thera_ref (SPObject *object, TheraNode *theref, SPObject *ignore)
{
	SPObject *oref, *ochild;
	TheraNode *thechild;

	if (!theref) return NULL;

	oref = NULL;
	thechild = thera_node_get_first_child (object->node);
	ochild = object->children;
	if (ochild == ignore) ochild = ochild->next;

	while (thechild) {
		if (ochild && (ochild->node == thechild)) {
			/* Advance object reference */
			oref = ochild;
			ochild = ochild->next;
			if (ochild == ignore) ochild = ochild->next;
		}
		if (thechild == theref) return oref;
		/* Advance node reference */
		thechild = thera_node_get_next_sibling (thechild);
	}

	g_assert_not_reached ();

	return NULL;
}

SPObject *
sp_object_get_ref_from_thera_child (SPObject *object, TheraNode *thechild)
{
	SPObject *oref, *ochild;
	TheraNode *theref;

	if (!thechild) return NULL;

	oref = NULL;
	theref = thera_node_get_first_child (object->node);
	ochild = object->children;

	while (theref) {
		if (theref == thechild) return oref;
		if (ochild && (ochild->node == theref)) {
			/* Advance object reference */
			oref = ochild;
			ochild = ochild->next;
		}
		/* Advance node reference */
		theref = thera_node_get_next_sibling (theref);
	}

	g_assert_not_reached ();

	return NULL;
}

/* DEPRECATED */

#if 1
/*
 * Attaching/detaching
 */

SPObject *
sp_object_attach_list_reref (SPObject *parent, SPObject *object, SPObject *next)
{
	g_return_val_if_fail (parent != NULL, NULL);
	g_return_val_if_fail (SP_IS_OBJECT (parent), NULL);
	g_return_val_if_fail (object != NULL, NULL);
	g_return_val_if_fail (SP_IS_OBJECT (object), NULL);
	g_return_val_if_fail (!next || SP_IS_OBJECT (next), NULL);
	g_return_val_if_fail (!object->parent, NULL);
	g_return_val_if_fail (!object->next, NULL);

	sp_object_ref (object, parent);
	g_object_unref (G_OBJECT (object));
	object->parent = parent;
	object->next = next;

	return object;
}

SPObject *
sp_object_detach_list (SPObject *parent, SPObject *ref, SPObject *object)
{
	SPObject *next;

	g_return_val_if_fail (parent != NULL, NULL);
	g_return_val_if_fail (SP_IS_OBJECT (parent), NULL);
	g_return_val_if_fail (object != NULL, NULL);
	g_return_val_if_fail (SP_IS_OBJECT (object), NULL);
	g_return_val_if_fail (object->parent == parent, NULL);

	next = object->next;
	object->parent = NULL;
	object->next = NULL;

	sp_object_invoke_release (object, parent, ref);

	return next;
}

SPObject *
sp_object_detach_list_unref (SPObject *parent, SPObject *ref, SPObject *object)
{
	SPObject *next;

	g_return_val_if_fail (parent != NULL, NULL);
	g_return_val_if_fail (SP_IS_OBJECT (parent), NULL);
	g_return_val_if_fail (object != NULL, NULL);
	g_return_val_if_fail (SP_IS_OBJECT (object), NULL);
	g_return_val_if_fail (object->parent == parent, NULL);

	next = object->next;
	object->parent = NULL;
	object->next = NULL;

	sp_object_invoke_release (object, parent, ref);

	sp_object_unref (object, parent);

	return next;
}
#endif

