#ifndef __SP_URL_H__
#define __SP_URL_H__

/*
 * URL Handling
 *
 * Copyright Lauris Kaplinski 2010
 */

#include <libarikkei/arikkei-object.h>

typedef struct _SPURLHandler SPURLHandler;
typedef struct _SPURLHandlerClass SPURLHandlerClass;

#define SP_TYPE_URL_HANDLER (sp_url_handler_get_type ())
#define SP_URL_HANDLER(o) (ARIKKEI_CHECK_INSTANCE_CAST ((o), SP_TYPE_URL_HANDLER, SPURLHandler))
#define SP_IS_URL_HANDLER(o) (ARIKKEI_CHECK_INSTANCE_TYPE ((o), SP_TYPE_URL_HANDLER))

typedef struct _SPFileHandler SPFileHandler;
typedef struct _SPFileHandlerClass SPFileHandlerClass;

#define SP_TYPE_FILE_HANDLER (sp_file_handler_get_type ())
#define SP_FILE_HANDLER(o) (ARIKKEI_CHECK_INSTANCE_CAST ((o), SP_TYPE_FILE_HANDLER, SPFileHandler))
#define SP_IS_FILE_HANDLER(o) (ARIKKEI_CHECK_INSTANCE_TYPE ((o), SP_TYPE_FILE_HANDLER))

#ifdef __cplusplus
extern "C" {
#endif

struct _SPURLHandler {
	ArikkeiObject object;
};

struct _SPURLHandlerClass {
	ArikkeiObjectClass parent_class;
	const unsigned char * (*mmap) (SPURLHandler *handler, const unsigned char *address, size_t *size);
	void (*munmap) (SPURLHandler *handler, const unsigned char *cdata, size_t csize);
};

unsigned int sp_url_handler_get_type (void);

const unsigned char *sp_url_mmap (SPURLHandler *handler, const unsigned char *address, size_t *size);
void sp_url_munmap (SPURLHandler *handler, const unsigned char *cdata, size_t csize);

void sp_url_handler_release (SPURLHandler *handler);

/* Get reference handler that is capable fetching given URL or NULL */
SPURLHandler *sp_url_handler_get (const unsigned char *url);

/* Create canonic URL - i.e. prepend file: if needed and replace backslashes */
unsigned char *sp_build_canonic_url (const unsigned char *address);

/* SPFileHandler */

unsigned int sp_file_handler_get_type (void);

/* Create new file handler with given base /*
/* Protocol has to be file: or NULL */
/* Filename, ref and argument parts are ignored */
SPFileHandler *sp_file_handler_new (const unsigned char *base);

#ifdef __cplusplus
}
#endif

#endif
