#ifndef __THERA_TRANSACTION_H__
#define __THERA_TRANSACTION_H__

//
// LibThera
//
// Copyright (C) Lauris Kaplinski 2007-2009
//

#include <thera/thera.h>

namespace Thera {

struct Path {
public:
	int length;
	int *positions;

	// Constructor
	Path (Node *node);
	// Destructor
	~Path (void);

	Node *getNode (Document *doc);
};

struct Record {
private:
	Node *getRef (Node *node, int refvalue);
public:
	enum Type { ATTRIBUTE_CHANGE, CONTENT_CHANGE, CHILD_INSERTION, CHILD_DELETION, CHILD_RELOCATION };

	Type type;

	Record *next;

	// Path to changed node
	Path path;
	// Attribute id
	char *attrid;
	// Copy of previous attribute/content
	char *prevvalue;
	// Position of the ref of inserted or relocated child
	int newref;
	// Position of the ref of deleted child or previous location
	int oldref;
	// Copy of deleted child
	Node *delnode;

	// Constructors
	// These must be called after actual operation is performed (oldref and newref point to children positions after mutation)
	// Attribute change (consumes pprevvalue)
	Record (Node *pnode, const char *pattrid, char *pprevvalue);
	// Content change (consumes pprevvalue)
	Record (Node *pnode, char *pprevvalue);
	// Child insertion
	Record (Node *pnode, Node *pref);
	// Child deletion (consumes pchild)
	Record (Node *pnode, Node *pref, Node *pchild);
	// Child relocation
	Record (Node *pnode, Node *poldref, Node *pnewref, Node *pchild);
	// Destructor
	~Record (void);

	// Revert recorded action
	void revert (Document *doc);
	// Check if current record overwrites the value of previous record
	bool isValueOverwrite (Record *prev);
};

struct Transaction {
public:
	Transaction *next;

	Record *records;

	// Constructor;
	Transaction (void);
	// Destructor
	~Transaction (void);

	// Add record
	void addRecord (Record *record);
};

} // Namespace Thera

#endif
