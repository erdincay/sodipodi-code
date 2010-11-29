#define __SP_PRINT_PDF_C__

/*
 * PDF printing
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *   Mitsuru Oka <oka326@parkcity.ne.jp>
 *
 * Basic printing code, EXCEPT image and
 * ascii85 filter is in public domain
 *
 * Image printing and Ascii85 filter:
 *
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 * Copyright (C) 1997-98 Peter Kirchgessner
 * George White <aa056@chebucto.ns.ca>
 * Austin Donnelly <austin@gimp.org>
 *
 * Licensed under GNU GPL
 */

#include <string.h>
#include <stdio.h>

#include <zlib.h>

#include <libarikkei/arikkei-strlib.h>

#include "document.h"

#include "print-pdf.h"

/* #define PDF_EOL "\r\n" */

#define PDF_EOL "\r\n"

#define PDF_BUFFER_INITIAL_SIZE 1024

enum {
	PDFO_DICT_NONE = 0,
	PDFO_DICT_START,
	PDFO_DICT_END
};

enum {
	PDFO_STREAM_NONE = 0,
	PDFO_STREAM_START,
	PDFO_STREAM_END
};

struct _SPPDFObject {
	int id;
	int offset;
	unsigned int state_dict : 2;
	unsigned int state_stream : 2;
	unsigned int external : 1;
	gchar *name;
	SPPDFObject *next;
};

struct _SPPDFBuffer {
	unsigned char *buffer;
	int length;
	int alloc_size;
};

struct _SPPDFImage {
	unsigned int id;
	char *name;
};

static void sp_print_context_pdf_print_path (SPPrintContextPDF *pdf, const NRPath *path);
static unsigned int sp_pdf_print_image (SPPrintContextPDF *pdf, unsigned char *px, unsigned int width, unsigned int height, unsigned int rs, const NRMatrixF *transform);

static int sp_print_context_pdf_fprintf (SPPrintContextPDF *pdf, const char *format, ...);
static int sp_print_context_pdf_fwrite (SPPrintContextPDF *pdf, const char *buffer, int len);
static int sp_print_context_pdf_fprint_double (SPPrintContextPDF *pdf, double value);

static int sp_print_context_pdf_stream_fprintf (SPPrintContextPDF *pdf, const char *format, ...);
static int sp_print_context_pdf_stream_fprint_double (SPPrintContextPDF *pdf, double value);

static int sp_print_context_pdf_write_contents (SPPrintContextPDF *pdf);
static int sp_print_context_pdf_write_resources (SPPrintContextPDF *pdf);
static int sp_print_context_pdf_write_page (SPPrintContextPDF *pdf);
static int sp_print_context_pdf_write_pages (SPPrintContextPDF *pdf, int catalog_id);

static int sp_print_context_pdf_object_new (SPPrintContextPDF *pdf);
static void sp_print_context_pdf_object_start (SPPrintContextPDF *pdf, int object_id);
static void sp_print_context_pdf_object_start_stream (SPPrintContextPDF *pdf, int object_id);
static void sp_print_context_pdf_object_end (SPPrintContextPDF *pdf, int object_id);
SPPDFObject *sp_print_context_pdf_objects_reverse (SPPDFObject *objects);

static SPPDFBuffer *sp_pdf_buffer_new (void);
static SPPDFBuffer *sp_pdf_buffer_delete (SPPDFBuffer *buffer);
static int sp_pdf_buffer_write (SPPDFBuffer *buffer, unsigned char *text, int length);



static void sp_print_context_pdf_class_init (SPPrintContextPDFClass *pdf_class);
static void sp_print_context_pdf_init (SPPrintContextPDF *ctx);
static void sp_print_context_pdf_finalize (ArikkeiObject *object);
static unsigned int sp_print_context_pdf_begin (SPPrintContext *ctx, SPDocument *doc);
static unsigned int sp_print_context_pdf_finish (SPPrintContext *ctx);
static unsigned int sp_print_context_pdf_bind (SPPrintContext *ctx, const NRMatrixF *transform, float opacity);
static unsigned int sp_print_context_pdf_release (SPPrintContext *ctx);
static unsigned int sp_print_context_pdf_fill (SPPrintContext *ctx, const NRPath *path, const NRMatrixF *ctm, const SPStyle *style, const NRRectF *pbox, const NRRectF *dbox, const NRRectF *bbox);
static unsigned int sp_print_context_pdf_stroke (SPPrintContext *ctx, const NRPath *path, const NRMatrixF *ctm, const SPStyle *style, const NRRectF *pbox, const NRRectF *dbox, const NRRectF *bbox);
static unsigned int sp_print_context_pdf_image (SPPrintContext *ctx, unsigned char *px, unsigned int w, unsigned int h, unsigned int rs, const NRMatrixF *transform, const SPStyle *style);

static SPPrintContextClass *parent_class;

