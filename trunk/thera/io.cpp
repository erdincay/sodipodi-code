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

#include "thera.h"

namespace Thera {

static bool
appendChildNode (Document *doc, Node **current, Node *child)
{
	if (*current) {
		// fprintf (stdout, "Appending %s->%s\n", (*current)->name, child->name);
		(*current)->appendChild (child);
	} else {
		if (doc->root) {
			fprintf (stderr, "appendChildNode: Document already has root\n");
			return false;
		}
		// fprintf (stdout, "Appending ROOT->%s\n", child->name);
		doc->root = child;
	}
	return true;
}

static void
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
				xmlChar *xmlname = xmlTextReaderLocalName (reader);
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
		} else {
			delete child;
		}
	} else if (nodetype == XML_READER_TYPE_TEXT) {
		Node *child = new Node(Node::TEXT, doc, NULL);
		if (appendChildNode (doc, current, child)) {
			xmlChar *value = xmlTextReaderValue (reader);
			child->setTextContent ((const char *) value);
			xmlFree (value);
		} else {
			delete child;
		}
	} else if (nodetype == XML_READER_TYPE_CDATA) {
	} else if (nodetype == XML_READER_TYPE_COMMENT) {
		Node *child = new Node(Node::COMMENT, doc, NULL);
		if (appendChildNode (doc, current, child)) {
			xmlChar *value = xmlTextReaderValue (reader);
			child->setTextContent ((const char *) value);
			xmlFree (value);
		} else {
			delete child;
		}
	} else if (nodetype == XML_READER_TYPE_SIGNIFICANT_WHITESPACE) {
	} else if (nodetype == XML_READER_TYPE_END_ELEMENT) {
		if (!*current) {
			fprintf (stderr, "No current element\n");
			return;
		}
		// if (xmlTextReaderDepth (reader) < 10) {
		// 	for (int i = 0; i < xmlTextReaderDepth (reader); i++) fprintf (stdout, "  ");
		// 	fprintf (stdout, "</%s>\n", (const char *) xmlTextReaderLocalName (reader));
		// }
		// fprintf (stdout, "backing %s->%s\n", (*current)->name, ((*current)->parent) ? (*current)->parent->name : "NONE");
		*current = (*current)->parent;
	} else {
		fprintf (stderr, "Unsupported node type: %d\n", nodetype);
	}
}

Document *
load (const char *filename)
{
	Document *doc = NULL;

	xmlTextReaderPtr reader = xmlNewTextReaderFilename (filename);
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
			fprintf (stderr, "%s : failed to parse\n", filename);
		}
	} else {
		fprintf(stderr, "Unable to open %s\n", filename);
	}

	return doc;
}

unsigned int
saveNode (FILE *ofs, Node *node, int level)
{
	unsigned int wlen = 0;

	for (int i = 0; i < level; i++) wlen += fputs ("  ", ofs);
	wlen += fprintf (ofs, "<%s", node->name);
	for (int i = 0; i < node->getNumAttributes (); i++) {
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
			wlen += fprintf (ofs, "\n");
			while (child) {
				wlen += saveNode (ofs, child, level + 1);
				child = child->getNextSibling ();
			}
			for (int i = 0; i < level; i++) wlen += fputs ("  ", ofs);
		}
		wlen += fprintf (ofs, "</%s>\n", node->name);
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

	wlen += saveNode (ofs, document->root, 0);

	fclose (ofs);

	return wlen;
}

} // Namespace Thera
