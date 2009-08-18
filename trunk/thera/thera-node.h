#ifndef __THERA_NODE_H__
#define __THERA_NODE_H__

/*
 * LibThera
 *
 * Copyright (C) Lauris Kaplinski 2007
 *
*/

typedef struct _TheraNode TheraNode;
typedef struct _TheraNodeEventVector TheraNodeEventVector;

#include <thera/thera-document.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _TheraNodeEventVector {
	// Cannot be vetoed
	void (* destroy) (TheraNode *node, void *data);
	// Returning false vetoes mutation
	// Ref is one step left from child
	unsigned int (* add_child) (TheraNode *node, TheraNode *child, TheraNode *ref, void *data);
	void (* child_added) (TheraNode *node, TheraNode *child, TheraNode *ref, void *data);
	unsigned int (* remove_child) (TheraNode *node, TheraNode *child, TheraNode *ref, void *data);
	void (* child_removed) (TheraNode *node, TheraNode *child, TheraNode *ref, void *data);
	unsigned int (* change_attr) (TheraNode *node, const char *key, const char *oldval, const char *newval, void *data);
	void (* attr_changed) (TheraNode *node, const char *key, const char *oldval, const char *newval, void *data);
	unsigned int (* change_content) (TheraNode *node, const char *oldcontent, const char *newcontent, void *data);
	void (* content_changed) (TheraNode *node, const char *oldcontent, const char *newcontent, void *data);
	unsigned int (* change_order) (TheraNode *node, TheraNode *child, TheraNode *oldref, TheraNode *newref, void *data);
	void (* order_changed) (TheraNode *node, TheraNode *child, TheraNode *oldref, TheraNode *newref, void *data);
	// Upstream handlers
	// If node does not have particular handler installed parent upstream handler is called and so on
	void (* downstream_child_added) (TheraNode *node, TheraNode *child, TheraNode *ref, void *data);
	void (* downstream_child_removed) (TheraNode *node, TheraNode *child, TheraNode *ref, void *data);
	void (* downstream_attr_changed) (TheraNode *node, const char *key, const char *oldval, const char *newval, void *data);
	void (* downstream_content_changed) (TheraNode *node, const char *oldcontent, const char *newcontent, void *data);
	void (* downstream_order_changed) (TheraNode *node, TheraNode *child, TheraNode *oldref, TheraNode *newref, void *data);
};

/* Element types */

enum TheraNodeType { THERA_NODE_ELEMENT, THERA_NODE_TEXT, THERA_NODE_CDATA, THERA_NODE_COMMENT, THERA_NODE_DOCTYPE, NUM_THERA_NODE_TYPES };

unsigned int thera_node_get_type (TheraNode *node);
TheraDocument *thera_node_get_document (TheraNode *node);

const char *thera_node_get_name (TheraNode *node);

unsigned int thera_node_get_num_attributes (TheraNode *node);
const char *thera_node_get_attribute_name (TheraNode *node, unsigned int attridx);
const char *thera_node_get_attribute_value (TheraNode *node, unsigned int attridx);
const char *thera_node_get_attribute (TheraNode *node, const char *key);
unsigned int thera_node_set_attribute (TheraNode *node, const char *key, const char *value);

const char *thera_node_get_text_content (TheraNode *node);
unsigned int thera_node_set_text_content (TheraNode *node, const char *value);

TheraNode *thera_node_get_parent (TheraNode *node);
TheraNode *thera_node_get_first_child (TheraNode *node);
TheraNode *thera_node_get_next_sibling (TheraNode *node);

unsigned int thera_node_add_child (TheraNode *node, TheraNode *child, TheraNode *ref);
unsigned int thera_node_append_child (TheraNode *node, TheraNode *child);
unsigned int thera_node_remove_child (TheraNode *node, TheraNode *child);
unsigned int thera_node_relocate_child (TheraNode *node, TheraNode *child, TheraNode *ref);

void thera_node_add_listener (TheraNode *node, const TheraNodeEventVector *events, void *data);
void thera_node_remove_listener (TheraNode *node, void *data);

TheraNode *thera_node_clone (TheraNode *node, TheraDocument *document, unsigned int recursive);

unsigned int thera_node_set_attribute_recursive (TheraNode *node, const char *key, const char *value);
unsigned int thera_node_merge (TheraNode *node, TheraNode *from, const char *identitykey, unsigned int recursive);

unsigned int thera_node_get_boolean (TheraNode *node, const char *key, unsigned int *value);
unsigned int thera_node_get_int (TheraNode *node, const char *key, int *value);
unsigned int thera_node_get_double (TheraNode *node, const char *key, double *value);

unsigned int thera_node_set_boolean (TheraNode *node, const char *key, unsigned int value);
unsigned int thera_node_set_int (TheraNode *node, const char *key, int value);
unsigned int thera_node_set_double (TheraNode *node, const char *key, double value);

unsigned int thera_node_unparent (TheraNode *node);

unsigned int thera_node_get_num_children (TheraNode *node);
TheraNode *thera_node_get_child_by_index (TheraNode *node, unsigned int index);

int thera_node_compare_position (TheraNode *lhs, TheraNode *rhs);

unsigned int thera_node_set_position_absolute (TheraNode *node, int newpos);

void thera_node_delete (TheraNode *node);

TheraNode *thera_node_lookup_child_by_attribute (TheraNode *node, const char *key, const char *value);
TheraNode *thera_node_lookup_child_by_name (TheraNode *node, const char *name);
TheraNode *thera_node_lookup_child_by_name_id (TheraNode *node, const char *name, const char *id);

#ifdef __cplusplus
}
#endif

#endif