unsigned int
sp_print_context_pdf_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		type = arikkei_object_register_type (SP_TYPE_PRINT_CONTEXT,
						"SPPrintContextPDF",
						sizeof (SPPrintContextPDFClass),
						sizeof (SPPrintContextPDF),
						(void (*) (ArikkeiObjectClass *)) sp_print_context_pdf_class_init,
						(void (*) (ArikkeiObject *)) sp_print_context_pdf_init);
	}
	return type;
}

static void
sp_print_context_pdf_class_init (SPPrintContextPDFClass *pdf_class)
{
	ArikkeiObjectClass *object_class;
	SPPrintContextClass *ctx_class;

	object_class = (ArikkeiObjectClass *) pdf_class;
	ctx_class = (SPPrintContextClass *) pdf_class;

	parent_class = (SPPrintContextClass *) object_class->parent;

	object_class->finalize = sp_print_context_pdf_finalize;

	ctx_class->begin = sp_print_context_pdf_begin;
	ctx_class->finish = sp_print_context_pdf_finish;
	ctx_class->bind = sp_print_context_pdf_bind;
	ctx_class->release = sp_print_context_pdf_release;
	ctx_class->fill = sp_print_context_pdf_fill;
	ctx_class->stroke = sp_print_context_pdf_stroke;
	ctx_class->image = sp_print_context_pdf_image;
}

static void
sp_print_context_pdf_init (SPPrintContextPDF *pdf)
{
	/* fixme: */
	pdf->type = 0;

	pdf->stream = sp_pdf_buffer_new ();
	pdf->objects = NULL;
	pdf->n_objects = 0;

	pdf->offset = 0;
	pdf->color_set_stroke = FALSE;
	pdf->color_set_fill = FALSE;

	pdf->contents_id = 0;
	pdf->resources_id = 0;
}

static void
sp_print_context_pdf_finalize (ArikkeiObject *object)
{
	SPPrintContextPDF *pdf;
	SPPDFObject *pdfobj;

	pdf = SP_PRINT_CONTEXT_PDF(object);

	sp_pdf_buffer_delete (pdf->stream);

	pdfobj = pdf->objects;
	while (pdfobj) {
		SPPDFObject *next;
		next = pdfobj->next;
		g_free (pdfobj->name);
		free (pdfobj);
		pdfobj = next;
	}

	((ArikkeiObjectClass *) (parent_class))->finalize (object);
}

static unsigned int
sp_print_context_pdf_begin (SPPrintContext *ctx, SPDocument *doc)
{
	SPPrintContextPDF *pdf;
	int res;

	pdf = SP_PRINT_CONTEXT_PDF(ctx);

	res = sp_print_context_pdf_fprintf (pdf, "%%PDF-1.3" PDF_EOL);
	if (res >= 0) res = sp_print_context_pdf_fprintf (pdf, "%%\xb5\xed\xae\xfe" PDF_EOL);
#if 0
	/* flush this to test output stream as early as possible */
	if (fflush (pmod->stream)) {
	/* g_print("caught error in sp_print_context_pdf_begin\n");*/
		if (ferror (pmod->stream)) {
			g_print("Error %d on output stream: %s\n", errno,
				g_strerror(errno));
		}
		g_warning ("Printing failed\n");
		/* fixme: should use pclose() for pipes */
		fclose (pmod->stream);
		pmod->stream = NULL;
		fflush (stdout);
		return 0;
	}
#endif

	pdf->width = sp_document_get_width (doc);
	pdf->height = sp_document_get_height (doc);

	sp_print_context_pdf_stream_fprintf (pdf, "0.8 0 0 -0.8 0.0 ");
	sp_print_context_pdf_stream_fprint_double (pdf, pdf->height);
	res = sp_print_context_pdf_stream_fprintf (pdf, " cm" PDF_EOL);

	pdf->page_id = sp_print_context_pdf_object_new (pdf);
	pdf->contents_id = sp_print_context_pdf_object_new (pdf);
	pdf->resources_id = sp_print_context_pdf_object_new (pdf);

	return res;
}

static unsigned int
sp_print_context_pdf_finish (SPPrintContext *ctx)
{
	SPPrintContextPDF *pdf;
	SPPDFObject *object;
	unsigned int written;
	int xref_offset, n_objects;
	int catalog_id;

	pdf = SP_PRINT_CONTEXT_PDF(ctx);

	/* Write single page contents */
	written = sp_print_context_pdf_write_page (pdf);

	/* Write pages */
	catalog_id = sp_print_context_pdf_object_new (pdf);
	written += sp_print_context_pdf_write_pages (pdf, catalog_id);

	/* Write info block */

	/* Write xref */
	xref_offset = pdf->offset;
	n_objects = pdf->n_objects + 1;
	written += sp_print_context_pdf_fprintf (pdf, "xref" PDF_EOL
						  "0 %d" PDF_EOL
						  "%010d %05d f" PDF_EOL,
						  n_objects,
						  0, 65535);

	pdf->objects = sp_print_context_pdf_objects_reverse (pdf->objects);

	for (object = pdf->objects; object; object = object->next) {
		if (object->offset < 1) g_warning ("Object with zero offset!");
		
		written += sp_print_context_pdf_fprintf (pdf, "%010d %05d n" PDF_EOL, object->offset, 0);
	}

	/* Write trailer */
	written += sp_print_context_pdf_fprintf (pdf, "trailer" PDF_EOL
						  "<<" PDF_EOL
						  "/Size %d" PDF_EOL
						  "/Root %d 0 R" PDF_EOL
						  ">>" PDF_EOL
						  "startxref" PDF_EOL
						  "%d" PDF_EOL
						  "%%%%EOF" PDF_EOL,
						  n_objects, catalog_id,
						  xref_offset);
						  
#if 0
	/* Flush stream to be sure */
	(void) fflush (pmod->stream);

	/* fixme: should really use pclose for popen'd streams */
	fclose (pmod->stream);
	pmod->stream = 0;
#endif

	return written;
}

