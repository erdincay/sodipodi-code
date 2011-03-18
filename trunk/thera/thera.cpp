#define __THERA_CPP__

#include <malloc.h>
#include <string.h>
#include <stdio.h>

#include <libarikkei/arikkei-strlib.h>

#include "transaction.h"

#include "thera.h"

#ifdef WIN32
#define strdup _strdup
#endif

namespace Thera {

Document::Document (const char *rootname)
: log(0), collate(0), current(NULL), undolist(NULL), redolist(NULL), children(NULL), root(NULL)
{
	if (rootname) {
		children = new Node(Node::ELEMENT, this, rootname);
		root = children;
	}
}

Document::~Document (void)
{
	delete current;
	while (undolist) {
		Transaction *t = undolist;
		undolist = undolist->next;
		delete t;
	}
	while (redolist) {
		Transaction *t = redolist;
		redolist = redolist->next;
		delete t;
	}
	while (children) {
		Node *child = children;
		children = children->next;
		delete child;
	}
}

bool
Document::addNode (Node *node, Node *ref)
{
	if (!node || node->parent || (node->document != this)) return false;
	if (ref && (ref->parent || (node->document != this))) return false;
	if (ref) {
		node->next = ref->next;
		ref->next = node;
	} else {
		node->next = children;
		children = node;
	}
	fprintf (stderr, "Document::addNode: fixme: Implement transactions for rootlevel nodes\n");
	// childInserted (NULL, ref, node);
	return true;
}

bool
Document::collateTexts (void)
{
	unsigned int logstatus = log;
	for (Node *node = children; node; node = node->next) {
		node->collateTexts ();
	}
	log = logstatus;
	return true;
}

bool
Document::expandTexts (void)
{
	unsigned int logstatus = log;
	for (Node *node = children; node; node = node->next) {
		node->expandTexts ();
	}
	log = logstatus;
	return true;
}

struct Node::Attribute {
	char *name;
	char *value;
};

struct Node::AttributeArray {
	unsigned int size;
	unsigned int length;
	Attribute elements[1];
	static void destroy (AttributeArray *aa);
	static AttributeArray *ensureSpace (AttributeArray *aa, int ensure);
	static char *get (const AttributeArray *aa, const char *name);
	static AttributeArray *set (AttributeArray *aa, const char *name, const char *value);
	// Create duplicate of this attribute array
	AttributeArray *duplicate ();
};

struct Node::Listener {
	const EventVector *events;
	void *data;
};

struct Node::ListenerArray {
	int size;
	int length;
	Listener elements[1];
	static void destroy (ListenerArray *la);
	static ListenerArray *add (ListenerArray *la, const EventVector *events, void *data);
	static ListenerArray *remove (ListenerArray *la, void *data);
};

Node::Node (Type ptype, Document *pdocument, const char *pname)
: type(ptype), document(pdocument), parent(NULL), next(NULL), name(pname ? strdup(pname) : NULL), content(NULL),
children(NULL), attributes(NULL), listeners(NULL)
{
}

Node::Node (Document *pdocument, const char *pname)
: type(ELEMENT), document(pdocument), parent(NULL), next(NULL), name(strdup(pname)), content(NULL),
children(NULL), attributes(NULL), listeners(NULL)
{
}

Node::~Node (void)
{
	// Emit destroy
	if (listeners) {
		for (int i = 0; i < listeners->length; i++) {
			Listener& l(listeners->elements[i]);
			if (l.events->destroy) l.events->destroy (this, l.data);
		}
	}
	ListenerArray::destroy (listeners);
	AttributeArray::destroy (attributes);
	Node *child = children;
	while (child) {
		Node *next = child->next;
		delete child;
		child = next;
	}
	if (content) free (content);
	free (name);
}

unsigned int
Node::getNumAttributes (void) const
{
	return (attributes) ? attributes->length : 0;
}

const char *
Node::getAttributeName (unsigned int idx) const
{
	return (attributes && (idx < attributes->length)) ? attributes->elements[idx].name : NULL;
}

const char *
Node::getAttribute (unsigned int idx) const
{
	return (attributes && (idx < attributes->length)) ? attributes->elements[idx].value : NULL;
}

const char *
Node::getAttribute (const char *name) const
{
	return AttributeArray::get (attributes, name);
}

bool
Node::setAttribute (const char *name, const char *newvalue)
{
	char *oldvalue = AttributeArray::get (attributes, name);
	if (!oldvalue && !newvalue) return true;
	if (oldvalue && newvalue && !strcmp (oldvalue, newvalue)) return true;
	// Emit change_attr
	if (listeners) {
		for (int i = 0; i < listeners->length; i++) {
			Listener& l(listeners->elements[i]);
			if (l.events->change_attr) if (!l.events->change_attr (this, name, oldvalue, newvalue, l.data)) return false;
		}
	}
	attributes = AttributeArray::set (attributes, name, newvalue);
	// Emit attr_changed
	bool emitted = false;
	if (listeners) {
		for (int i = 0; i < listeners->length; i++) {
			Listener& l(listeners->elements[i]);
			if (l.events->attr_changed) {
				l.events->attr_changed (this, name, oldvalue, newvalue, l.data);
				emitted = true;
			}
		}
	}
	if (!emitted && parent) parent->emitDownstreamAttrChanged (this, name, oldvalue, newvalue);
	// This has to be last, because document consumes oldvalue
	document->attributeChanged (this, name, oldvalue, newvalue);
	return true;
}

bool
Node::setTextContent (const char *newcontent)
{
	char *oldcontent = content;
	if (!oldcontent && !newcontent) return true;
	if (oldcontent && newcontent && !strcmp (oldcontent, newcontent)) return true;
	// Emit change_content
	if (listeners) {
		for (int i = 0; i < listeners->length; i++) {
			Listener& l(listeners->elements[i]);
			if (l.events->change_content) if (!l.events->change_content (this, oldcontent, newcontent, l.data)) return false;
		}
	}
	content = (newcontent) ? strdup (newcontent) : NULL;
	// Emit content_changed
	bool emitted = false;
	if (listeners) {
		for (int i = 0; i < listeners->length; i++) {
			Listener& l(listeners->elements[i]);
			if (l.events->content_changed) {
				l.events->content_changed (this, oldcontent, newcontent, l.data);
				emitted = true;
			}
		}
	}
	if (!emitted && parent) parent->emitDownstreamContentChanged (this, oldcontent, newcontent);
	// This has to be last because document consumes oldcontent
	document->contentChanged (this, oldcontent, newcontent);
	return true;
}

bool
Node::addChild (Node *child, Node *ref)
{
	// Emit add_child
	if (listeners) {
		for (int i = 0; i < listeners->length; i++) {
			Listener& l(listeners->elements[i]);
			if (l.events->add_child) if (!l.events->add_child (this, child, ref, l.data)) return false;
		}
	}
	child->parent = this;
	if (!ref) {
		child->next = children;
		children = child;
	} else {
		child->next = ref->next;
		ref->next = child;
	}
	// This has to be before emiting or we got id changes messed up
	document->childInserted (this, ref, child);
	// Emit child_added
	bool emitted = false;
	if (listeners) {
		for (int i = 0; i < listeners->length; i++) {
			Listener& l(listeners->elements[i]);
			if (l.events->child_added) {
				l.events->child_added (this, child, ref, l.data);
				emitted = true;
			}
		}
	}
	if (!emitted && parent) parent->emitDownstreamChildAdded (this, child, ref);
	return true;
}

bool
Node::appendChild (Node *child)
{
	Node *ref = children;
	if (ref) {
		while (ref->next) ref = ref->next;
	}
	return addChild (child, ref);
}

bool
Node::removeChild (Node *child)
{
	Node *ref = NULL;
	if (children != child) {
		ref = children;
		while (ref && (ref->next != child)) ref = ref->next;
		if (!ref) return false;
	}
	// Emit remove_child
	if (listeners) {
		for (int i = 0; i < listeners->length; i++) {
			Listener& l(listeners->elements[i]);
			if (l.events->remove_child) if (!l.events->remove_child (this, child, ref, l.data)) return false;
		}
	}
	if (ref) {
		ref->next = child->next;
	} else {
		children = child->next;
	}
	child->next = NULL;
	child->parent = NULL;
	// Emit child_removed
	bool emitted = false;
	if (listeners) {
		for (int i = 0; i < listeners->length; i++) {
			Listener& l(listeners->elements[i]);
			if (l.events->child_removed) {
				l.events->child_removed (this, child, ref, l.data);
				emitted = true;
			}
		}
	}
	if (!emitted && parent) parent->emitDownstreamChildRemoved (this, child, ref);
	// This has to be last because document either deletes child or grabs ownership
	document->childRemoved (this, ref, child);
	return true;
}

bool
Node::relocateChild (Node *child, Node *nref)
{
	Node *cref = NULL;
	if (child != children) {
		cref = children;
		while (cref && (cref->next != child)) cref = cref->next;
		if (!cref) return false;
	}
	// Emit change_order
	if (listeners) {
		for (int i = 0; i < listeners->length; i++) {
			Listener& l(listeners->elements[i]);
			if (l.events->change_order) if (!l.events->change_order (this, child, cref, nref, l.data)) return false;
		}
	}
	if (cref) {
		cref->next = child->next;
	} else {
		children = child->next;
	}
	if (nref) {
		child->next = nref->next;
		nref->next = child;
	} else {
		child->next = children;
		children = child;
	}
	document->childRelocated (this, cref, nref, child);
	// Emit order_changed
	bool emitted = false;
	if (listeners) {
		for (int i = 0; i < listeners->length; i++) {
			Listener& l(listeners->elements[i]);
			if (l.events->order_changed) {
				l.events->order_changed (this, child, cref, nref, l.data);
				emitted = true;
			}
		}
	}
	if (!emitted && parent) parent->emitDownstreamOrderChanged (this, child, cref, nref);
	return true;
}

void
Node::emitDownstreamChildAdded (Node *node, Node *child, Node *ref)
{
	bool emitted = false;
	if (listeners) {
		for (int i = 0; i < listeners->length; i++) {
			Listener& l(listeners->elements[i]);
			if (l.events->downstream_child_added) {
				l.events->downstream_child_added (node, child, ref, l.data);
				emitted = true;
			}
		}
	}
	if (!emitted && parent) parent->emitDownstreamChildAdded (node, child, ref);
}

void
Node::emitDownstreamChildRemoved (Node *node, Node *child, Node *ref)
{
	bool emitted = false;
	if (listeners) {
		for (int i = 0; i < listeners->length; i++) {
			Listener& l(listeners->elements[i]);
			if (l.events->downstream_child_removed) {
				l.events->downstream_child_removed (node, child, ref, l.data);
				emitted = true;
			}
		}
	}
	if (!emitted && parent) parent->emitDownstreamChildRemoved (node, child, ref);
}

void
Node::emitDownstreamAttrChanged (Node *node, const char *key, const char *oldval, const char *newval)
{
	bool emitted = false;
	if (listeners) {
		for (int i = 0; i < listeners->length; i++) {
			Listener& l(listeners->elements[i]);
			if (l.events->downstream_attr_changed) {
				l.events->downstream_attr_changed (node, key, oldval, newval, l.data);
				emitted = true;
			}
		}
	}
	if (!emitted && parent) parent->emitDownstreamAttrChanged (node, key, oldval, newval);
}

void
Node::emitDownstreamContentChanged (Node *node, const char *oldcontent, const char *newcontent)
{
	bool emitted = false;
	if (listeners) {
		for (int i = 0; i < listeners->length; i++) {
			Listener& l(listeners->elements[i]);
			if (l.events->downstream_content_changed) {
				l.events->downstream_content_changed (node, oldcontent, newcontent, l.data);
				emitted = true;
			}
		}
	}
	if (!emitted && parent) parent->emitDownstreamContentChanged (node, oldcontent, newcontent);
}

void
Node::emitDownstreamOrderChanged (Node *node, Node *child, Node *oldref, Node *newref)
{
	bool emitted = false;
	if (listeners) {
		for (int i = 0; i < listeners->length; i++) {
			Listener& l(listeners->elements[i]);
			if (l.events->downstream_order_changed) {
				l.events->downstream_order_changed (node, child, oldref, newref, l.data);
				emitted = true;
			}
		}
	}
	if (!emitted && parent) parent->emitDownstreamOrderChanged (node, child, oldref, newref);
}

unsigned int
Node::getNumChildren (void) const
{
	unsigned int nchildren = 0;
	for (Node *child = children; child; child = child->next) nchildren += 1;
	return nchildren;
}

Node *
Node::getChild (unsigned int childidx) const
{
	unsigned int childpos = 0;
	for (Node *child = children; child; child = child->next) {
		if (childpos == childidx) return child;
		childpos += 1;
	}
	return NULL;
}

Node *
Node::clone (Document *pdocument, bool recursive) const
{
	Node *newnode = new Node(type, pdocument, name);
	if (content) newnode->content = strdup (content);
	if (attributes) newnode->attributes = attributes->duplicate ();
	if (recursive) {
		Node *refchild = NULL;
		for (Node *child = children; child; child = child->next) {
			Node *newchild = child->clone (pdocument, recursive);
			newnode->addChild (newchild, refchild);
			refchild = newchild;
		}
	}
	return newnode;
}

void
Node::addListener (const EventVector *events, void *data)
{
	listeners = ListenerArray::add (listeners, events, data);
}

void
Node::removeListener (void *data)
{
	listeners = ListenerArray::remove (listeners, data);
}

void
Node::AttributeArray::destroy (Node::AttributeArray *aa)
{
	if (!aa) return;
	for (unsigned int i = 0; i < aa->length; i++) {
		free (aa->elements[i].name);
		free (aa->elements[i].value);
	}
	free (aa);
}

Node::AttributeArray *
Node::AttributeArray::ensureSpace (Node::AttributeArray *aa, int ensure)
{
	if (ensure <= 0) return aa;
	if (!aa) {
		int size = (ensure < 4) ? 4 : ensure;
		aa = (AttributeArray *) malloc (sizeof (AttributeArray) + (size - 1) * sizeof (Node::Attribute));
		aa->length = 0;
		aa->size = size;
	} else {
		if ((aa->length + ensure) <= aa->size) return aa;
		aa->size = aa->size << 1;
		if (aa->size < (aa->length + ensure)) aa->size = (aa->length + ensure);
		aa = (AttributeArray *) realloc (aa, sizeof (AttributeArray) + (aa->size - 1) * sizeof (Node::Attribute));
	}
	return aa;
}

char *
Node::AttributeArray::get (const AttributeArray *aa, const char *name)
{
	if (!aa || !name) return NULL;
	for (unsigned int i = 0; i < aa->length; i++) {
		if (!strcmp (name, aa->elements[i].name)) return aa->elements[i].value;
	}
	return NULL;
}

Node::AttributeArray *
Node::AttributeArray::set (AttributeArray *aa, const char *name, const char *value)
{
	if (!name) return aa;
	if (aa) {
		for (unsigned int i = 0; i < aa->length; i++) {
			if (!strcmp (name, aa->elements[i].name)) {
				if (value) {
					// free (aa->elements[i].value);
					aa->elements[i].value = strdup (value);
				} else {
					for (unsigned int j = i + 1; j < aa->length; j++) {
						aa->elements[j - 1] = aa->elements[j];
					}
					aa->length -= 1;
				}
				return aa;
			}
		}
	}
	if (!value) return aa;
	aa = ensureSpace (aa, 1);
	aa->elements[aa->length].name = strdup (name);
	aa->elements[aa->length].value = strdup (value);
	aa->length += 1;
	return aa;
}

Node::AttributeArray *
Node::AttributeArray::duplicate ()
{
	AttributeArray *aa = NULL;
	aa = ensureSpace (aa, length);
	for (unsigned int i = 0; i < length; i++) {
		aa->elements[i].name = strdup (elements[i].name);
		aa->elements[i].value = strdup (elements[i].value);
	}
	aa->length = length;
	return aa;
}

void
Node::ListenerArray::destroy (ListenerArray *la)
{
	if (la) free (la);
}

Node::ListenerArray *
Node::ListenerArray::add (ListenerArray *la, const EventVector *events, void *data)
{
	if (!la) {
		int size = 4;
		la = (ListenerArray *) malloc (sizeof (ListenerArray) + (size - 1) * sizeof (Node::Listener));
		la->length = 0;
		la->size = size;
	} else if ((la->length + 1) > la->size) {
		la->size = la->size << 1;
		la = (ListenerArray *) realloc (la, sizeof (ListenerArray) + (la->size - 1) * sizeof (Node::Listener));
	}
	la->elements[la->length].events = events;
	la->elements[la->length].data = data;
	la->length += 1;
	return la;
}

Node::ListenerArray *
Node::ListenerArray::remove (ListenerArray *la, void *data)
{
	if (!la) return la;
	for (int i = 0; i < la->length; i++) {
		if (la->elements[i].data == data) {
			la->length -= 1;
			if (la->length > 0) la->elements[i] = la->elements[la->length];
		}
	}
	return la;
}

bool
Node::setAttributeInt (const char *name, int value)
{
	char c[16];
	arikkei_itoa ((unsigned char *) c, 16, value);
	return setAttribute (name, c);
}

bool
Node::setAttributeUint (const char *name, unsigned int value)
{
	char c[16];
	arikkei_dtoa_simple ((unsigned char *) c, 16, (double) value, 15, 0, 0);
	return setAttribute (name, c);
}

bool
Node::setAttributeFloat (const char *name, float value, unsigned int precision)
{
	char c[16];
	arikkei_dtoa_exp ((unsigned char *) c, 16, value, precision, 0);
	return setAttribute (name, c);
}

const char *
Node::getContentOrChildText (void)
{
	if (content) return content;
	Node *text = NULL;
	for (Node *child = children; child; child = child->next) {
		if (child->type == TEXT) {
			if (text) return NULL;
			text = child;
		}
	}
	if (!text) return NULL;
	return text->content;
}

bool
Node::collateTexts (void)
{
	if (!content && children && !children->next && (children->type == TEXT)) {
		content = (children->content) ? strdup (children->content) : NULL;
		removeChild (children);
	}
	for (Node *child = children; child; child = child->next) {
		child->collateTexts ();
	}
	return true;
}

bool
Node::expandTexts (void)
{
	if (content) {
		Node *child = new Node(TEXT, document, NULL);
		child->setTextContent (content);
		addChild (child, NULL);
		setTextContent (NULL);
	}
	for (Node *child = children; child; child = child->next) {
		child->expandTexts ();
	}
	return true;
}

} // Namespace Thera

