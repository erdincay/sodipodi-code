#define __SP_DOCUMENT_C__

/*
 * SVG document implementation
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * Copyright (C) 1999-2002 Lauris Kaplinski
 * Copyright (C) 2000-2001 Ximian, Inc.
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#define noSP_DOCUMENT_DEBUG_IDLE
#define noSP_DOCUMENT_DEBUG_UNDO

#include <string.h>

#include <libarikkei/arikkei-iolib.h>
#include <libarikkei/arikkei-url.h>

#include <thera/thera-document.h>

#include "marshal.h"
#include "factory.h"
#include "sp-root.h"

#include "config.h"
#include "intl.h"
#include "preferences.h"

#include "document.h"

#define SP_DOCUMENT_UPDATE_PRIORITY (G_PRIORITY_HIGH_IDLE - 1)

#define SP_DOCUMENT_DEFS(d) ((SPObject *) SP_ROOT (SP_DOCUMENT_ROOT (d))->defs)

typedef struct _IDListener IDListener;
typedef struct _SPDocTimer SPDocTimer;

struct _IDListener {
	char *id;
	GSList *objects;
};

struct _SPDocumentPrivate {
	/* Id dictionary */
	GHashTable *iddef;
	/* Identity change listeners */
	GHashTable *idlistenerdict;
	GSList *idlisteners;

	/* Resources */
	/* It is GHashTable of GSLists */
	GHashTable *resources;

	/* If we save actions to undo stack */
	/* guint sensitive: 1; */

	/* Timers */
	unsigned int timers_size;
	SPDocTimer *timers;
	/* Queue entry points */
	GSList *itqueue;
	GSList *etqueue;
};

enum {
	DESTROY,
	UPDATE_REQUEST,
	MODIFIED,
	URI_SET,
	RESIZED,
	OBJECT_ADDED,
	OBJECT_REMOVED,
	OBJECT_RELOCATED,
	OBJECT_MODIFIED,
	BEGIN,
	END,
	LAST_SIGNAL
};

static const unsigned char *sp_svg_doctype_str = "svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\"\n\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\"";
static const unsigned char *sp_comment_str = " Created with Sodipodi (\"http://www.sodipodi.com/\") ";

static void sp_document_class_init (SPDocumentClass *klass);
static void sp_document_init (SPDocument *document);
static void sp_document_dispose (GObject *object);
static void sp_document_finalize (GObject *object);

static gint sp_document_idle_handler (gpointer data);

gboolean sp_document_resource_list_free (gpointer key, gpointer value, gpointer data);

#ifdef SP_DOCUMENT_DEBUG_UNDO
static gboolean sp_document_warn_undo_stack (SPDocument *doc);
#endif

static GObjectClass * parent_class;
static guint signals[LAST_SIGNAL] = {0};
static gint doc_count = 0;

GType
sp_document_get_type (void)
{
	static GType type = 0;
	if (!type) {
		GTypeInfo info = {
			sizeof (SPDocumentClass),
			NULL, NULL,
			(GClassInitFunc) sp_document_class_init,
			NULL, NULL,
			sizeof (SPDocument),
			4,
			(GInstanceInitFunc) sp_document_init,
		};
		type = g_type_register_static (G_TYPE_OBJECT, "SPDocument", &info, 0);
	}
	return type;
}

