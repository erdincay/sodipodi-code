#ifndef __SP_PRINT_PDF_H__
#define __SP_PRINT_PDF_H__

/*
 * PDF printing
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *   Mitsuru Oka <oka326@parkcity.ne.jp>
 *
 * This code is in public domain
 */

typedef struct _SPPrintContextPDF SPPrintContextPDF;
typedef struct _SPPrintContextPDFClass SPPrintContextPDFClass;

#define SP_TYPE_PRINT_CONTEXT_PDF (sp_print_context_pdf_get_type ())
#define SP_PRINT_CONTEXT_PDF(o) (ARIKKEI_CHECK_INSTANCE_CAST ((o), SP_TYPE_PRINT_CONTEXT_PDF, SPPrintContextPDF))
#define SP_IS_PRINT_CONTEXT_PDF(o) (ARIKKEI_CHECK_INSTANCE_TYPE ((o), SP_TYPE_PRINT_CONTEXT_PDF))

#include "print.h"

typedef struct _SPPDFObject SPPDFObject;
typedef struct _SPPDFBuffer SPPDFBuffer;

struct _SPPrintContextPDF {
	SPPrintContext ctx;

	unsigned int type;
	/* Document dimensions */
	float width;
	float height;
	/* PDF internal state */
	SPPDFBuffer *stream;
	SPPDFObject *objects;
	int n_objects;
	int offset;
	float stroke_rgb[3];
	float fill_rgb[3];
	unsigned int color_set_stroke : 1;
	unsigned int color_set_fill : 1;
	/* Objects Ids */
	/* single page support */
	int page_id;
	int contents_id;
	int resources_id;
};

struct _SPPrintContextPDFClass {
	SPPrintContextClass parent_class;
};

unsigned int sp_print_context_pdf_get_type (void);

SPPrintContext *sp_print_context_pdf_new (void);

#if 0
typedef enum _SPPlainPDFType SPPlainPDFType;

enum _SPPlainPDFType {
	SP_PLAIN_PDF_DEFAULT,
	SP_PLAIN_PDF_BITMAP
};

SPPrintPlainDriver *sp_plain_pdf_driver_new (SPPlainPDFType type, SPModulePrintPlain *module);
#endif

#endif