static unsigned int
sp_print_context_pdf_bind (SPPrintContext *ctx, const NRMatrixF *transform, float opacity)
{
	SPPrintContextPDF *pdf;
	unsigned int i;
	unsigned int res;

	pdf = SP_PRINT_CONTEXT_PDF(ctx);

	sp_print_context_pdf_stream_fprintf (pdf, "q" PDF_EOL);

	for (i = 0; i < 6; i++) {
		sp_print_context_pdf_stream_fprint_double (pdf, transform->c[i]);
		sp_print_context_pdf_stream_fprintf (pdf, " ");
	}
	res = sp_print_context_pdf_stream_fprintf (pdf, "cm" PDF_EOL);

	return res;
}

static unsigned int
sp_print_context_pdf_release (SPPrintContext *ctx)
{
	SPPrintContextPDF *pdf;
	int res;

	pdf = SP_PRINT_CONTEXT_PDF(ctx);

	printf ("pdf_release\n");

	pdf->color_set_fill = FALSE;
	pdf->color_set_stroke = FALSE;

	res = sp_print_context_pdf_stream_fprintf (pdf, "Q" PDF_EOL);

	return res;
}

static unsigned int
sp_print_context_pdf_fill (SPPrintContext *ctx, const NRPath *path, const NRMatrixF *ctm, const SPStyle *style,
			    const NRRectF *pbox, const NRRectF *dbox, const NRRectF *bbox)
{
	SPPrintContextPDF *pdf;

	pdf = SP_PRINT_CONTEXT_PDF(ctx);

	if (style->fill.type == SP_PAINT_TYPE_COLOR) {
		float rgb[3];

		sp_color_get_rgb_floatv (&style->fill.value.color, rgb);

		/* Set fill color */
		if (!(pdf->color_set_fill && (pdf->fill_rgb[0] == rgb[0]) && (pdf->fill_rgb[1] == rgb[1]) && (pdf->fill_rgb[2] == rgb[2]))) {
			unsigned char c[3][32];
			int i;
			for (i = 0; i < 3; i++) {
				arikkei_dtoa_simple (c[i], 32, rgb[i], 6, 3, 0);
			}
			sp_print_context_pdf_stream_fprintf (pdf, "%s %s %s rg" PDF_EOL, c[0], c[1], c[2]);
			pdf->fill_rgb[0] = rgb[0];
			pdf->fill_rgb[1] = rgb[1];
			pdf->fill_rgb[2] = rgb[2];
			pdf->color_set_fill = TRUE;
		}
		sp_print_context_pdf_print_path (pdf, path);

		switch (style->fill_rule.value) {
		case NR_WIND_RULE_NONZERO:
			sp_print_context_pdf_stream_fprintf (pdf, "f" PDF_EOL);
			break;
		case NR_WIND_RULE_EVENODD:
		default:
			sp_print_context_pdf_stream_fprintf (pdf, "f*" PDF_EOL);
			break;
		}
	}

	return 0;
}