static void
sp_document_class_init (SPDocumentClass * klass)
{
	GObjectClass * object_class;

	object_class = (GObjectClass *) klass;

	parent_class = g_type_class_peek_parent (klass);

	object_class->dispose = sp_document_dispose;
	object_class->finalize = sp_document_finalize;

	signals[DESTROY] =  g_signal_new ("destroy",
					    G_TYPE_FROM_CLASS (klass),
					    G_SIGNAL_RUN_FIRST,
					    G_STRUCT_OFFSET (SPDocumentClass, destroy),
					    NULL, NULL,
					    sp_marshal_NONE__NONE,
					    G_TYPE_NONE, 0);
	signals[UPDATE_REQUEST] =   g_signal_new ("update_request",
					    G_TYPE_FROM_CLASS (klass),
					    G_SIGNAL_RUN_FIRST,
						G_STRUCT_OFFSET (SPDocumentClass, update_request),
					    NULL, NULL,
					    sp_marshal_NONE__NONE,
					    G_TYPE_NONE, 0);
	signals[MODIFIED] = g_signal_new ("modified",
					    G_TYPE_FROM_CLASS (klass),
					    G_SIGNAL_RUN_FIRST,
					    G_STRUCT_OFFSET (SPDocumentClass, modified),
					    NULL, NULL,
					    sp_marshal_NONE__UINT,
					    G_TYPE_NONE, 1,
					    G_TYPE_UINT);
	signals[URI_SET] =    g_signal_new ("uri_set",
					    G_TYPE_FROM_CLASS (klass),
					    G_SIGNAL_RUN_FIRST,
					    G_STRUCT_OFFSET (SPDocumentClass, uri_set),
					    NULL, NULL,
					    sp_marshal_NONE__POINTER,
					    G_TYPE_NONE, 1,
					    G_TYPE_POINTER);
	signals[RESIZED] =    g_signal_new ("resized",
					    G_TYPE_FROM_CLASS (klass),
					    G_SIGNAL_RUN_FIRST,
					    G_STRUCT_OFFSET (SPDocumentClass, uri_set),
					    NULL, NULL,
					    sp_marshal_NONE__DOUBLE_DOUBLE,
					    G_TYPE_NONE, 2,
					    G_TYPE_DOUBLE, G_TYPE_DOUBLE);
	signals[OBJECT_ADDED] = g_signal_new ("object_added",
					    G_TYPE_FROM_CLASS (klass),
					    G_SIGNAL_RUN_FIRST,
					    G_STRUCT_OFFSET (SPDocumentClass, object_added),
					    NULL, NULL,
					    sp_marshal_NONE__OBJECT_OBJECT_OBJECT,
					    G_TYPE_NONE, 3,
					    G_TYPE_OBJECT, G_TYPE_OBJECT, G_TYPE_OBJECT);
	signals[OBJECT_REMOVED] = g_signal_new ("object_removed",
					    G_TYPE_FROM_CLASS (klass),
					    G_SIGNAL_RUN_FIRST,
					    G_STRUCT_OFFSET (SPDocumentClass, object_removed),
					    NULL, NULL,
						sp_marshal_NONE__OBJECT_OBJECT_OBJECT,
					    G_TYPE_NONE, 3,
					    G_TYPE_OBJECT, G_TYPE_OBJECT, G_TYPE_OBJECT);
	signals[OBJECT_RELOCATED] = g_signal_new ("object_relocated",
					    G_TYPE_FROM_CLASS (klass),
					    G_SIGNAL_RUN_FIRST,
					    G_STRUCT_OFFSET (SPDocumentClass, object_relocated),
					    NULL, NULL,
					    sp_marshal_NONE__OBJECT_OBJECT_OBJECT_OBJECT,
					    G_TYPE_NONE, 4,
					    G_TYPE_OBJECT, G_TYPE_OBJECT, G_TYPE_OBJECT, G_TYPE_OBJECT);
	signals[OBJECT_MODIFIED] = g_signal_new ("object_modified",
					    G_TYPE_FROM_CLASS (klass),
					    G_SIGNAL_RUN_FIRST,
					    G_STRUCT_OFFSET (SPDocumentClass, object_modified),
					    NULL, NULL,
					    sp_marshal_NONE__POINTER_UINT,
					    G_TYPE_NONE, 2,
					    G_TYPE_POINTER, G_TYPE_UINT);
	signals[BEGIN] =      g_signal_new ("begin",
					    G_TYPE_FROM_CLASS (klass),
					    G_SIGNAL_RUN_FIRST,
					    G_STRUCT_OFFSET (SPDocumentClass, begin),
					    NULL, NULL,
					    sp_marshal_NONE__DOUBLE,
					    G_TYPE_NONE, 1,
					    G_TYPE_DOUBLE);
	signals[END] =        g_signal_new ("end",
					    G_TYPE_FROM_CLASS (klass),
					    G_SIGNAL_RUN_FIRST,
					    G_STRUCT_OFFSET (SPDocumentClass, end),
					    NULL, NULL,
					    sp_marshal_NONE__DOUBLE,
					    G_TYPE_NONE, 1,
					    G_TYPE_DOUBLE);
}

static void
sp_document_init (SPDocument *doc)
{
	doc->priv = g_new0 (SPDocumentPrivate, 1);

	doc->priv->iddef = g_hash_table_new (g_str_hash, g_str_equal);
	doc->priv->idlistenerdict  = g_hash_table_new (g_str_hash, g_str_equal);

	doc->priv->resources = g_hash_table_new (g_str_hash, g_str_equal);
}

static void
sp_document_dispose (GObject *object)
{
	SPDocument *doc;

	doc = SP_DOCUMENT(object);

	g_signal_emit (G_OBJECT (doc), signals [DESTROY], 0);

	while (doc->priv->itqueue) {
		/* free (priv->itqueue->data); */
		doc->priv->itqueue = g_slist_remove (doc->priv->itqueue, doc->priv->itqueue->data);
	}
	while (doc->priv->etqueue) {
		/* free (priv->etqueue->data); */
		doc->priv->etqueue = g_slist_remove (doc->priv->etqueue, doc->priv->etqueue->data);
	}
	if (doc->priv->timers) {
		free (doc->priv->timers);
		doc->priv->timers = NULL;
	}

#ifdef ACTIVE_DOCUMENT
		if (priv->partial) {
			sp_repr_free_log (priv->partial);
			priv->partial = NULL;
		}
#endif

	thera_document_enable_transactions (doc->thedoc, FALSE);

	if (doc->root) {
		sp_object_invoke_release (doc->root, NULL, NULL);
		g_object_unref (G_OBJECT (doc->root));
		doc->root = NULL;
	}

	while (doc->priv->idlisteners) {
		IDListener *listener;
		listener = (IDListener *) doc->priv->idlisteners->data;
		g_free (listener->id);
		g_slist_free (listener->objects);
		doc->priv->idlisteners = g_slist_remove (doc->priv->idlisteners, listener);
		g_free (listener);
	}
	if (doc->priv->idlistenerdict) {
		g_hash_table_destroy (doc->priv->idlistenerdict);
		doc->priv->idlistenerdict = NULL;
	}
	if (doc->priv->iddef) {
		g_hash_table_destroy (doc->priv->iddef);
		doc->priv->iddef = NULL;
	}

	if (doc->thedoc) thera_document_unref (doc->thedoc);

	/* Free resources */
	if (doc->priv->resources) {
		g_hash_table_foreach_remove (doc->priv->resources, sp_document_resource_list_free, doc);
		g_hash_table_destroy (doc->priv->resources);
		doc->priv->resources = NULL;
	}

	if (doc->handler) {
		sp_url_handler_release (doc->handler);
		doc->handler = NULL;
	}
	if (doc->name) {
		free (doc->name);
		doc->name = NULL;
	}
	if (doc->base) {
		free (doc->base);
		doc->base = NULL;
	}
	if (doc->uri) {
		free (doc->uri);
		doc->uri = NULL;
	}

	G_OBJECT_CLASS (parent_class)->dispose (object);
}

