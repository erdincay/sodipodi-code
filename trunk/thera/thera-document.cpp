#define __THERA_DOCUMENT_CPP__

//
// LibThera
//
// Copyright (C) Lauris Kaplinski 2009
//

#include "thera.h"

#include "thera-document.h"

struct _TheraDocument {
	Thera::Document t;
};

TheraDocument *
thera_document_new (const char *rootname)
{
	return (TheraDocument *) new Thera::Document(rootname);
}

void
thera_document_unref (TheraDocument *doc)
{
	delete (Thera::Document *) doc;
}

TheraNode *
thera_document_get_first_node (TheraDocument *doc)
{
	return (TheraNode *) doc->t.children;
}

TheraNode *
thera_document_get_root (TheraDocument *doc)
{
	return (TheraNode *) doc->t.root;
}

unsigned int
thera_document_add_node (TheraDocument *thedoc, TheraNode *node, TheraNode *ref)
{
	return thedoc->t.addNode ((Thera::Node *) node, (Thera::Node *) ref);
}

TheraNode *
thera_document_new_node (TheraDocument *doc, unsigned int nodetype, const char *name)
{
	return (TheraNode *) new Thera::Node((Thera::Node::Type) nodetype, (Thera::Document *) doc, name);
}

TheraNode *
thera_document_new_element (TheraDocument *doc, const char *name)
{
	return (TheraNode *) new Thera::Node((Thera::Document *) doc, name);
}

void 
thera_document_enable_transactions (TheraDocument *doc, unsigned int enabled)
{
	doc->t.enableTransactions (enabled);
}

void 
thera_document_collate_transactions (TheraDocument *doc, unsigned int collate)
{
	doc->t.collateTransactions (collate);
}

void 
thera_document_finish_transaction (TheraDocument *doc)
{
	doc->t.finishTransaction ();
}

void 
thera_document_cancel_transaction (TheraDocument *doc)
{
	doc->t.cancelTransaction ();
}

void 
thera_document_undo (TheraDocument *doc)
{
	doc->t.undo ();
}

void 
thera_document_redo (TheraDocument *doc)
{
	doc->t.redo ();
}

/* IO methods */

TheraDocument *
thera_document_load (const char *filename)
{
	return (TheraDocument *) Thera::load (filename);
}

TheraDocument *
thera_document_load_from_mem (const unsigned char *cdata, size_t csize)
{
	return (TheraDocument *) Thera::load (cdata, csize, NULL);
}

unsigned int
thera_document_save (TheraDocument *doc, const char *filename)
{
	return Thera::save ((Thera::Document *) doc, filename);
}