static unsigned int
sp_print_context_pdf_stroke (SPPrintContext *ctx, const NRPath *path, const NRMatrixF *ctm, const SPStyle *style,
			      const NRRectF *pbox, const NRRectF *dbox, const NRRectF *bbox)
{
	SPPrintContextPDF *pdf;

	pdf = SP_PRINT_CONTEXT_PDF(ctx);

	if (style->stroke.type == SP_PAINT_TYPE_COLOR) {
		float rgb[3];

		sp_color_get_rgb_floatv (&style->stroke.value.color, rgb);

		/* Set stroke color */
		if (! (pdf->color_set_stroke && (pdf->stroke_rgb[0] != rgb[0]) && (pdf->stroke_rgb[1] != rgb[1]) && (pdf->stroke_rgb[2] != rgb[2]))) {
			unsigned char c[3][32];
			int i;
			for (i = 0; i < 3; i++) {
				arikkei_dtoa_simple (c[i], 32, rgb[i], 6, 3, 0);
			}
			sp_print_context_pdf_stream_fprintf (pdf, "%s %s %s RG" PDF_EOL, c[0], c[1], c[2]);
			pdf->stroke_rgb[0] = rgb[0];
			pdf->stroke_rgb[1] = rgb[1];
			pdf->stroke_rgb[2] = rgb[2];
			pdf->color_set_stroke = TRUE;
		}
		
		/* Set dash */
		if (style->stroke_dash) {
			unsigned char c[32];
			unsigned int i;
			sp_print_context_pdf_stream_fprintf (pdf, "[");
			for (i = 0; i < style->stroke_dash->ndashes; i++) {
				sp_print_context_pdf_stream_fprintf (pdf, (i) ? " " : "");
				sp_print_context_pdf_stream_fprint_double (pdf, style->stroke_dash->dashes[i]);
			}
			arikkei_dtoa_simple (c, 32, style->stroke_dash->offset, 6, 3, 0);
			sp_print_context_pdf_stream_fprintf (pdf, "] %s d" PDF_EOL, c);
		} else {
			sp_print_context_pdf_stream_fprintf (pdf, "[] 0 d" PDF_EOL);
		}

		/* Set line */
		sp_print_context_pdf_stream_fprint_double (pdf, style->stroke_width.computed);
		sp_print_context_pdf_stream_fprintf (pdf, " w %d J %d j ",
							  style->stroke_linecap.computed,
							  style->stroke_linejoin.computed);
		sp_print_context_pdf_stream_fprint_double (pdf, style->stroke_miterlimit.value);
		sp_print_context_pdf_stream_fprintf (pdf, " M" PDF_EOL);

		/* Path */
		sp_print_context_pdf_print_path (pdf, path);

		/* Stroke */
		sp_print_context_pdf_stream_fprintf (pdf, "S" PDF_EOL);
	}

	return 0;
}

static unsigned int
sp_print_context_pdf_image (SPPrintContext *ctx, unsigned char *px, unsigned int w, unsigned int h, unsigned int rs,
			     const NRMatrixF *transform, const SPStyle *style)
{
	SPPrintContextPDF *pdf;

	pdf = SP_PRINT_CONTEXT_PDF(ctx);

	return sp_pdf_print_image (pdf, px, w, h, rs, transform);
}

/* PDF helpers */

static unsigned int
sp_print_path_moveto (float x0, float y0, unsigned int flags, void *data)
{
	SPPrintContextPDF *pdf;
	unsigned char c0[32], c1[32];
	pdf = SP_PRINT_CONTEXT_PDF(data);
	arikkei_dtoa_simple (c0, 32, x0, 6, 3, 0);
	arikkei_dtoa_simple (c1, 32, y0, 6, 3, 0);
	sp_print_context_pdf_stream_fprintf (pdf, "%s %s m\n", c0, c1);
	return TRUE;
}

static unsigned int
sp_print_path_lineto (float x0, float y0, float x1, float y1, unsigned int flags, void *data)
{
	SPPrintContextPDF *pdf;
	unsigned char c0[32], c1[32];
	pdf = SP_PRINT_CONTEXT_PDF(data);
	arikkei_dtoa_simple (c0, 32, x1, 6, 3, 0);
	arikkei_dtoa_simple (c1, 32, y1, 6, 3, 0);
	sp_print_context_pdf_stream_fprintf (pdf, "%s %s l\n", c0, c1);
	if ((flags & NR_PATH_CLOSED) && (flags & NR_PATH_LAST)) {
		sp_print_context_pdf_stream_fprintf (pdf, "h\n");
	}
	return TRUE;
}

static unsigned int
sp_print_path_curveto3 (float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3,
			unsigned int flags, void *data)
{
	SPPrintContextPDF *pdf;
	unsigned char c0[32], c1[32], c2[32], c3[32], c4[32], c5[32];
	pdf = SP_PRINT_CONTEXT_PDF(data);
	arikkei_dtoa_simple (c0, 32, x1, 6, 3, 0);
	arikkei_dtoa_simple (c1, 32, y1, 6, 3, 0);
	arikkei_dtoa_simple (c2, 32, x2, 6, 3, 0);
	arikkei_dtoa_simple (c3, 32, y2, 6, 3, 0);
	arikkei_dtoa_simple (c4, 32, x3, 6, 3, 0);
	arikkei_dtoa_simple (c5, 32, y3, 6, 3, 0);
	sp_print_context_pdf_stream_fprintf (pdf, "%s %s %s %s %s %s c\n", c0, c1, c2, c3, c4, c5);
	return TRUE;
}

static NRPathGVector pdfpgv = {
	sp_print_path_moveto,
	sp_print_path_lineto,
	NULL,
	sp_print_path_curveto3,
	NULL
};