static void
sp_document_finalize (GObject *object)
{
	SPDocument *doc;

	doc = SP_DOCUMENT(object);

	g_free (doc->priv);

	G_OBJECT_CLASS (parent_class)->finalize (object);
}

/* For object implementations */

void
sp_document_def_id (SPDocument * document, const gchar * id, SPObject * object)
{
	g_assert (g_hash_table_lookup (document->priv->iddef, id) == NULL);

	g_hash_table_insert (document->priv->iddef, (gchar *) id, object);
}

void
sp_document_undef_id (SPDocument * document, const gchar * id)
{
	g_assert (g_hash_table_lookup (document->priv->iddef, id) != NULL);

	g_hash_table_remove (document->priv->iddef, id);
}

void
sp_document_invoke_object_added (SPDocument *doc, SPObject *object, SPObject *parent, SPObject *ref)
{
	if (doc->emit_object_signals) {
		g_signal_emit (G_OBJECT (doc), signals [OBJECT_ADDED], 0, object, parent, ref);
	}
}

void
sp_document_invoke_object_removed (SPDocument *doc, SPObject *object, SPObject *parent, SPObject *ref)
{
	if (doc->emit_object_signals) {
		g_signal_emit (G_OBJECT (doc), signals [OBJECT_REMOVED], 0, object, parent, ref);
	}
}

void
sp_document_invoke_object_relocated (SPDocument *doc, SPObject *object, SPObject *parent, SPObject *oldref, SPObject *ref)
{
	if (doc->emit_object_signals) {
		g_signal_emit (G_OBJECT (doc), signals [OBJECT_RELOCATED], 0, object, parent, oldref, ref);
	}
}

void
sp_document_invoke_object_modified (SPDocument *doc, SPObject *object, unsigned int flags)
{
	if (doc->emit_object_signals) {
		g_signal_emit (G_OBJECT (doc), signals [OBJECT_MODIFIED], 0, object, flags);
	}
}

void
sp_document_add_identity_change_listener (SPDocument *document, const unsigned char *id, SPObject *object)
{
	IDListener *listener;
	listener = g_hash_table_lookup (document->priv->idlistenerdict, id);
	if (!listener) {
		listener = g_new (IDListener, 1);
		listener->id = g_strdup (id);
		listener->objects = NULL;
		document->priv->idlisteners = g_slist_prepend (document->priv->idlisteners, listener);
		g_hash_table_insert (document->priv->idlistenerdict, listener->id, listener);
	}
	listener->objects = g_slist_prepend (listener->objects, object);
}

void
sp_document_remove_identity_change_listener (SPDocument *document, const unsigned char *id, SPObject *object)
{
	IDListener *listener;
	listener = g_hash_table_lookup (document->priv->idlistenerdict, id);
	if (!listener) return;
	while (g_slist_find (listener->objects, object)) {
		listener->objects = g_slist_remove (listener->objects, object);
	}
	if (!listener->objects) {
		document->priv->idlisteners = g_slist_remove (document->priv->idlisteners, listener);
		g_hash_table_remove (document->priv->idlistenerdict, listener->id);
		g_free (listener->id);
		g_free (listener);
	}
}

void
sp_document_remove_identity_change_listeners (SPDocument *document, SPObject *object)
{
	GSList *l, *empty;
	empty = NULL;
	for (l = document->priv->idlisteners; l; l = l->next) {
		IDListener *listener = (IDListener *) l->data;
		while (g_slist_find (listener->objects, object)) {
			listener->objects = g_slist_remove (listener->objects, object);
		}
		if (!listener->objects) empty = g_slist_prepend (empty, listener);
	}
	while (empty) {
		IDListener *listener = (IDListener *) l->data;
		empty = g_slist_remove (empty, listener);
		document->priv->idlisteners = g_slist_remove (document->priv->idlisteners, listener);
		g_hash_table_remove (document->priv->idlistenerdict, listener->id);
		g_free (listener->id);
		g_free (listener);
	}
}

