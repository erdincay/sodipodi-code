#ifndef __THERA_H__
#define __THERA_H__

//
// LibThera
//
// Copyright (C) Lauris Kaplinski 2007
//

namespace Thera {

struct Document;
struct Node;

struct EventVector {
	// Cannot be vetoed
	void (* destroy) (Node *node, void *data);
	// Returning false vetoes mutation
	// Ref is one step left from child
	unsigned int (* add_child) (Node *node, Node *child, Node *ref, void *data);
	void (* child_added) (Node *node, Node *child, Node *ref, void *data);
	unsigned int (* remove_child) (Node *node, Node *child, Node *ref, void *data);
	void (* child_removed) (Node *node, Node *child, Node *ref, void *data);
	unsigned int (* change_attr) (Node *node, const char *key, const char *oldval, const char *newval, void *data);
	void (* attr_changed) (Node *node, const char *key, const char *oldval, const char *newval, void *data);
	unsigned int (* change_content) (Node *node, const char *oldcontent, const char *newcontent, void *data);
	void (* content_changed) (Node *node, const char *oldcontent, const char *newcontent, void *data);
	unsigned int (* change_order) (Node *node, Node *child, Node *oldref, Node *newref, void *data);
	void (* order_changed) (Node *node, Node *child, Node *oldref, Node *newref, void *data);
};

struct Document
{
	Node *root;

	Document (void);
	~Document (void);
};

struct Node {
	struct NodeArray;
	struct Attribute;
	struct AttributeArray;
	struct Listener;
	struct ListenerArray;

	enum Type {
		ELEMENT,
		TEXT,
		CDATA,
		COMMENT,
		NUM_TYPES
	};

	Type type;
	Document *document;
	Node *parent;
	Node *next;
	char *name;
	char *content;
	Node *children;
	AttributeArray *attributes;
	ListenerArray *listeners;

	Node (Type type, Document *document, const char *name);
	~Node (void);

	Type getType (void) const { return type; }

	const char *getName (void) const { return name; }

	Document *getDocument (void) const { return document; }

	int getNumAttributes (void) const;
	const char *getAttributeName (int idx) const;
	const char *getAttribute (int idx) const;
	const char *getAttribute (const char *name) const;
	bool setAttribute (const char *name, const char *value);

	const char *getTextContent (void) { return content; }
	bool setTextContent (const char *newcontent);

	Node *getParent (void) { return parent; }
	Node *getFirstChild (void) { return children; }
	Node *getNextSibling (void) { return next; }

	bool addChild (Node *child, Node *ref);
	bool appendChild (Node *child);
	bool removeChild (Node *child);
	bool relocateChild (Node *child, Node *ref);

	void addListener (const EventVector *events, void *data);
	void removeListener (void *data);

	// Helper
	Node *clone (Document *pdocument, bool recursive);
};

Document *load (const char *filename);

unsigned int save (const Document *document, const char *filename);

} // Namespace Thera

#endif