static void
sp_print_context_pdf_print_path (SPPrintContextPDF *pdf, const NRPath *path)
{
#if 1
	nr_path_forall (path, NULL, &pdfpgv, pdf);
#else
	unsigned int started, closed;

	started = FALSE;
	closed = FALSE;
	while (bp->code != ART_END) {
		switch (bp->code) {
		case ART_MOVETO:
			if (started && closed) {
				sp_print_context_pdf_stream_fprintf (pdf, "h" PDF_EOL);
			}
			started = TRUE;
			closed = TRUE;
			sp_print_context_pdf_stream_fprintf (pdf, "%g %g m" PDF_EOL, bp->x3, bp->y3);
			break;
		case ART_MOVETO_OPEN:
			if (started && closed) {
				sp_print_context_pdf_stream_fprintf (pdf, "h" PDF_EOL);
			}
			started = FALSE;
			closed = FALSE;
			sp_print_context_pdf_stream_fprintf (pdf, "%g %g m" PDF_EOL, bp->x3, bp->y3);
			break;
		case ART_LINETO:
			sp_print_context_pdf_stream_fprintf (pdf, "%g %g l" PDF_EOL, bp->x3, bp->y3);
			break;
		case ART_CURVETO:
			sp_print_context_pdf_stream_fprintf (pdf, "%g %g %g %g %g %g c" PDF_EOL, bp->x1, bp->y1, bp->x2, bp->y2, bp->x3, bp->y3);
			break;
		default:
			break;
		}
		bp += 1;
	}
	if (started && closed) {
		sp_print_context_pdf_stream_fprintf (pdf, "h" PDF_EOL);
	}
#endif
}

static int
sp_print_context_pdf_fprintf (SPPrintContextPDF *pdf, const char *format, ...)
{
	va_list args;
	char *text;
	int len;

	va_start (args, format);
	text = g_strdup_vprintf (format, args);
	va_end (args);

	len = strlen (text);
	/* fputs (text, pdf->module->stream); */
	sp_print_context_write (SP_PRINT_CONTEXT(pdf), text, len);
	pdf->offset += len;

	g_free (text);

	return len;
}

static int
sp_print_context_pdf_fwrite (SPPrintContextPDF *pdf, const char *buffer, int len)
{
	/* fputs (buffer, pdf->module->stream); */
	sp_print_context_write (SP_PRINT_CONTEXT(pdf), buffer, len);
	pdf->offset += len;

	return len;
}

static int
sp_print_context_pdf_fprint_double (SPPrintContextPDF *pdf, double value)
{
	unsigned char c[32];
	int len;
	
	len = arikkei_dtoa_simple (c, 32, value, 6, 0, FALSE);
	/* fputs (c, pdf->module->stream); */
	sp_print_context_write (SP_PRINT_CONTEXT(pdf), c, len);
	pdf->offset += len;

	return len;
}

static int
sp_print_context_pdf_stream_fprintf (SPPrintContextPDF *pdf, const char *format, ...)
{
	va_list args;
	char *text;
	int len;

	va_start (args, format);
	text = g_strdup_vprintf (format, args);
	va_end (args);

	len = sp_pdf_buffer_write (pdf->stream, text, -1);

	g_free (text);

	return len;
}

static int
sp_print_context_pdf_stream_fprint_double (SPPrintContextPDF *pdf, double value)
{
	unsigned char c[32];
	int len;
	
	len = arikkei_dtoa_simple (c, 32, value, 6, 0, FALSE);

	sp_pdf_buffer_write (pdf->stream, c, len);

	return len;
}

static int
sp_print_context_pdf_write_contents (SPPrintContextPDF *pdf)
{
	int res = 0;

	printf ("pdf_write_contents\n");

	sp_print_context_pdf_object_start (pdf, pdf->contents_id);
	sp_print_context_pdf_fprintf (pdf, "/Length %d" PDF_EOL, pdf->stream->length);
	sp_print_context_pdf_object_start_stream (pdf, pdf->contents_id);
	sp_print_context_pdf_fwrite (pdf, pdf->stream->buffer, pdf->stream->length);
	sp_print_context_pdf_object_end (pdf, pdf->contents_id);

	return res;
}

static int
sp_print_context_pdf_write_resources (SPPrintContextPDF *pdf)
{
	SPPDFObject *obj;
	int res = 0;

	printf ("pdf_write_resources\n");

	sp_print_context_pdf_object_start (pdf, pdf->resources_id);
	sp_print_context_pdf_fprintf (pdf, "/ProcSet [/PDF");
#if 0
	if (pdf->use_images) {
		sp_print_context_pdf_fprintf (pdf, " /ImageC");
	}
#endif
	sp_print_context_pdf_fprintf (pdf, "]" PDF_EOL);

	sp_print_context_pdf_fprintf (pdf, "/XObject <<" PDF_EOL);
	for (obj = pdf->objects; obj; obj = obj->next) {
		if (obj->external) {
			sp_print_context_pdf_fprintf (pdf, "/%s %d 0 R" PDF_EOL, obj->name, obj->id);
		}
	}
	sp_print_context_pdf_fprintf (pdf, ">>" PDF_EOL);

	sp_print_context_pdf_object_end (pdf, pdf->resources_id);
	
	return res;
}