void
sp_document_invoke_identity_added (SPDocument *document, const unsigned char *id, SPObject *object)
{
	IDListener *listener;
	GSList *l;
	listener = g_hash_table_lookup (document->priv->idlistenerdict, id);
	if (!listener) return;
	for (l = listener->objects; l; l = l->next) {
		sp_object_invoke_identity_added (SP_OBJECT(l->data), id, object);
	}
}

void
sp_document_invoke_identity_removed (SPDocument *document, const unsigned char *id, SPObject *object)
{
	IDListener *listener;
	GSList *l;
	listener = g_hash_table_lookup (document->priv->idlistenerdict, id);
	if (!listener) return;
	for (l = listener->objects; l; l = l->next) {
		sp_object_invoke_identity_removed (SP_OBJECT(l->data), id, object);
	}
}

void
sp_document_request_update (SPDocument *doc)
{
	if (!doc->update_requested) {
		g_signal_emit (G_OBJECT (doc), signals [UPDATE_REQUEST], 0);
		doc->update_requested = 1;
	}
}

/* Generic access */

static SPDocument *
sp_document_create (TheraDocument *thedoc, const unsigned char *address)
{
	ArikkeiURL url;
	SPDocument *document;
	TheraNode *theroot, *doctype, *comment;
	unsigned int version;
	GType type;

	if (!arikkei_url_setup (&url, address, NULL)) {
		g_warning ("sp_document_create: Invalid document URL: %s", url);
		return NULL;
	}

	theroot = thera_document_get_root (thedoc);

	document = g_object_new (SP_TYPE_DOCUMENT, NULL);

	document->thedoc = thedoc;
	document->theroot = theroot;

	document->uri = strdup (url.address);
	document->base = strdup (url.base);
	document->name = strdup (url.filename);

	document->handler = sp_url_handler_get (document->uri);

	type = sp_object_type_lookup (thera_node_get_name (document->theroot));
	g_assert (g_type_is_a (type, SP_TYPE_ROOT));
	document->root = g_object_new (type, 0);
	g_assert (document->root != NULL);
	sp_object_invoke_build (document->root, document, document->theroot, NULL, NULL, FALSE);
	sp_document_invoke_object_added (document, document->root, NULL, NULL);

#if 0
	/* fixme: Is this correct here? (Lauris) */
	sp_document_ensure_up_to_date (document);
#endif

	/* Ensure that we have proper header */
	doctype = thera_document_get_first_node (thedoc);
	if (thera_node_get_type (doctype) != THERA_NODE_DOCTYPE) {
		doctype = NULL;
	} else {
		const char *content;
		content = thera_node_get_text_content (doctype);
		if (!content || strcmp (content, sp_svg_doctype_str)) {
			doctype = NULL;
		}
	}
	if (!doctype) {
		doctype = thera_document_new_node (thedoc, THERA_NODE_DOCTYPE, NULL);
		thera_node_set_text_content (doctype, sp_svg_doctype_str);
		thera_document_add_node (thedoc, doctype, NULL);
	}
	comment = thera_node_get_next_sibling (doctype);
	if (thera_node_get_type (comment) != THERA_NODE_COMMENT) {
		comment = NULL;
	} else {
		const char *content;
		content = thera_node_get_text_content (comment);
		if (!content || strcmp (content, sp_comment_str)) {
			comment = NULL;
		}
	}
	if (!comment) {
		comment = thera_document_new_node (thedoc, THERA_NODE_COMMENT, NULL);
		thera_node_set_text_content (comment, sp_comment_str);
		thera_document_add_node (thedoc, comment, doctype);
	}

	version = SP_ROOT (document->root)->sodipodi;

	/* fixme: Not sure about this, but lets assume ::build updates */
	thera_node_set_attribute (document->theroot, "sodipodi:version", LIBSODIPODI_VERSION);
	/* fixme: Again, I moved these here to allow version determining in ::build (Lauris) */
	/* A quick hack to get namespaces into doc */
	thera_node_set_attribute (document->theroot, "xmlns", SP_NAMESPACE_SVG);
	thera_node_set_attribute (document->theroot, "xmlns:sodipodi", SP_NAMESPACE_SODIPODI);
	thera_node_set_attribute (document->theroot, "xmlns:xlink", SP_NAMESPACE_XLINK);
	/* End of quick hack */
	/* Quick hack 2 - get default image size into document */
	if (!thera_node_get_attribute (document->theroot, "width")) thera_node_set_attribute (document->theroot, "width", A4_WIDTH_STR);
	if (!thera_node_get_attribute (document->theroot, "height")) thera_node_set_attribute (document->theroot, "height", A4_HEIGHT_STR);
	/* End of quick hack 2 */
	/* Quick hack 3 - Set uri attributes */
	/* fixme: Think, what this means for images (Lauris) */
	thera_node_set_attribute (document->theroot, "sodipodi:docname", document->uri);
	thera_node_set_attribute (document->theroot, "sodipodi:docbase", document->base);

	/* Namedviews */
	/* fixme: Move to sodipodi? */
	if (!sp_item_group_get_child_by_name ((SPGroup *) document->root, NULL, "sodipodi:namedview")) {
		TheraNode *r;
		r = sp_config_node_get ("template.sodipodi:namedview", FALSE);
		if (!r || strcmp (thera_node_get_name (r), "sodipodi:namedview")) {
			r = thera_document_new_node (document->thedoc, THERA_NODE_ELEMENT, "sodipodi:namedview");
		} else {
			r = thera_node_clone (r, document->thedoc, TRUE);
		}
		thera_node_set_attribute (r, "id", "base");
		thera_node_add_child (document->theroot, r, NULL);
	}

	/* Defs */
	/* fixme: Move to sodipodi? */
	if (!SP_ROOT (document->root)->defs) {
		TheraNode *r;
		r = thera_document_new_node (document->thedoc, THERA_NODE_ELEMENT, "defs");
		thera_node_add_child (document->theroot, r, NULL);
		g_assert (SP_ROOT (document->root)->defs);
	}

	/* fixme: I think we do not want to make all SVG files dirty by adding spns crap (Lauris) */
	document->modified = 0;

	return document;
}

