#define __SP_URL_C__

/*
 * URL Handling
 *
 * Copyright Lauris Kaplinski 2010
 */

#include <malloc.h>
#include <string.h>
#include <stdlib.h>

#include <libarikkei/arikkei-url.h>
#include <libarikkei/arikkei-dict.h>
#include <libarikkei/arikkei-iolib.h>

#include "url.h"

static void sp_url_handler_class_init (SPURLHandlerClass *klass);
static void sp_url_handler_init (SPURLHandler *object);
static void sp_url_handler_finalize (ArikkeiObject *object);

static ArikkeiObjectClass *url_handler_parent_class;

unsigned int
sp_url_handler_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		type = arikkei_object_register_type (ARIKKEI_TYPE_OBJECT,
						(const unsigned char *) "SPURLHandler",
						sizeof (SPURLHandlerClass),
						sizeof (SPURLHandler),
						(void (*) (ArikkeiObjectClass *)) sp_url_handler_class_init,
						(void (*) (ArikkeiObject *)) sp_url_handler_init);
	}
	return type;
}

static void
sp_url_handler_class_init (SPURLHandlerClass *klass)
{
	ArikkeiObjectClass *object_class;

	object_class = (ArikkeiObjectClass *) klass;

	url_handler_parent_class = ((ArikkeiObjectClass *) klass)->parent;

	object_class->finalize = sp_url_handler_finalize;
}

static void
sp_url_handler_init (SPURLHandler *handler)
{
}

static void
sp_url_handler_finalize (ArikkeiObject *object)
{
	((ArikkeiObjectClass *) (url_handler_parent_class))->finalize (object);
}

void
sp_url_handler_release (SPURLHandler *handler)
{
	arikkei_object_unref (ARIKKEI_OBJECT(handler));
}

SPURLHandler *
sp_url_handler_get (const unsigned char *url)
{
	unsigned char *p;
	ArikkeiURL urld;

	if (!url || !*url) return NULL;

	if (arikkei_url_setup (&urld, url, NULL)) {
		if (urld.protocol && !strcmp (urld.protocol, "file")) {
			SPURLHandler *handler;
			/* fixme: Keep dictionary of created handlers (Lauris) */
			/* fixme: Set base to curent directory (Lauris) */
			handler = (SPURLHandler *) arikkei_object_new (SP_TYPE_FILE_HANDLER);
			return handler;
		}
	}
	arikkei_url_release (&urld);

	return NULL;
}

const unsigned char *
sp_url_mmap (SPURLHandler *handler, const unsigned char *address, size_t *size)
{
	arikkei_return_val_if_fail (handler != NULL, NULL);
	arikkei_return_val_if_fail (address && *address, NULL);

	return ((SPURLHandlerClass *) ((ArikkeiObject *) handler)->klass)->mmap (handler, address, size);
}

void
sp_url_munmap (SPURLHandler *handler, const unsigned char *cdata, size_t csize)
{
	arikkei_return_if_fail (handler != NULL);
	arikkei_return_if_fail (cdata != NULL);

	((SPURLHandlerClass *) ((ArikkeiObject *) handler)->klass)->munmap (handler, cdata, csize);
}

unsigned char *
sp_build_canonic_url (const unsigned char *address)
{
	unsigned char *url, *p;

	if (!address || !*address) return NULL;

	url = NULL;

#ifdef WIN32
	if (address[1] == ':') {
		/* We are absolute path C:PATH*/
		url = arikkei_build_file_url (address);
	}
#endif
	if (!strchr (address, ':')) {
		/* We are file URL */
		url = arikkei_build_file_url (address);
	}
	if (!url) url = strdup ((const char *) address);
	for (p = url; *p; p++) {
		if (*p == '\\') *p = '/';
	}

	return url;
}

/* SPFileHandler */

struct FileMap {
	unsigned int refcount;
	unsigned char *path;
	const unsigned char *cdata;
	size_t csize;
};

struct _SPFileHandler {
	SPURLHandler handler;

	ArikkeiURL base;

	ArikkeiDict path2map;
	ArikkeiDict cdata2map;
};

struct _SPFileHandlerClass {
	SPURLHandlerClass parent_class;
};

static void sp_file_handler_class_init (SPFileHandlerClass *klass);
static void sp_file_handler_init (SPFileHandler *object);
static void sp_file_handler_finalize (ArikkeiObject *object);

static const unsigned char *sp_file_handler_mmap (SPURLHandler *handler, const unsigned char *address, size_t *size);
static void sp_file_handler_munmap (SPURLHandler *handler, const unsigned char *cdata, size_t csize);

static SPURLHandlerClass *file_handler_parent_class;