static int
sp_print_context_pdf_write_page (SPPrintContextPDF *pdf)
{
	int res = 0;
	
	res += sp_print_context_pdf_write_contents (pdf);

	res += sp_print_context_pdf_write_resources (pdf);

	return res;
}

static int
sp_print_context_pdf_write_pages (SPPrintContextPDF *pdf, int catalog_id)
{
	int res = 0;
	int pages_id;

	pages_id = sp_print_context_pdf_object_new (pdf);

	/* We have only single page */
	sp_print_context_pdf_object_start (pdf, pdf->page_id);
	sp_print_context_pdf_fprintf (pdf,
						  "/Type /Page" PDF_EOL
						  "/Parent %d 0 R" PDF_EOL
						  "/Resources %d 0 R" PDF_EOL
						  "/Contents %d 0 R" PDF_EOL,
						  pages_id,
						  pdf->resources_id,
						  pdf->contents_id);
	sp_print_context_pdf_object_end (pdf, pdf->page_id);

	/* Pages object */
	sp_print_context_pdf_object_start (pdf, pages_id);
	sp_print_context_pdf_fprintf (pdf,
						  "/Type /Pages" PDF_EOL
						  "/Kids [%d 0 R]" PDF_EOL /* Single page */
						  "/Count %d" PDF_EOL
						  "/MediaBox [0 0 %d %d]" PDF_EOL,
						  pdf->page_id,
						  1,
						  (int) pdf->width, (int) pdf->height);
	sp_print_context_pdf_object_end (pdf, pages_id);
	
	/* Catalog */
	sp_print_context_pdf_object_start (pdf, catalog_id);
	sp_print_context_pdf_fprintf (pdf,
						  "/Type /Catalog" PDF_EOL
						  "/Pages %d 0 R" PDF_EOL,
						  pages_id);
	sp_print_context_pdf_object_end (pdf, catalog_id);
	
	return res;
}

static int
sp_print_context_pdf_object_new (SPPrintContextPDF *pdf)
{
	SPPDFObject *object;

	object = (SPPDFObject*)malloc (sizeof (SPPDFObject));

	object->id = ++pdf->n_objects;
	object->offset = 0;
	object->state_dict = PDFO_DICT_NONE;
	object->state_stream = PDFO_STREAM_NONE;
	object->external = 0;
	object->name = NULL;

	if (pdf->objects) {
		object->next = pdf->objects;
	} else {
		object->next = NULL;
	}
	pdf->objects = object;

	return object->id;
}

static void
sp_print_context_pdf_object_start (SPPrintContextPDF *pdf, int object_id)
{
	SPPDFObject *object;

	object = pdf->objects;
	while (object && object->id != object_id) object = object->next;
	assert (object);

	object->offset = pdf->offset;

	sp_print_context_pdf_fprintf (pdf,
						  "%d 0 obj" PDF_EOL
						  "<<" PDF_EOL,
						  object_id);

	if (object->state_dict != PDFO_DICT_NONE) g_warning ("Dictionary state is invalid: object = %d, state = %d", object_id, object->state_dict);
	object->state_dict = PDFO_DICT_START;
}

static void
sp_print_context_pdf_object_start_stream (SPPrintContextPDF *pdf, int object_id)
{
	SPPDFObject *object;

	object = pdf->objects;
	while (object && object->id != object_id) object = object->next;
	assert (object);

	if (object->state_dict == PDFO_DICT_START) {
		sp_print_context_pdf_fprintf (pdf, ">>" PDF_EOL);
		object->state_dict = PDFO_DICT_END;
	}

	sp_print_context_pdf_fprintf (pdf, "stream" PDF_EOL);
	object->state_stream = PDFO_STREAM_START;
}

static void
sp_print_context_pdf_object_end (SPPrintContextPDF *pdf, int object_id)
{
	SPPDFObject *object;

	object = pdf->objects;
	while (object && object->id != object_id) object = object->next;
	assert (object);

	if (object->state_dict == PDFO_DICT_START) {
		sp_print_context_pdf_fprintf (pdf, ">>" PDF_EOL);
		object->state_dict = PDFO_DICT_END;
	} else if (object->state_stream == PDFO_STREAM_START) {
		sp_print_context_pdf_fprintf (pdf, "endstream" PDF_EOL);
		object->state_stream = PDFO_STREAM_END;
	}

	sp_print_context_pdf_fprintf (pdf, "endobj" PDF_EOL);
}

SPPDFObject *
sp_print_context_pdf_objects_reverse (SPPDFObject *objects)
{
	SPPDFObject *prev;

	prev = NULL;

	while (objects) {
		SPPDFObject *next;

		next = objects->next;
		objects->next = prev;

		prev = objects;
		objects = next;
	}

	return prev;
}

static SPPDFBuffer *
sp_pdf_buffer_new (void)
{
	SPPDFBuffer *buffer;

	buffer = (SPPDFBuffer *)malloc (sizeof(SPPDFBuffer));
	buffer->buffer = NULL;
	buffer->length = 0;
	buffer->alloc_size = 0;

	return buffer;
}