static TheraDocument *
sp_document_load_xml (const unsigned char *cdata, size_t csize)
{
	TheraDocument *thedoc;
	TheraNode *theroot;

	thedoc = thera_document_load_from_mem (cdata, csize);
	if (!thedoc) return NULL;

	theroot = thera_document_get_root (thedoc);
	if (!theroot || strcmp (thera_node_get_name (theroot), "svg")) {
		thera_document_unref (thedoc);
		return NULL;
	}

	return thedoc;
}

SPDocument *
sp_document_new (void)
{
	TheraDocument *thedoc;
	SPDocument *doc;
	gchar *url;

	thedoc = thera_document_new ("svg");

	url = g_strdup_printf (_("none:New document %d"), ++doc_count);
	doc = sp_document_create (thedoc, url);
	g_free (url);

	return doc;
}

SPDocument *
sp_document_new_from_file (const unsigned char *path)
{
	size_t csize;
	const unsigned char *cdata;
	TheraDocument *thedoc;
	unsigned char *url;
	SPDocument *doc;

	g_return_val_if_fail (path != NULL, NULL);

	cdata = arikkei_mmap (path, &csize, NULL);
	if (!cdata) return NULL;
	thedoc = sp_document_load_xml (cdata, csize);
	arikkei_munmap (cdata, csize);
	if (!thedoc) return NULL;

	url = sp_build_canonic_url (path);
	doc = sp_document_create (thedoc, url);
	free (url);

	return doc;
}

SPDocument *
sp_document_new_from_mem (const unsigned char *cdata, size_t csize)
{
	TheraDocument *thedoc;
	SPDocument *doc;
	gchar *url;

	thedoc = sp_document_load_xml (cdata, csize);
	if (!thedoc) return NULL;

	url = g_strdup_printf (_("none:Memory document %d"), ++doc_count);
	doc = sp_document_create (thedoc, url);
	g_free (url);

	return doc;
}

void
sp_document_set_emit_object_signals (SPDocument *doc, unsigned int enable)
{
	if (enable) {
		doc->emit_object_signals += 1;
	} else {
		g_return_if_fail (doc->emit_object_signals > 0);
		doc->emit_object_signals -= 1;
	}
}

void
sp_document_set_uri (SPDocument *doc, const unsigned char *uri)
{
	ArikkeiURL url;

	g_return_if_fail (doc != NULL);
	g_return_if_fail (SP_IS_DOCUMENT (doc));
	g_return_if_fail (uri != NULL);

	g_return_if_fail (arikkei_url_setup (&url, uri, NULL));

	if (doc->name) {
		free (doc->name);
		doc->name = NULL;
	}
	if (doc->base) {
		free (doc->base);
		doc->base = NULL;
	}
	if (doc->uri) {
		free (doc->uri);
		doc->uri = NULL;
	}
	doc->uri = strdup (url.address);
	doc->base = strdup (url.base);
	doc->name = strdup (url.filename);
	arikkei_url_release (&url);

	if (doc->handler) {
		sp_url_handler_release (doc->handler);
	}
	doc->handler = sp_url_handler_get (doc->uri);

	g_signal_emit (G_OBJECT (doc), signals [URI_SET], 0, doc->uri);
}

void
sp_document_set_size_px (SPDocument *doc, gdouble width, gdouble height)
{
	g_return_if_fail (doc != NULL);
	g_return_if_fail (SP_IS_DOCUMENT (doc));
	g_return_if_fail (width > 0.001);
	g_return_if_fail (height > 0.001);

	g_signal_emit (G_OBJECT (doc), signals [RESIZED], 0, width / 1.25, height / 1.25);
}

SPObject *
sp_document_get_object_from_id (SPDocument *doc, const unsigned char *id)
{
	g_return_val_if_fail (doc != NULL, NULL);
	g_return_val_if_fail (SP_IS_DOCUMENT (doc), NULL);
	g_return_val_if_fail (id != NULL, NULL);

	return g_hash_table_lookup (doc->priv->iddef, id);
}

SPObject *
sp_document_get_object_from_node (SPDocument *doc, TheraNode *node)
{
	const unsigned char *id;
	g_return_val_if_fail (doc != NULL, NULL);
	g_return_val_if_fail (SP_IS_DOCUMENT (doc), NULL);
	g_return_val_if_fail (node != NULL, NULL);

	id = thera_node_get_attribute (node, "id");
	return sp_document_get_object_from_id (doc, id);
}

