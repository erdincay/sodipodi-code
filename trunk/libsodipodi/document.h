#ifndef __SP_DOCUMENT_H__
#define __SP_DOCUMENT_H__

/*
 * Typed SVG document implementation
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 1999-2002 Lauris Kaplinski
 * Copyright (C) 2000-2001 Ximian, Inc.
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#define SP_TYPE_DOCUMENT (sp_document_get_type ())
#define SP_DOCUMENT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), SP_TYPE_DOCUMENT, SPDocument))
#define SP_IS_DOCUMENT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SP_TYPE_DOCUMENT))

typedef struct _SPDocumentClass SPDocumentClass;

#include <libnr/nr-types.h>
#include <thera/thera-node.h>

#include <glib-object.h>

#include <libsodipodi/sodipodi.h>
#include <libsodipodi/url.h>

#define SP_NAMESPACE_SVG "http://www.w3.org/2000/svg"
#define SP_NAMESPACE_SODIPODI "http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd"
#define SP_NAMESPACE_XLINK "http://www.w3.org/1999/xlink"

#define A4_WIDTH_STR "210mm"
#define A4_HEIGHT_STR "297mm"

/* Version type. See SPRoot for more detail */
enum {
	SP_VERSION_SVG,
	SP_VERSION_SODIPODI,
	SP_VERSION_ORIGINAL,
};

typedef struct _SPDocumentPrivate SPDocumentPrivate;

struct _SPDocument {
	GObject object;

	TheraDocument *thedoc;
	TheraNode *theroot;
	SPObject *root;

	/* URI string or NULL */
	unsigned char *uri;
	/* Base part of URI (to resolve relative paths) */
	unsigned char *base;
	/* Name part of URI */
	unsigned char *name;

	/* Dirty tag */
	unsigned int modified : 1;

	/* fixme: remove this */
	SPDocumentPrivate *priv;

	/* Whether to emit mutation signals */
	unsigned int emit_object_signals;
	unsigned int update_requested;

	/* Begin time */
	double begintime;

	/* Handler for resolving relative references */
	SPURLHandler *handler;

	/* unsigned int advertize : 1; */
	/* Last action key */
	/* const guchar *actionkey; */
};

struct _SPDocumentClass {
	GObjectClass parent_class;

	void (* destroy) (SPDocument *document);

	/* Asychrnonus update of document is requested */
	void (* update_request) (SPDocument *doc);

	/* Global signals */
	void (* modified) (SPDocument *document, guint flags);
	void (* uri_set) (SPDocument *document, const guchar *uri);
	void (* resized) (SPDocument *document, gdouble width, gdouble height);
	/* Tree mutation signals */
	/* Tree is guaranteed to be alredy at updated state (including unique id) but objects are not fully built yet */
	void (* object_added) (SPDocument *doc, SPObject *object, SPObject *parent, SPObject *ref);
	void (* object_removed) (SPDocument *doc, SPObject *object, SPObject *parent, SPObject *ref);
	void (* object_relocated) (SPDocument *doc, SPObject *object, SPObject *parent, SPObject *oldref, SPObject *ref);
	/* Asynchronous signal */
	void (* object_modified) (SPDocument *doc, SPObject *object, unsigned int flags);
	/* Timer events */
	void (* begin) (SPDocument *doc, double dtime);
	void (* end) (SPDocument *doc, double dtime);
};

GType sp_document_get_type (void);

/* For object implementations */

void sp_document_def_id (SPDocument * document, const gchar * id, SPObject * object);
void sp_document_undef_id (SPDocument * document, const gchar * id);

void sp_document_invoke_object_added (SPDocument *doc, SPObject *object, SPObject *parent, SPObject *ref);
void sp_document_invoke_object_removed (SPDocument *doc, SPObject *object, SPObject *parent, SPObject *ref);
void sp_document_invoke_object_relocated (SPDocument *doc, SPObject *object, SPObject *parent, SPObject *oldref, SPObject *ref);
void sp_document_invoke_object_modified (SPDocument *doc, SPObject *object, unsigned int flags);