static SPPDFBuffer *
sp_pdf_buffer_delete (SPPDFBuffer *buffer)
{
	if (buffer->buffer) {
		free (buffer->buffer);
	}
	free (buffer);

	return NULL;
}

static int
sp_pdf_buffer_write (SPPDFBuffer *buffer, unsigned char *text, int length)
{
	if (!text || length == 0) return 0;

	if (length < 0) {
		unsigned char *p;
		p = text;
		for (length = 0; p && *p; p++) length += 1;
	}

	if (length + buffer->length + 1 > buffer->alloc_size) {
		int grow_size = PDF_BUFFER_INITIAL_SIZE;
		while (length + buffer->length + 1 > buffer->alloc_size) {
			buffer->alloc_size += grow_size;
			grow_size <<= 1;
		}
		buffer->buffer = (unsigned char *)realloc (buffer->buffer, buffer->alloc_size);
	}

	memcpy (buffer->buffer + buffer->length, text, length);
	buffer->length += length;
	buffer->buffer[buffer->length] = '\0';

	return length;
}

#define ASCII85_EOL '\n'

struct ASCII85Buf {
	unsigned int val;
	unsigned int len;
	unsigned int maxwidth;
	unsigned int width;
	unsigned int compressed : 1;
	/* Buffer */
	size_t bufsize;
	size_t buflen;
	unsigned char *buffer;
};

static void
sp_pdf_ascii85_setup (struct ASCII85Buf *buf)
{
	memset (buf, 0, sizeof (struct ASCII85Buf));
	buf->bufsize = 32768;
	buf->buffer = (unsigned char *) malloc (buf->bufsize);
	buf->maxwidth = 74;
}

static void
sp_pdf_ascii85_release (struct ASCII85Buf *buf)
{
	free (buf->buffer);
}

static void
sp_pdf_ascii85_append_raw (struct ASCII85Buf *buf, unsigned char byte)
{
	if (buf->buflen >= buf->bufsize) {
		buf->bufsize = buf->bufsize << 1;
		buf->buffer = (unsigned char *) realloc (buf->buffer, buf->bufsize);
	}
	buf->buffer[buf->buflen++] = byte;
}

static void
sp_pdf_ascii85_append_raw_bytes (struct ASCII85Buf *buf, unsigned char bytes[], unsigned int nbytes)
{
	unsigned int i;
	for (i = 0; i < nbytes; i++) {
		if (buf->width >= buf->maxwidth) {
			sp_pdf_ascii85_append_raw (buf, ASCII85_EOL);
			buf->width = 0;
		}
		sp_pdf_ascii85_append_raw (buf, bytes[i]);
		buf->width += 1;
	}
}

static void
sp_pdf_ascii85_flush (struct ASCII85Buf *buf)
{
	char c[5];
	unsigned int i;
	/* Special case - 4 zeroes */
	if ((buf->val == 0) && (buf->len == 4)) {
		sp_pdf_ascii85_append_raw_bytes (buf, "z", 1);
	} else {
		for (i = 0; i < buf->len + 1; i++) {
			c[buf->len - i] = (buf->val % 85) + '!';
			buf->val /= 85;
		}
		sp_pdf_ascii85_append_raw_bytes (buf, c, buf->len + 1);
	}
	buf->val = 0;
	buf->len = 0;
}


static void
sp_pdf_ascii85_append_byte (struct ASCII85Buf *buf, unsigned char byte)
{
	buf->val = (buf->val << 8) | byte;
	buf->len += 1;
	if (buf->len == 4) {
		sp_pdf_ascii85_flush (buf);
	}
}

static void
sp_pdf_ascii85_append_bytes (struct ASCII85Buf *buf, const unsigned char bytes[], unsigned int nbytes)
{
	unsigned int i;
	for (i = 0; i < nbytes; i++) {
		sp_pdf_ascii85_append_byte (buf, bytes[i]);
	}
}

static void
sp_pdf_ascii85_finish (struct ASCII85Buf *buf)
{
	sp_pdf_ascii85_flush (buf);
	sp_pdf_ascii85_append_raw (buf, '~');
	sp_pdf_ascii85_append_raw (buf, '>');
	sp_pdf_ascii85_append_raw (buf, ASCII85_EOL);
}