unsigned int
sp_document_ensure_up_to_date (SPDocument *doc, unsigned int maxtries)
{
	while ((maxtries > 0) && (doc->root->uflags || doc->root->mflags)) {
		if (doc->root->uflags) {
			SPItemCtx ctx;
			ctx.ctx.flags = 0;
			nr_matrix_d_set_identity (&ctx.i2doc);
			/* Set up viewport in case svg has it defined as percentages */
			ctx.vp.x0 = 0.0;
			ctx.vp.y0 = 0.0;
			ctx.vp.x1 = 21.0 / 2.54 * 72.0 * 1.25;
			ctx.vp.y1 = 29.7 / 2.54 * 72.0 * 1.25;
			nr_matrix_d_set_identity (&ctx.i2vp);
			sp_object_invoke_update (doc->root, (SPCtx *) &ctx, 0);
		}
		if (doc->root->mflags) {
			unsigned int flags = doc->root->mflags;
			sp_object_invoke_modified (doc->root, 0);

			g_signal_emit (G_OBJECT (doc), signals [MODIFIED], 0, flags);
		}
		maxtries -= 1;
	}
	doc->update_requested = 0;

	return !doc->root->uflags && !doc->root->mflags;
}

SPObject *
sp_document_add_node (SPDocument *document, TheraNode *node)
{
	GType type;

	g_return_val_if_fail (document != NULL, NULL);
	g_return_val_if_fail (SP_IS_DOCUMENT (document), NULL);
	g_return_val_if_fail (node != NULL, NULL);

	type = sp_node_type_lookup (node);

	if (g_type_is_a (type, SP_TYPE_ITEM)) {
		thera_node_append_child (document->theroot, node);
	} else if (g_type_is_a (type, SP_TYPE_OBJECT)) {
		SPDefs *defs = sp_document_get_defs (document);
		thera_node_append_child (((SPObject *) defs)->node, node);
	}

	return sp_document_lookup_id (document, thera_node_get_attribute (node, "id"));
}

/* Returns the sequence number of object */

unsigned int
sp_document_object_sequence_get (SPDocument *doc, SPObject *object)
{
	unsigned int seq;

	seq = 0;

	sp_object_invoke_sequence (doc->root, object, &seq);

	return seq;
}

/* Resource management */

gboolean
sp_document_add_resource (SPDocument *document, const guchar *key, SPObject *object)
{
	GSList *rlist;

	g_return_val_if_fail (document != NULL, FALSE);
	g_return_val_if_fail (SP_IS_DOCUMENT (document), FALSE);
	g_return_val_if_fail (key != NULL, FALSE);
	g_return_val_if_fail (*key != '\0', FALSE);
	g_return_val_if_fail (object != NULL, FALSE);
	g_return_val_if_fail (SP_IS_OBJECT (object), FALSE);

	rlist = g_hash_table_lookup (document->priv->resources, key);
	g_return_val_if_fail (!g_slist_find (rlist, object), FALSE);
	rlist = g_slist_prepend (rlist, object);
	g_hash_table_insert (document->priv->resources, (gpointer) key, rlist);

	return TRUE;
}

gboolean
sp_document_remove_resource (SPDocument *document, const guchar *key, SPObject *object)
{
	GSList *rlist;

	g_return_val_if_fail (document != NULL, FALSE);
	g_return_val_if_fail (SP_IS_DOCUMENT (document), FALSE);
	g_return_val_if_fail (key != NULL, FALSE);
	g_return_val_if_fail (*key != '\0', FALSE);
	g_return_val_if_fail (object != NULL, FALSE);
	g_return_val_if_fail (SP_IS_OBJECT (object), FALSE);

	rlist = g_hash_table_lookup (document->priv->resources, key);
	g_return_val_if_fail (rlist != NULL, FALSE);
	g_return_val_if_fail (g_slist_find (rlist, object), FALSE);
	rlist = g_slist_remove (rlist, object);
	g_hash_table_insert (document->priv->resources, (gpointer) key, rlist);

	return TRUE;
}

const GSList *
sp_document_get_resource_list (SPDocument *document, const guchar *key)
{
	g_return_val_if_fail (document != NULL, NULL);
	g_return_val_if_fail (SP_IS_DOCUMENT (document), NULL);
	g_return_val_if_fail (key != NULL, NULL);
	g_return_val_if_fail (*key != '\0', NULL);

	return g_hash_table_lookup (document->priv->resources, key);
}

/* Helpers */

gboolean
sp_document_resource_list_free (gpointer key, gpointer value, gpointer data)
{
	g_slist_free ((GSList *) value);
	return TRUE;
}

guint
sp_document_get_version (SPDocument *document, guint version_type)
{
	SPRoot *root;

	g_return_val_if_fail (document != NULL, 0);
	g_return_val_if_fail (SP_IS_DOCUMENT (document), 0);
	g_return_val_if_fail (document->root != NULL, 0);

	root = SP_ROOT (document->root);

	switch (version_type)
	{
	case SP_VERSION_SVG:
		return root->svg;
	case SP_VERSION_SODIPODI:
		return root->sodipodi;
	case SP_VERSION_ORIGINAL:
		return root->original;
	default:
		g_assert_not_reached ();
	}

	return 0;
}

