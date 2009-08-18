#define __THERA_NODE_CPP__

//
// LibThera
//
// Copyright (C) Lauris Kaplinski 2009
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "thera.h"

#include "thera-node.h"

struct _TheraNode {
	Thera::Node t;
};

struct _TheraDocument {
	Thera::Document t;
};

unsigned int
thera_node_get_type (TheraNode *node)
{
	return (unsigned int) node->t.type;
}

TheraDocument *
thera_node_get_document (TheraNode *node)
{
	return (TheraDocument *) node->t.document;
}

const char *
thera_node_get_name (TheraNode *node)
{
	return node->t.name;
}

unsigned int
thera_node_get_num_attributes (TheraNode *node)
{
	return node->t.getNumAttributes ();
}

const char *
thera_node_get_attribute_name (TheraNode *node, unsigned int attridx)
{
	return node->t.getAttributeName (attridx);
}

const char *
thera_node_get_attribute_value (TheraNode *node, unsigned int attridx)
{
	return node->t.getAttribute (attridx);
}

const char *
thera_node_get_attribute (TheraNode *node, const char *key)
{
	return node->t.getAttribute (key);
}

unsigned int
thera_node_set_attribute (TheraNode *node, const char *key, const char *value)
{
	return node->t.setAttribute (key, value);
}

const char *
thera_node_get_text_content (TheraNode *node)
{
	return node->t.getTextContent ();
}

unsigned int
thera_node_set_text_content (TheraNode *node, const char *value)
{
	return node->t.setTextContent (value);
}

TheraNode *
thera_node_get_parent (TheraNode *node)
{
	return (TheraNode *) node->t.parent;
}

TheraNode *
thera_node_get_first_child (TheraNode *node)
{
	return (TheraNode *) node->t.children;
}

TheraNode *
thera_node_get_next_sibling (TheraNode *node)
{
	return (TheraNode *) node->t.next;
}

unsigned int
thera_node_add_child (TheraNode *node, TheraNode *child, TheraNode *ref)
{
	return node->t.addChild ((Thera::Node *) child, (Thera::Node *) ref);
}

unsigned int
thera_node_append_child (TheraNode *node, TheraNode *child)
{
	return node->t.appendChild ((Thera::Node *) child);
}

unsigned int
thera_node_remove_child (TheraNode *node, TheraNode *child)
{
	return node->t.removeChild ((Thera::Node *) child);
}

unsigned int
thera_node_relocate_child (TheraNode *node, TheraNode *child, TheraNode *ref)
{
	return node->t.relocateChild ((Thera::Node *) child, (Thera::Node *) ref);
}

void
thera_node_add_listener (TheraNode *node, const TheraNodeEventVector *events, void *data)
{
	node->t.addListener ((const Thera::EventVector *) events, data);
}

void
thera_node_remove_listener (TheraNode *node, void *data)
{
	node->t.removeListener (data);
}

TheraNode *
thera_node_clone (TheraNode *node, TheraDocument *document, unsigned int recursive)
{
	return (TheraNode *) node->t.clone ((Thera::Document *) document, recursive != 0);
}

unsigned int
thera_node_set_attribute_recursive (TheraNode *node, const char *key, const char *value)
{
	bool result = node->t.setAttribute (key, value);
	for (Thera::Node *child = node->t.children; child; child = child->next) {
		result = result & child->setAttribute (key, value);
	}
	return result;
}

unsigned int
thera_node_merge (TheraNode *node, TheraNode *from, const char *identitykey, unsigned int recursive)
{
	// fixme:
	return 0;
}

unsigned int
thera_node_get_boolean (TheraNode *node, const char *key, unsigned int *value)
{
	const char *val = node->t.getAttribute (key);
	if (!val) return 0;
	int rval = -1;
	if (!strcmp (val, "true") || !strcmp (val, "True") || !strcmp (val, "TRUE")) {
		rval = 1;
	} else if (!strcmp (val, "yes") || !strcmp (val, "Yes") || !strcmp (val, "YES")) {
		rval = 1;
	} else if (!strcmp (val, "false") || !strcmp (val, "False") || !strcmp (val, "FALSE")) {
		rval = 0;
	} else if (!strcmp (val, "no") || !strcmp (val, "No") || !strcmp (val, "NO")) {
		rval = 0;
	} else {
		char *end;
		unsigned long sval = strtoul (val, &end, 10);
		if (end && !*end) {
			rval = (sval != 0);
		}
	}
	if (rval >= 0) {
		if (value) *value = rval;
		return 1;
	}
	return 0;
}

