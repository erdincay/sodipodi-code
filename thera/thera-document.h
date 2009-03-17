#ifndef __THERA_DOCUMENT_H__
#define __THERA_DOCUMENT_H__

/*
 * LibThera
 *
 * Copyright (C) Lauris Kaplinski 2007
 *
*/

typedef struct _TheraDocument TheraDocument;

#ifdef __cplusplus
extern "C" {
#endif

TheraDocument *thera_document_new (void);

void thera_document_ref (TheraDocument *doc);
void thera_document_unref (TheraDocument *doc);

TheraNode *thera_document_get_root (TheraDocument *doc);

TheraNode *thera_document_new_node (TheraDocument *doc, unsigned int nodetype, const char *name);

/* IO methods */

TheraDocument *thera_document_load (const char *filename);
unsigned int save (TheraDocument *doc, const char *filename);

#ifdef __cplusplus
}
#endif

#endif
