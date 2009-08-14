#ifndef __THERA_DOCUMENT_H__
#define __THERA_DOCUMENT_H__

/*
 * LibThera
 *
 * Copyright (C) Lauris Kaplinski 2007-2009
 *
*/

typedef struct _TheraDocument TheraDocument;

#include <stdlib.h>

#include <thera/thera-node.h>

#ifdef __cplusplus
extern "C" {
#endif

TheraDocument *thera_document_new (const char *rootname);

void thera_document_ref (TheraDocument *doc);
void thera_document_unref (TheraDocument *doc);

TheraNode *thera_document_get_root (TheraDocument *doc);

TheraNode *thera_document_new_node (TheraDocument *doc, unsigned int nodetype, const char *name);

void thera_document_enable_transactions (TheraDocument *doc, unsigned int enabled);
void thera_document_collate_transactions (TheraDocument *doc, unsigned int collate);
void thera_document_finish_transaction (TheraDocument *doc);
void thera_document_cancel_transaction (TheraDocument *doc);
void thera_document_undo (TheraDocument *doc);
void thera_document_redo (TheraDocument *doc);

/* IO methods */

TheraDocument *thera_document_load (const char *filename);
TheraDocument *thera_document_load_from_mem (const unsigned char *cdata, size_t csize);

unsigned int thera_document_save (TheraDocument *doc, const char *filename);

#ifdef __cplusplus
}
#endif

#endif