void sp_document_add_identity_change_listener (SPDocument *document, const unsigned char *id, SPObject *object);
void sp_document_remove_identity_change_listener (SPDocument *document, const unsigned char *id, SPObject *object);
void sp_document_remove_identity_change_listeners (SPDocument *document, SPObject *object);
void sp_document_invoke_identity_added (SPDocument *document, const unsigned char *id, SPObject *object);
void sp_document_invoke_identity_removed (SPDocument *document, const unsigned char *id, SPObject *object);

void sp_document_request_update (SPDocument *doc);

/* Resource management */
gboolean sp_document_add_resource (SPDocument *document, const guchar *key, SPObject *object);
gboolean sp_document_remove_resource (SPDocument *document, const guchar *key, SPObject *object);
const GSList *sp_document_get_resource_list (SPDocument *document, const guchar *key);

/*
 * Fetches document from URI, or creates new, if NULL
 * Public document appear in document list
 */

SPDocument *sp_document_new (void);
SPDocument *sp_document_new_from_file (const unsigned char *path);
SPDocument *sp_document_new_from_mem (const unsigned char *buffer, size_t length);

SPDefs *sp_document_get_defs (SPDocument *doc);
float sp_document_get_width (SPDocument *document);
float sp_document_get_height (SPDocument *document);

SPObject *sp_document_get_object_from_id (SPDocument *doc, const unsigned char *id);
SPObject *sp_document_get_object_from_node (SPDocument *doc, TheraNode *node);
#define sp_document_lookup_id sp_document_get_object_from_id

/* Undo & redo */

void sp_document_set_undo_sensitive (SPDocument * document, gboolean sensitive);

#if 0
void sp_document_clear_undo (SPDocument * document);
void sp_document_clear_redo (SPDocument * document);
#endif

/* Tree mutation signals */
/* Each call to ::set_emit_object_signals increases/decreases tag value */
void sp_document_set_emit_object_signals (SPDocument *doc, unsigned int enable);

/* Returns 1 if up-to-date was achieved */
unsigned int sp_document_ensure_up_to_date (SPDocument *doc, unsigned int maxtries);

/* Save all previous actions to stack, as one undo step */
void sp_document_done (SPDocument *document);
/* void sp_document_maybe_done (SPDocument *document, const unsigned char *key); */
/* Cancel (and revert) current unsaved actions */
void sp_document_cancel (SPDocument *document);

/* Undo and redo */
void sp_document_undo (SPDocument * document);
void sp_document_redo (SPDocument * document);

/* Adds repr to document, returning created object (if any) */
/* Items will be added to root (fixme: should be namedview root) */
/* Non-item objects will go to root-level defs group */
SPObject *sp_document_add_node (SPDocument *document, TheraNode *node);

/* Returns the sequence number of object */
unsigned int sp_document_object_sequence_get (SPDocument *doc, SPObject *object);

/*
 * Ideas: How to overcome style invalidation nightmare
 *
 * 1. There is reference request dictionary, that contains
 * objects (styles) needing certain id. Object::build checks
 * final id against it, and invokes necesary methods
 *
 * 2. Removing referenced object is simply prohibited -
 * needs analyse, how we can deal with situations, where
 * we simply want to ungroup etc. - probably we need
 * Repr::reparent method :( [Or was it ;)]
 *
 */

void sp_document_set_uri (SPDocument *document, const unsigned char *uri);
void sp_document_set_size_px (SPDocument *doc, gdouble width, gdouble height);

guint sp_document_get_version (SPDocument *document, guint version_type);

/*
 * Timer
 *
 * Objects are required to use time services only from their parent document
 */

enum {
	SP_TIMER_IDLE,
	SP_TIMER_EXACT
};

/* Begin document and start timer */
double sp_document_begin (SPDocument *doc);
/* End document and stop timer */
double sp_document_end (SPDocument *doc);
/* Get time in seconds from document begin */
double sp_document_get_time (SPDocument *doc);

/* Create and set up timer */
unsigned int sp_document_add_timer (SPDocument *doc, double time, unsigned int klass,
				    unsigned int (* callback) (double, void *),
				    void *data);
/* Remove timer */
void sp_document_remove_timer (SPDocument *doc, unsigned int id);
/* Set up timer */
void sp_document_set_timer (SPDocument *doc, unsigned int id, double time, unsigned int klass);

#endif