SPDefs *
sp_document_get_defs (SPDocument *doc)
{
	SPRoot *root;

	root = SP_ROOT (doc->root);

	return root->defs;
}

float
sp_document_get_width (SPDocument * document)
{
	g_return_val_if_fail (document != NULL, 0.0);
	g_return_val_if_fail (SP_IS_DOCUMENT (document), 0.0);
	g_return_val_if_fail (document->priv != NULL, 0.0);
	g_return_val_if_fail (document->root != NULL, 0.0);

	return SP_ROOT (document->root)->group.width.computed / 1.25f;
}

float
sp_document_get_height (SPDocument * document)
{
	g_return_val_if_fail (document != NULL, 0.0);
	g_return_val_if_fail (SP_IS_DOCUMENT (document), 0.0);
	g_return_val_if_fail (document->priv != NULL, 0.0);
	g_return_val_if_fail (document->root != NULL, 0.0);

	return SP_ROOT (document->root)->group.height.computed / 1.25f;
}

void
sp_document_set_undo_sensitive (SPDocument *document, gboolean sensitive)
{
	thera_document_enable_transactions (document->thedoc, sensitive);
}

void
sp_document_done (SPDocument *document)
{
	thera_document_finish_transaction (document->thedoc);
}

void
sp_document_undo (SPDocument * document)
{
	thera_document_undo (document->thedoc);
}

void
sp_document_redo (SPDocument * document)
{
	thera_document_redo (document->thedoc);
}

/*
 * Timer
 *
 * Objects are required to use time services only from their parent document
 */

#ifdef WIN32
#include <sys/timeb.h>
#include <time.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

static double
sp_document_get_abstime (void)
{
	GTimeVal tv;

	g_get_current_time (&tv);

	return (tv.tv_sec + tv.tv_usec / 1000000.0);
}

/* Begin document and start timer */
double
sp_document_begin (SPDocument *doc)
{
	g_return_val_if_fail (doc->begintime == 0.0, 0.0);
	doc->begintime = sp_document_get_abstime ();
	g_signal_emit (G_OBJECT (doc), signals [BEGIN], 0, doc->begintime);
	return doc->begintime;
}

/* End document and stop timer */
double
sp_document_end (SPDocument *doc)
{
	double endtime;
	g_return_val_if_fail (doc->begintime != 0.0, 0.0);
	endtime = sp_document_get_abstime ();
	g_signal_emit (G_OBJECT (doc), signals [BEGIN], 0, endtime);
	doc->begintime = 0.0;
	return endtime;
}

/* Get time in seconds from document begin */
double
sp_document_get_time (SPDocument *document)
{
	return sp_document_get_abstime () - document->begintime;
}

struct _SPDocTimer {
	double time;
	unsigned int flags : 1;
	unsigned int awake : 1;
	unsigned int (* callback) (double, void *);
	void *data;
};

gint
sp_doc_timer_compare (gconstpointer a, gconstpointer b)
{
	SPDocTimer *ta, *tb;
	ta = (SPDocTimer *) a;
	tb = (SPDocTimer *) b;
	if (ta->time < tb->time) return -1;
	if (ta->time > tb->time) return 1;
	return 0;
}

/* Create and set up timer */
unsigned int
sp_document_add_timer (SPDocument *doc, double time, unsigned int flags,
		       unsigned int (* callback) (double, void *),
		       void *data)
{
	SPDocumentPrivate *priv;
	SPDocTimer *timer;
	unsigned int id;
	priv = doc->priv;
	for (id = 0; id < priv->timers_size; id++) {
		if (!priv->timers[id].callback) break;
	}
	if (id >= priv->timers_size) {
		priv->timers_size = MIN ((priv->timers_size << 1), 4);
		priv->timers = realloc (priv->timers, priv->timers_size * sizeof (SPDocTimer));
		memset (priv->timers + id, 0, (priv->timers_size - id) * sizeof (SPDocTimer));
	}
	timer = &priv->timers[id];
	timer->time = time;
	timer->flags = flags;
	timer->awake = 1;
	timer->callback = callback;
	timer->data = data;
	if (flags == SP_TIMER_EXACT) {
		priv->etqueue = g_slist_insert_sorted (priv->etqueue, timer, sp_doc_timer_compare);
	} else {
		priv->itqueue = g_slist_prepend (priv->itqueue, timer);
	}
	return id;
}

/* Remove timer */
void
sp_document_remove_timer (SPDocument *doc, unsigned int id)
{
	SPDocumentPrivate *priv;
	SPDocTimer *timer;
	priv = doc->priv;
	timer = &priv->timers[id];
	if (timer->awake) {
		if (timer->flags == SP_TIMER_EXACT) {
			priv->etqueue = g_slist_remove (priv->etqueue, timer);
		} else {
			priv->itqueue = g_slist_remove (priv->itqueue, timer);
		}
	}
	timer->callback = NULL;
	timer->data = NULL;
}