unsigned int
sp_file_handler_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		type = arikkei_object_register_type (SP_TYPE_URL_HANDLER,
						(const unsigned char *) "SPFileHandler",
						sizeof (SPFileHandlerClass),
						sizeof (SPFileHandler),
						(void (*) (ArikkeiObjectClass *)) sp_file_handler_class_init,
						(void (*) (ArikkeiObject *)) sp_file_handler_init);
	}
	return type;
}

static void
sp_file_handler_class_init (SPFileHandlerClass *klass)
{
	ArikkeiObjectClass *object_class;
	SPURLHandlerClass *handler_class;

	object_class = (ArikkeiObjectClass *) klass;
	handler_class = (SPURLHandlerClass *) klass;

	file_handler_parent_class = (SPURLHandlerClass *) ((ArikkeiObjectClass *) klass)->parent;

	object_class->finalize = sp_file_handler_finalize;

	handler_class->mmap = sp_file_handler_mmap;
	handler_class->munmap = sp_file_handler_munmap;
}

static void
sp_file_handler_init (SPFileHandler *handler)
{
	arikkei_dict_setup_string (&handler->path2map, 87);
	arikkei_dict_setup_pointer (&handler->cdata2map, 87);
}

unsigned int
sp_file_handler_delete_map (const void *key, const void *value, void *data)
{
	struct FileMap *map;
	map = (struct FileMap *) value;
	arikkei_munmap (map->cdata, map->csize);
	free (map->path);
	free (map);
	return 1;
}

static void
sp_file_handler_finalize (ArikkeiObject *object)
{
	SPFileHandler *handler;

	handler = SP_FILE_HANDLER(object);

	arikkei_dict_forall  (&handler->path2map, sp_file_handler_delete_map, handler);
	arikkei_dict_release (&handler->path2map);
	arikkei_dict_release (&handler->cdata2map);
	arikkei_url_release (&handler->base);

	((ArikkeiObjectClass *) (file_handler_parent_class))->finalize (object);
}

static const unsigned char *
sp_file_handler_mmap (SPURLHandler *handler, const unsigned char *address, size_t *size)
{
	SPFileHandler *filehandler;
	ArikkeiURL url;
	const unsigned char *cdata;
	size_t csize, dlen, flen;
	unsigned char *path;
	struct FileMap *map;

	arikkei_return_val_if_fail (SP_IS_FILE_HANDLER(handler), NULL);
	arikkei_return_val_if_fail (address && *address, NULL);

	filehandler = SP_FILE_HANDLER (handler);

	arikkei_url_setup (&url, address, "file");
	if (strcmp (url.protocol, "file") || !url.filename || !*url.filename) {
		arikkei_url_release (&url);
		return NULL;
	}

	dlen = (url.directory) ? strlen (url.directory) : 0;
	flen = strlen (url.filename);
	path = (unsigned char *) malloc (dlen + flen + 1);
	if (url.directory) strcpy (path, url.directory);
	strcpy (path + dlen, url.filename);
	path[dlen + flen] = 0;

	arikkei_url_release (&url);

	map = (struct FileMap *) arikkei_dict_lookup (&filehandler->path2map, path);
	if (map) {
		free (path);
		map->refcount += 1;
		return map->cdata;
	}

	cdata = arikkei_mmap (path, &csize, NULL);
	if (!cdata) {
		free (path);
		return NULL;
	}

	map = (struct FileMap *) malloc (sizeof (struct FileMap));
	map->refcount = 1;
	map->path = path;
	map->cdata = cdata;
	map->csize = csize;
	arikkei_dict_insert (&filehandler->path2map, map->path, map);
	arikkei_dict_insert (&filehandler->cdata2map, map->cdata, map);

	return cdata;
}

static void
sp_file_handler_munmap (SPURLHandler *handler, const unsigned char *cdata, size_t csize)
{
	SPFileHandler *filehandler;
	struct FileMap *map;

	arikkei_return_if_fail (SP_IS_FILE_HANDLER(handler));
	arikkei_return_if_fail (cdata);

	filehandler = SP_FILE_HANDLER (handler);

	map = (struct FileMap *) arikkei_dict_lookup (&filehandler->cdata2map, cdata);
	if (!map) return;
	map->refcount -= 1;
	if (map->refcount > 0) return;

	arikkei_dict_remove (&filehandler->path2map, map->path);
	arikkei_dict_remove (&filehandler->cdata2map, map->cdata);
	arikkei_munmap (map->cdata, map->csize);
	free (map->path);
	free (map);
}

SPFileHandler *
sp_file_handler_new (const unsigned char *base)
{
	SPFileHandler *handler;
	ArikkeiURL url;
	arikkei_url_setup (&url, base, "file");
	if (strcmp (url.protocol, "file")) {
		arikkei_url_release (&url);
		return NULL;
	}
	handler = (SPFileHandler *) arikkei_object_new (SP_TYPE_FILE_HANDLER);
	handler->base = url;
	return handler;
}