unsigned int
thera_node_get_int (TheraNode *node, const char *key, int *value)
{
	const char *val = node->t.getAttribute (key);
	if (!val) return 0;
	char *end;
	long sval = strtol (val, &end, 10);
	if (end && !*end) {
		if (value) *value = (int) sval;
		return 1;
	}
	return 0;
}

unsigned int
thera_node_get_double (TheraNode *node, const char *key, double *value)
{
	const char *val = node->t.getAttribute (key);
	char *end;
	double sval = strtod (val, &end);
	if (end && !*end) {
		if (value) *value = sval;
		return 1;
	}
	return 0;
}

unsigned int
thera_node_set_boolean (TheraNode *node, const char *key, unsigned int value)
{
	return node->t.setAttribute (key, (value) ? "true" : "false");
}

unsigned int
thera_node_set_int (TheraNode *node, const char *key, int value)
{
	char c[32];
	sprintf (c, "%d", value);
	return node->t.setAttribute (key, c);
}

unsigned int
thera_node_set_double (TheraNode *node, const char *key, double value)
{
	char c[32];
	sprintf (c, "%g", value);
	return node->t.setAttribute (key, c);
}

unsigned int
thera_node_unparent (TheraNode *node)
{
	if (!node->t.parent) return 0;
	return node->t.parent->removeChild ((Thera::Node *) node);
}

unsigned int
thera_node_get_num_children (TheraNode *node)
{
	return node->t.getNumChildren ();
}

TheraNode *
thera_node_get_child_by_index (TheraNode *node, unsigned int index)
{
	return (TheraNode *) node->t.getChild (index);
}

int
thera_node_compare_position (TheraNode *lhs, TheraNode *rhs)
{
	if (lhs == rhs) return NULL;
	for (Thera::Node *node = lhs->t.next; node; node = node->next) {
		if (node == (Thera::Node *) rhs) return -1;
	}
	for (Thera::Node *node = rhs->t.next; node; node = node->next) {
		if (node == (Thera::Node *) lhs) return 1;
	}
	return 0;
}

unsigned int
thera_node_set_position_absolute (TheraNode *node, int newpos)
{
	Thera::Node *parent = node->t.parent;
	if (!parent) return 0;
	if (newpos < 0) newpos = parent->getNumChildren () - 1;
	if (newpos == 0) {
		return parent->relocateChild ((Thera::Node *) node, NULL);
	} else {
		Thera::Node *ref = parent->getChild (newpos - 1);
		return parent->relocateChild ((Thera::Node *) node, ref);
	}
}

void
thera_node_delete (TheraNode *node)
{
	delete node;
}

TheraNode *
thera_node_lookup_child_by_attribute (TheraNode *node, const char *key, const char *value)
{
	for (Thera::Node *child = node->t.children; child; child = child->next) {
		const char *cval = child->getAttribute (key);
		if (cval && value && !strcmp (cval, value)) return (TheraNode *) child;
		if (!cval && !value) return (TheraNode *) child;
	}
	return NULL;
}

TheraNode *
thera_node_lookup_child_by_name (TheraNode *node, const char *name)
{
	for (Thera::Node *child = node->t.children; child; child = child->next) {
		if (child->name && !strcmp (child->name, name)) return (TheraNode *) child;
	}
	return NULL;
}

TheraNode *
thera_node_lookup_child_by_name_id (TheraNode *node, const char *name, const char *id)
{
	for (Thera::Node *child = node->t.children; child; child = child->next) {
		if (child->name && !strcmp (child->name, name)) {
			const char *cval = child->getAttribute ("id");
			if (cval && id && !strcmp (cval, id)) return (TheraNode *) child;
			if (!cval && !id) return (TheraNode *) child;
		}
	}
	return NULL;
}