static unsigned int
sp_pdf_print_image (SPPrintContextPDF *pdf, unsigned char *px, unsigned int width, unsigned int height, unsigned int rs, const NRMatrixF *transform)
{
	unsigned char *rgb, *a;
	unsigned int rgblen, alen;
	unsigned int acompressed, rgbcompressed;
	unsigned char *drgb, *da;
	unsigned int x, y;
	unsigned long zsize;
	unsigned char *zbuf;
	int zresult;
	struct ASCII85Buf buf;
	int alphaid, rgbid, i;
	const char *filter;

	/* Fill initial buffers */
	rgblen = height * width * 3;
	alen = height * width;
	rgb = (unsigned char *) malloc (rgblen);
	a = (unsigned char *) malloc (alen);
	drgb = rgb;
	da = a;
	for (y = 0; y < height; y++) {
		unsigned char *s = px + y * rs;
		for (x = 0; x < width; x++) {
			*drgb++ = *s++;
			*drgb++ = *s++;
			*drgb++ = *s++;
			*da++ = *s++;
		}
	}
	/* Compress alpha */
	zsize = compressBound (alen);
	zbuf = (unsigned char *) malloc (zsize);
	zresult = compress2 (zbuf, &zsize, a, alen, 9);
	if ((zresult != Z_OK) || (zsize >= alen)) {
		free (zbuf);
		acompressed = 0;
	} else {
		free (a);
		a = zbuf;
		alen = zsize;
		acompressed = 1;
	}
	/* Encode alpha */
	sp_pdf_ascii85_setup (&buf);
	sp_pdf_ascii85_append_bytes (&buf, a, alen);
	sp_pdf_ascii85_finish (&buf);
	free (a);
	/* Write alpha */
	alphaid = sp_print_context_pdf_object_new (pdf);
	pdf->objects->external = 1;
	pdf->objects->name = g_strdup_printf ("Img%d", alphaid);
	sp_print_context_pdf_object_start (pdf, alphaid);
	filter = (acompressed) ? "[/ASCII85Decode /FlateDecode]" : "/ASCII85Decode";
	sp_print_context_pdf_fprintf (pdf,
						  "/Type /XObject" PDF_EOL
						  "/Subtype /Image" PDF_EOL
						  "/Width %d" PDF_EOL
						  "/Height %d" PDF_EOL
						  "/BitsPerComponent 8" PDF_EOL
						  "/ColorSpace /DeviceGray" PDF_EOL
						  "/Filter %s" PDF_EOL
						  "/Length %d" PDF_EOL,
						  width, height, filter,
						  buf.buflen);
	sp_print_context_pdf_object_start_stream (pdf, alphaid);
	sp_print_context_pdf_fwrite (pdf, buf.buffer, buf.buflen);
	sp_print_context_pdf_object_end (pdf, alphaid);
	sp_pdf_ascii85_release (&buf);

	/* Compress rgb */
	zsize = compressBound (rgblen);
	zbuf = (unsigned char *) malloc (zsize);
	zresult = compress2 (zbuf, &zsize, rgb, rgblen, 9);
	if ((zresult != Z_OK) || (zsize >= rgblen)) {
		free (zbuf);
		rgbcompressed = 0;
	} else {
		free (rgb);
		rgb = zbuf;
		rgblen = zsize;
		rgbcompressed = 1;
	}
	/* Encode rgb */
	sp_pdf_ascii85_setup (&buf);
	sp_pdf_ascii85_append_bytes (&buf, rgb, rgblen);
	sp_pdf_ascii85_finish (&buf);
	free (rgb);
	/* Write rgb */
	rgbid = sp_print_context_pdf_object_new (pdf);
	pdf->objects->external = 1;
	pdf->objects->name = g_strdup_printf ("Img%d", rgbid);
	sp_print_context_pdf_object_start (pdf, rgbid);
	filter = (rgbcompressed) ? "[/ASCII85Decode /FlateDecode]" : "/ASCII85Decode";
	sp_print_context_pdf_fprintf (pdf,
						  "/Type /XObject" PDF_EOL
						  "/Subtype /Image" PDF_EOL
						  "/Width %d" PDF_EOL
						  "/Height %d" PDF_EOL
						  "/BitsPerComponent 8" PDF_EOL
						  "/ColorSpace /DeviceRGB" PDF_EOL
						  "/SMask %d 0 R" PDF_EOL
						  "/Filter %s" PDF_EOL
						  "/Length %d" PDF_EOL,
						  width, height,
						  alphaid, filter,
						  buf.buflen);
	sp_print_context_pdf_object_start_stream (pdf, rgbid);
	sp_print_context_pdf_fwrite (pdf, buf.buffer, buf.buflen);
	sp_print_context_pdf_object_end (pdf, rgbid);
	sp_pdf_ascii85_release (&buf);

	sp_print_context_pdf_stream_fprintf (pdf, "q" PDF_EOL);
	for (i = 0; i < 6; i++) {
		sp_print_context_pdf_stream_fprint_double (pdf, transform->c[i]);
		sp_print_context_pdf_stream_fprintf (pdf, " ");
	}
	sp_print_context_pdf_stream_fprintf (pdf, "cm" PDF_EOL);
	sp_print_context_pdf_stream_fprintf (pdf, "/%s Do" PDF_EOL, pdf->objects->name);
	sp_print_context_pdf_stream_fprintf (pdf, "Q" PDF_EOL);

	return 0;
}

SPPrintContext *
sp_print_context_pdf_new (void)
{
	return (SPPrintContext *) arikkei_object_new (SP_TYPE_PRINT_CONTEXT_PDF);
}

/* End of GNU GPL code */

