#define __THERA_TRANSACTION_CPP__

//
// LibThera
//
// Copyright (C) Lauris Kaplinski 2007-2009
//

#include <stdlib.h>
#include <string.h>

#include "transaction.h"

namespace Thera {

Path::Path (Node *node)
: length(0), positions(NULL)
{
	// Get length
	for (Node *r = node->getParent (); r; r = r->getParent ()) length += 1;
	positions = new int[length];

	Node *n = node;
	Node *p = n->getParent ();
	int q = length - 1;
	while (q >= 0) {
		positions[q] = 0;
		for (Node *c = p->getFirstChild (); c != n; c = c->getNextSibling ()) positions[q] += 1;
		n = p;
		p = p->getParent ();
		q -= 1;
	}
}

Path::~Path (void)
{
	delete positions;
}

Node *
Path::getNode (Document *doc)
{
	Node *node = doc->root;
	for (int i = 0; i < length; i++) {
		Node *parent = node;
		node = parent->getFirstChild ();
		for (int j = 0; j < positions[i]; j++) node = node->getNextSibling ();
	}
	return node;
}

// Record

Record::Record (Node *pnode, const char *pattrid, char *pprevvalue)
: type(ATTRIBUTE_CHANGE), next(NULL), path(pnode), attrid(strdup (pattrid)), prevvalue(pprevvalue), newref(-1), oldref(-1), delnode(NULL)
{
}

Record::Record (Node *pnode, char *pprevvalue)
: type(CONTENT_CHANGE), next(NULL), path(pnode), attrid(NULL), prevvalue(pprevvalue), newref(-1), oldref(-1), delnode(NULL)
{
}

Record::Record (Node *pnode, Node *pref)
: type(CHILD_INSERTION), next(NULL), path(pnode), attrid(NULL), prevvalue(NULL), newref(-1), oldref(-1), delnode(NULL)
{
	if (!pref) return;
	newref = 0;
	for (Node *c = pnode->getFirstChild (); c != pref; c = c->getNextSibling ()) newref += 1;
}

Record::Record (Node *pnode, Node *pref, Node *pchild)
: type(CHILD_DELETION), next(NULL), path(pnode), attrid(NULL), prevvalue(NULL), newref(-1), oldref(-1), delnode(pchild)
{
	if (!pref) return;
	oldref = 0;
	for (Node *c = pnode->getFirstChild (); c != pref; c = c->getNextSibling ()) oldref += 1;
}

Record::Record (Node *pnode, Node *poldref, Node *pnewref, Node *pchild)
: type(CHILD_RELOCATION), next(NULL), path(pnode), attrid(NULL), prevvalue(NULL), newref(-1), oldref(-1), delnode(NULL)
{
	if (poldref) {
		oldref = 0;
		for (Node *c = pnode->getFirstChild (); c != poldref; c = c->getNextSibling ()) oldref += 1;
	}
	if (pnewref) {
		newref = 0;
		for (Node *c = pnode->getFirstChild (); c != pnewref; c = c->getNextSibling ()) newref += 1;
	}
}

Record::~Record (void)
{
	if (attrid) free (attrid);
	if (prevvalue) free (prevvalue);
	if (delnode) delete delnode;
}

Node *
Record::getRef (Node *node, int refvalue)
{
	if (refvalue < 0) return NULL;
	Node *child = node->getFirstChild ();
	while (refvalue > 0) {
		child = child->getNextSibling ();
		refvalue -= 1;
	}
	return child;
}

void
Record::revert (Document *doc)
{
	if (type == ATTRIBUTE_CHANGE) {
		Node *node = path.getNode (doc);
		node->setAttribute (attrid, prevvalue);
	} else if (type == CONTENT_CHANGE) {
		Node *node = path.getNode (doc);
		node->setTextContent (prevvalue);
	} else if (type == CHILD_INSERTION) {
		Node *node = path.getNode (doc);
		Node *newrefnode = getRef (node, newref);
		Node *child = (newrefnode) ? newrefnode->getNextSibling () : node->getFirstChild ();
		node->removeChild (child);
	} else if (type == CHILD_DELETION) {
		Node *node = path.getNode (doc);
		Node *oldrefnode = getRef (node, oldref);
		node->addChild (delnode, oldrefnode);
		delnode = NULL;
	} else if (type == CHILD_RELOCATION) {
		Node *node = path.getNode (doc);
		Node *newrefnode = getRef (node, newref);
		Node *oldrefnode = getRef (node, oldref);
		Node *child = (newrefnode) ? newrefnode->getNextSibling () : node->getFirstChild ();
		node->relocateChild (child, oldrefnode);
	}
}

// Transaction

Transaction::Transaction (void)
: next(NULL), records(NULL)
{
}

Transaction::~Transaction (void)
{
	while (records) {
		Record *r = records;
		records = records->next;
		delete r;
	}
}

void
Transaction::addRecord (Record *record)
{
	record->next = records;
	records = record;
}

// Document

void
Document::addRecord (Record *record)
{
	if (!current) current = new Transaction();
	current->addRecord (record);
}

unsigned int
Document::checkNode (Node *node)
{
	while (node->getParent ()) node = node->getParent ();
	return node == root;
}

void
Document::attributeChanged (Node *node, const char *attrid, char *oldvalue, const char *newvalue)
{
	if (!log || !checkNode (node)) {
		if (oldvalue) free (oldvalue);
		return;
	}
	Record *r = new Record(node, attrid, oldvalue);
	addRecord (r);
}

void
Document::contentChanged (Node *node, char *oldvalue, const char *newvalue)
{
	if (!log || !checkNode (node)) {
		if (oldvalue) free (oldvalue);
		return;
	}
	Record *r = new Record(node, oldvalue);
	addRecord (r);
}

void
Document::childInserted (Node *node, Node *ref, Node *child)
{
	if (!log || !checkNode (node)) return;
	Record *r = new Record(node, ref);
	addRecord (r);
}

void
Document::childRemoved (Node *node, Node *ref, Node *child)
{
	if (!log || !checkNode (node)) {
		delete child;
		return;
	}
	Record *r = new Record(node, ref, child);
	addRecord (r);
}

void
Document::childRelocated (Node *node, Node *oldref, Node *newref, Node *child)
{
	if (!log || !checkNode (node)) return;
	Record *r = new Record(node, oldref, newref, child);
	addRecord (r);
}

void
Document::enableTransactions (unsigned int enable)
{
	if (log && !enable) finishTransaction ();
	log = enable;
}

void
Document::finishTransaction (void)
{
	// Push current to undo
	if (current) {
		current->next = undolist;
		undolist = current;
		current = NULL;
	}
	// Clear redo
	while (redolist) {
		Transaction *t = redolist;
		redolist = redolist->next;
		delete t;
	}
}

void
Document::cancelTransaction (void)
{
	if (!current) return;
	unsigned int oldlog = log;
	log = false;
	for (Record *r = current->records; r; r = r->next) {
		r->revert (this);
	}
	log = oldlog;
	delete current;
	current = NULL;
}

void
Document::undo (void)
{
	if (current) return;
	if (!undolist) return;
	Transaction *t = undolist;
	undolist = undolist->next;
	for (Record *r = t->records; r; r = r->next) {
		r->revert (this);
	}
	if (current) {
		current->next = redolist;
		redolist = current;
		current = NULL;
	}
}

void
Document::redo (void)
{
	if (current) return;
	if (!redolist) return;
	Transaction *t = redolist;
	redolist = redolist->next;
	for (Record *r = t->records; r; r = r->next) {
		r->revert (this);
	}
	if (current) {
		current->next = undolist;
		undolist = current;
		current = NULL;
	}
}

} // Namespace Thera
