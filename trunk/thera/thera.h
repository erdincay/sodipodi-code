#ifndef __THERA_H__
#define __THERA_H__

//
// LibThera
//
// Copyright (C) Lauris Kaplinski 2007
//

namespace Thera {

class Document;
class Node;

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
	// Upstream handlers
	// If node does not have particular handler installed parent upstream handler is called and so on
	void (* downstream_child_added) (Node *node, Node *child, Node *ref, void *data);
	void (* downstream_child_removed) (Node *node, Node *child, Node *ref, void *data);
	void (* downstream_attr_changed) (Node *node, const char *key, const char *oldval, const char *newval, void *data);
	void (* downstream_content_changed) (Node *node, const char *oldcontent, const char *newcontent, void *data);
	void (* downstream_order_changed) (Node *node, Node *child, Node *oldref, Node *newref, void *data);
};

struct Record;
struct Transaction;

class Document
{
private:
	unsigned int log;
	unsigned int collate;

	// Transactions
	Transaction *current;
	Transaction *undolist;
	Transaction *redolist;
	void addRecord (Record *record);
	unsigned int checkNode (Node *node);
public:
	// All toplevel nodes
	Node *children;
	// Root element
	Node *root;

	// Constructor
	Document (const char *rootname);
	// Destructor
	virtual ~Document (void);

	// Transactions are not logged for following methods
	// Add root level node
	bool addNode (Node *node, Node *ref);
	// Clear transaction list
	void clearTransactions (void);
	// Move single text elements to content
	bool collateTexts (void);
	// Expand content to text elements
	bool expandTexts (void);

	// Transactions
	void enableTransactions (unsigned int enable);
	void collateTransactions (unsigned int enable);
	void finishTransaction (void);
	void cancelTransaction (void);
	void undo (void);
	void redo (void);

	// Transaction handlers for node implementation
	// Oldvalue is consumed by transaction
	void attributeChanged (Node *node, const char *attrid, char *oldvalue, const char *newvalue);
	void contentChanged (Node *node, char *oldvalue, const char *newvalue);
	void childInserted (Node *node, Node *ref, Node *child);
	// Child is consumed by transaction
	void childRemoved (Node *node, Node *ref, Node *child);
	void childRelocated (Node *node, Node *oldref, Node *newref, Node *child);
};

class Node {
private:
	struct NodeArray;
	struct Attribute;
	struct AttributeArray;
	struct Listener;
	struct ListenerArray;

	// Upstream signal emitters
	void emitDownstreamChildAdded (Node *node, Node *child, Node *ref);
	void emitDownstreamChildRemoved (Node *node, Node *child, Node *ref);
	void emitDownstreamAttrChanged (Node *node, const char *key, const char *oldval, const char *newval);
	void emitDownstreamContentChanged (Node *node, const char *oldcontent, const char *newcontent);
	void emitDownstreamOrderChanged (Node *node, Node *child, Node *oldref, Node *newref);
public:
	enum Type {
		ELEMENT,
		TEXT,
		CDATA,
		COMMENT,
		DOCTYPE,
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

	// Constructors
	Node (Type type, Document *document, const char *name);
	Node (Document *document, const char *name);
	// Destructor
	~Node (void);

	Type getType (void) const { return type; }

	const char *getName (void) const { return name; }

	Document *getDocument (void) const { return document; }

	unsigned int getNumAttributes (void) const;
	const char *getAttributeName (unsigned int idx) const;
	const char *getAttribute (unsigned int idx) const;
	const char *getAttribute (const char *name) const;
	bool setAttribute (const char *key, const char *value);

	const char *getTextContent (void) const { return content; }
	bool setTextContent (const char *newcontent);

	Node *getParent (void) const { return parent; }
	Node *getFirstChild (void) const { return children; }
	Node *getNextSibling (void) const { return next; }
	unsigned int getNumChildren (void) const;
	Node *getChild (unsigned int childidx) const;

	bool addChild (Node *child, Node *ref);
	bool appendChild (Node *child);
	// Child is not guaranteed to exist after removal
	bool removeChild (Node *child);
	bool relocateChild (Node *child, Node *ref);

	void addListener (const EventVector *events, void *data);
	void removeListener (void *data);

	// Helper
	Node *clone (Document *pdocument, bool recursive) const;
	// Convenience methods
	bool setAttributeInt (const char *key, int value);
	bool setAttributeUint (const char *key, unsigned int value);
	bool setAttributeFloat (const char *key, float value, unsigned int precision);
	bool setAttributeFloats (const char *key, const float *values, unsigned int nvalues, unsigned int precision);
	// Get content of this node or single text child
	const char *getContentOrChildText (void);

	// Move single text elements to content
	bool collateTexts (void);
	// Expand content to text elements
	bool expandTexts (void);
};

//
// Load document
//
// Texts are parsed as TEXT elements
//

Document *load (const char *filename);
Document *load (const unsigned char *cdata, size_t csize, const unsigned char *uri);

//
// Load node and insert in hierarchy
//
// Texts are parsed as TEXT elements
//

Node *loadNode (Node *parent, const char *filename);
Node *loadNode (Node *parent, const unsigned char *cdata, size_t csize, const unsigned char *uri);

unsigned int save (const Document *document, const char *filename);

// Negative level disables linefeeds
unsigned int write_node (char *d, unsigned int dlen, const Node *node, int level);
unsigned int write_document (char *d, unsigned int dlen, const Document *document, unsigned int indent);

} // Namespace Thera

#endif