/* Set up timer */
void
sp_document_set_timer (SPDocument *doc, unsigned int id, double time, unsigned int flags)
{
	SPDocumentPrivate *priv;
	SPDocTimer *timer;
	priv = doc->priv;
	timer = &priv->timers[id];
	/* Remove from queue */
	if (timer->awake) {
		if (timer->flags == SP_TIMER_EXACT) {
			priv->etqueue = g_slist_remove (priv->etqueue, timer);
		} else {
			priv->itqueue = g_slist_remove (priv->itqueue, timer);
		}
	}
	timer->time = time;
	timer->flags = flags;
	timer->awake = 1;
	if (flags == SP_TIMER_EXACT) {
		priv->etqueue = g_slist_insert_sorted (priv->etqueue, timer, sp_doc_timer_compare);
	} else {
		priv->itqueue = g_slist_prepend (priv->itqueue, timer);
	}
}

static void
sp_document_process_updates (SPDocument *doc)
{
	if (doc->root->uflags || doc->root->mflags) {
		SPItemCtx ctx;
		ctx.ctx.flags = 0;
		nr_matrix_d_set_identity (&ctx.i2doc);
		/* Set up viewport in case svg has it defined as percentages */
		ctx.vp.x0 = 0.0;
		ctx.vp.y0 = 0.0;
		ctx.vp.x1 = 21.0 / 2.54 * 72.0 * 1.25;
		ctx.vp.y1 = 29.7 / 2.54 * 72.0 * 1.25;
		nr_matrix_d_set_identity (&ctx.i2vp);
		sp_object_invoke_update (doc->root, (SPCtx *) &ctx, 0);
		/* if (doc->root->uflags & SP_OBJECT_MODIFIED_FLAG) return TRUE; */

		/* Emit "modified" signal on objects */
		sp_object_invoke_modified (doc->root, 0);

		/* Emit our own "modified" signal */
		g_signal_emit (G_OBJECT (doc), signals [MODIFIED], 0,
			       SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_CHILD_MODIFIED_FLAG | SP_OBJECT_PARENT_MODIFIED_FLAG);
	}
}

#define deltat 0.0

static void
sp_document_process_timers (SPDocument *doc)
{
	static int tsize = 0;
	static int tlen = 0;
	static SPDocTimer *timers = NULL;
	SPDocumentPrivate *priv;
	double ntime;
	GSList *l;
	int i;

	priv = doc->priv;

	ntime = sp_document_get_time (doc) + deltat;

	tlen = 0;
	while (priv->etqueue && (((SPDocTimer *) priv->etqueue->data)->time <= ntime)) {
		if (tlen >= tsize) {
			tsize = MAX (4, (tsize << 1));
			timers = (SPDocTimer *) realloc (timers, tsize * sizeof (SPDocTimer));
		}
		timers[tlen++] = *((SPDocTimer *) priv->etqueue->data);
		((SPDocTimer *) priv->etqueue->data)->awake = FALSE;
		priv->etqueue = g_slist_remove (priv->etqueue, priv->etqueue->data);
	}
	for (l = priv->itqueue; l != NULL; l = l->next) {
		if (tlen >= tsize) {
			tsize = MAX (4, (tsize << 1));
			timers = (SPDocTimer *) realloc (timers, tsize * sizeof (SPDocTimer));
		}
		timers[tlen++] = *((SPDocTimer *) l->data);
	}
	for (i = 0; i < tlen; i++) {
		timers[i].callback (ntime, timers[i].data);
	}
}

#if 0
/* Schedule either idle or timeout */
static void sp_document_schedule_timers (SPDocument *doc);

static void
sp_document_schedule_timers (SPDocument *doc)
{
	SPDocumentPrivate *priv;
	double ctime, dtime;
	unsigned int idle;
	priv = doc->priv;
	idle = FALSE;
	dtime = 1e18;
	if (doc->root && (doc->root->uflags || doc->root->mflags)) idle = TRUE;
	if (priv->itqueue) idle = TRUE;
	if (!idle && priv->etqueue) {
		SPDocTimer *timer;
		timer = (SPDocTimer *) priv->itqueue->data;
		/* Get current time */
		ctime = sp_document_get_time (doc);
		dtime = timer->time - ctime;
		if (dtime <= deltat) idle = TRUE;
	}
	/* We have to remove old timer always */
	if (priv->timeout) {
		/* Remove timer */
		gtk_timeout_remove (priv->timeout);
		priv->timeout = 0;
	}
	if (idle) {
		if (!priv->idle) {
			/* Register idle */
			priv->idle = gtk_idle_add_priority (SP_DOCUMENT_UPDATE_PRIORITY, sp_document_idle_handler, doc);
		}
	} else if (dtime < 1000 * 365.25 * 86400) {
		if (priv->idle) {
			/* Remove idle handler */
			gtk_idle_remove (priv->idle);
			priv->idle = 0;
		}
		/* Register timeout */
		priv->timeout = gtk_timeout_add ((int) dtime * 1000, sp_document_timeout_handler, doc);
	} else {
		if (priv->idle) {
			/* Remove idle handler */
			gtk_idle_remove (priv->idle);
			priv->idle = 0;
		}
	}
}
#endif


