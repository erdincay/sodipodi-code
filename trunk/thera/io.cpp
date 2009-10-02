#define __THERA_IO_CPP__

//
// LibThera
//
// Copyright (C) Lauris Kaplinski 2007-2008
//

#ifdef WIN32
#define _CRT_SECURE_NO_DEPRECATE 1
#pragma warning(disable : 4996)
#endif

#include <stdio.h>

#include <libxml/xmlreader.h>

#include <libarikkei/arikkei-iolib.h>

#include "thera.h"

namespace Thera {

static bool
appendChildNode (Document *doc, Node **current, Node *child)
{
	if (*current) {
		if ((*current)->content) {
			// Create text child
			Node *text = new Node(Node::TEXT, doc, NULL);
			text->setTextContent ((*current)->content);
			(*current)->appendChild (text);
			(*current)->setTextContent (NULL);
		}
		(*current)->appendChild (child);
	} else {
		if (doc->children) {
			Node *ref = doc->children;
			while (ref->next) ref = ref->next;
			ref->next = child;
		} else {
			doc->children = child;
		}
		if (!doc->root && (child->type == Node::ELEMENT)) {
			doc->root = child;
		}
	}
	return true;
}

static Node *
processNode (xmlTextReaderPtr reader, Document *doc, Node **current)
{
	int nodetype = xmlTextReaderNodeType (reader);
	if (nodetype == XML_READER_TYPE_ELEMENT) {
		// xmlChar *localname = xmlTextReaderLocalName (reader);
		xmlChar *xmlname = xmlTextReaderName (reader);
		// if (xmlTextReaderDepth (reader) < 10) {
		// 	for (int i = 0; i < xmlTextReaderDepth (reader); i++) fprintf (stdout, "  ");
		// 	fprintf (stdout, "<%s> %d\n", (const char *) localname, xmlTextReaderIsEmptyElement (reader));
		// }
		Node *child = new Node(Node::ELEMENT, doc, (const char *) xmlname);
		xmlFree (xmlname);
		if (appendChildNode (doc, current, child)) {
			while (xmlTextReaderMoveToNextAttribute (reader)) {
				// xmlChar *localname = xmlTextReaderLocalName (reader);
				// xmlChar *xmlname = xmlTextReaderLocalName (reader);
				xmlChar *xmlname = xmlTextReaderName (reader);
				xmlChar *value = xmlTextReaderValue (reader);
				child->setAttribute ((const char *) xmlname, (const char *) value);
				xmlFree (value);
				xmlFree (xmlname);
			}
			xmlTextReaderMoveToElement (reader);
			if (!xmlTextReaderIsEmptyElement (reader)) {
				// fprintf (stdout, ".");
				*current = child;
			}
			return child;
		} else {
			delete child;
			return NULL;
		}
	} else if (nodetype == XML_READER_TYPE_TEXT) {
		if ((*current) && !(*current)->children) {
			// Simply set context
			xmlChar *value = xmlTextReaderValue (reader);
			(*current)->setTextContent ((const char *) value);
			xmlFree (value);
			return NULL;
		}
		Node *child = new Node(Node::TEXT, doc, NULL);
		if (appendChildNode (doc, current, child)) {
			xmlChar *value = xmlTextReaderValue (reader);
			child->setTextContent ((const char *) value);
			xmlFree (value);
			return child;
		} else {
			delete child;
			return NULL;
		}
	} else if (nodetype == XML_READER_TYPE_CDATA) {
		return NULL;
	} else if (nodetype == XML_READER_TYPE_COMMENT) {
		Node *child = new Node(Node::COMMENT, doc, NULL);
		if (appendChildNode (doc, current, child)) {
			xmlChar *value = xmlTextReaderValue (reader);
			child->setTextContent ((const char *) value);
			xmlFree (value);
			return child;
		} else {
			delete child;
			return NULL;
		}
	} else if (nodetype == XML_READER_TYPE_SIGNIFICANT_WHITESPACE) {
		return NULL;
	} else if (nodetype == XML_READER_TYPE_DOCUMENT_TYPE) {
		return NULL;
	} else if (nodetype == XML_READER_TYPE_END_ELEMENT) {
		if (!*current) {
			fprintf (stderr, "No current element\n");
			return NULL;
		}
		// if (xmlTextReaderDepth (reader) < 10) {
		// 	for (int i = 0; i < xmlTextReaderDepth (reader); i++) fprintf (stdout, "  ");
		// 	fprintf (stdout, "</%s>\n", (const char *) xmlTextReaderLocalName (reader));
		// }
		// fprintf (stdout, "backing %s->%s\n", (*current)->name, ((*current)->parent) ? (*current)->parent->name : "NONE");
		*current = (*current)->parent;
		return NULL;
	} else {
		fprintf (stderr, "Unsupported node type: %d\n", nodetype);
		return NULL;
	}
}

Document *
load (const char *filename)
{
	Document *doc = NULL;

	size_t csize;
	const unsigned char *cdata = arikkei_mmap ((const unsigned char *) filename, &csize, NULL);
	if (cdata) {
		doc = load (cdata, csize, (const unsigned char *) filename);
		arikkei_munmap (cdata, csize);
	}

	return doc;
}

Document *
load (const unsigned char *cdata, size_t csize, const unsigned char *uri)
{
	Document *doc = NULL;

	xmlBuffer b = {
		(xmlChar *) cdata, // Content
		csize, // Use
		csize, // Size
		XML_BUFFER_ALLOC_IMMUTABLE // Alloc
	};

	xmlParserInputBuffer ib = {
		NULL, // Context
		NULL, // Read callback
		NULL, // Close callback
		NULL, // Encoder
		&b, // Buffer
		NULL, // Raw
		-1, // Compressed
		0, // Error
		0 // Raw consumed
	};

	xmlTextReaderPtr reader = xmlNewTextReader (&ib, NULL);
	if (reader != NULL) {
		// fixme: Think about root element (Lauris)
		doc = new Document(NULL);
		Node *current = NULL;

		int ret = xmlTextReaderRead (reader);
		while (ret == 1) {
			processNode (reader, doc, &current);
			ret = xmlTextReaderRead (reader);
		}
		xmlFreeTextReader (reader);
		if (ret != 0) {
			fprintf (stderr, "%s : failed to parse\n", uri);
		}
	} else {
		fprintf(stderr, "Unable to open %s\n", uri);
	}

	return doc;
}

Node *
loadNode (Node *parent, const char *filename)
{
	Node *node = NULL;

	size_t csize;
	const unsigned char *cdata = arikkei_mmap ((const unsigned char *) filename, &csize, NULL);
	if (cdata) {
		node = loadNode (parent, cdata, csize, (const unsigned char *) filename);
		arikkei_munmap (cdata, csize);
	}

	return node;
}

Node *
loadNode (Node *parent, const unsigned char *cdata, size_t csize, const unsigned char *uri)
{
	Node *child = NULL;

	Document *doc = parent->document;

	xmlBuffer b = {
		(xmlChar *) cdata, // Content
		csize, // Use
		csize, // Size
		XML_BUFFER_ALLOC_IMMUTABLE // Alloc
	};

	xmlParserInputBuffer ib = {
		NULL, // Context
		NULL, // Read callback
		NULL, // Close callback
		NULL, // Encoder
		&b, // Buffer
		NULL, // Raw
		-1, // Compressed
		0, // Error
		0 // Raw consumed
	};

	xmlTextReaderPtr reader = xmlNewTextReader (&ib, NULL);
	if (reader != NULL) {
		Node *current = parent;

		int ret = xmlTextReaderRead (reader);
		while (ret == 1) {
			Node *newnode = processNode (reader, doc, &current);
			if (!child && newnode) child = newnode;
			ret = xmlTextReaderRead (reader);
		}
		xmlFreeTextReader (reader);
		if (ret != 0) {
			fprintf (stderr, "%s : failed to parse\n", uri);
		}
	} else {
		fprintf(stderr, "Unable to open %s\n", uri);
	}

	return child;
}

static unsigned int saveNode (FILE *ofs, Node *node, int level);

static unsigned int
saveElement (FILE *ofs, Node *node, int level)
{
	unsigned int wlen = 0;

	for (int i = 0; i < level; i++) wlen += fputs ("  ", ofs);
	wlen += fprintf (ofs, "<%s", node->name);
	for (unsigned int i = 0; i < node->getNumAttributes (); i++) {
		wlen += fprintf (ofs, " %s=\"%s\"", node->getAttributeName (i), node->getAttribute (i));
	}
	const char *content = node->getTextContent ();
	Node *child = node->getFirstChild ();
	if (!content && !child) {
		// Empty node
		wlen += fprintf (ofs, "/>\n");
	} else {
		wlen += fprintf (ofs, ">");
		if (content) {
			wlen += fprintf (ofs, "%s", content);
		}
		if (child) {
			if (child->getType () == Node::ELEMENT) {
				wlen += fprintf (ofs, "\n");
				while (child) {
					wlen += saveNode (ofs, child, level + 1);
					child = child->getNextSibling ();
				}
				for (int i = 0; i < level; i++) wlen += fputs ("  ", ofs);
			} else if (child->getType () == Node::TEXT) {
				wlen += fprintf (ofs, "%s", child->getTextContent ());
			} else if (child->getType () == Node::CDATA) {
				// fixme: What goes here?
			}
		}
		wlen += fprintf (ofs, "</%s>\n", node->name);
	}

	return wlen;
}

static unsigned int
saveNode (FILE *ofs, Node *node, int level)
{
	unsigned int wlen = 0;

	if (node->type == Node::ELEMENT) {
		wlen += saveElement (ofs, node, level);
	} else if (node->type == Node::COMMENT) {
		for (int i = 0; i < level; i++) wlen += fputs ("  ", ofs);
		wlen += fprintf (ofs, "<!--");
		wlen += fprintf (ofs, "%s", node->content);
		wlen += fprintf (ofs, "-->\n");
	} else if (node->type == Node::DOCTYPE) {
		for (int i = 0; i < level; i++) wlen += fputs ("  ", ofs);
		wlen += fprintf (ofs, "<!DOCTYPE ");
		wlen += fprintf (ofs, "%s", node->content);
		wlen += fprintf (ofs, ">\n");
	}

	return wlen;
}

unsigned int
save (const Document *document, const char *filename)
{
	if (!document || !document->root) return 0;

	unsigned int wlen = 0;

	FILE *ofs = fopen (filename, "wb+");
	if (!ofs) return 0;

	wlen += fprintf (ofs, "<?xml version=\"1.0\"?>\n");

	for (Node *child = document->children; child; child = child->next) {
		wlen += saveNode (ofs, child, 0);
	}

	fclose (ofs);

	return wlen;
}

} // Namespace Thera
