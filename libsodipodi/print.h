#ifndef __SP_PRINT_H__
#define __SP_PRINT_H__

/*
 * Frontend to printing
 *
 * Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

typedef struct _SPPrintContext SPPrintContext;
typedef struct _SPPrintContextClass SPPrintContextClass;

#define SP_TYPE_PRINT_CONTEXT (sp_print_context_get_type ())
#define SP_PRINT_CONTEXT(o) (ARIKKEI_CHECK_INSTANCE_CAST ((o), SP_TYPE_PRINT_CONTEXT, SPPrintContext))
#define SP_IS_PRINT_CONTEXT(o) (ARIKKEI_CHECK_INSTANCE_TYPE ((o), SP_TYPE_PRINT_CONTEXT))

#include <stdio.h>

#include <libarikkei/arikkei-object.h>

#include <libnr/nr-types.h>
#include <libnr/nr-path.h>

#include <libsodipodi/style.h>

struct _SPPrintContext {
	ArikkeiObject object;

	/* For bitmap generation */
	SPItem *root;
	NRArena *arena;
	unsigned int dkey;
	NRArenaItem *arenaroot;

	/* Output */
	void *data;
	unsigned int (* write) (SPPrintContext *ctx, const unsigned char *str, unsigned int strlen);
};

struct _SPPrintContextClass {
	ArikkeiObjectClass parent_class;

	/* Return number of bytes that were written to stream/memory */
	unsigned int (* begin) (SPPrintContext *ctx, SPDocument *doc);
	unsigned int (* finish) (SPPrintContext *ctx);
	unsigned int (* bind) (SPPrintContext *ctx, const NRMatrixF *transform, float opacity);
	unsigned int (* release) (SPPrintContext *ctx);
	unsigned int (* fill) (SPPrintContext *ctx, const NRPath *path, const NRMatrixF *ctm, const SPStyle *style, const NRRectF *pbox, const NRRectF *dbox, const NRRectF *bbox);
	unsigned int (* stroke) (SPPrintContext *ctx, const NRPath *path, const NRMatrixF *transform, const SPStyle *style, const NRRectF *pbox, const NRRectF *dbox, const NRRectF *bbox);
	unsigned int (* image) (SPPrintContext *ctx, unsigned char *px, unsigned int w, unsigned int h, unsigned int rs, const NRMatrixF *transform, const SPStyle *style);
};

unsigned int sp_print_context_get_type (void);

/* For subclass implementation - generate arena for subsequent bitmap fetch */
void sp_print_context_display (SPPrintContext *ctx, SPDocument *doc, const NRMatrixF *transform);
void sp_print_context_hide (SPPrintContext *ctx);
void sp_print_context_render (SPPrintContext *ctx, const NRRectL *bbox, unsigned char *px, int rs);
/* For subclass implementation - output data */
unsigned int sp_print_context_write (SPPrintContext *ctx, const unsigned char *str, unsigned int strlen);
unsigned int sp_print_context_printf (SPPrintContext *ctx, const char *format, ...);
/* Set up write handlers */
void sp_print_context_set_write_stream (SPPrintContext *ctx, FILE *ofs);

unsigned int sp_print_begin (SPPrintContext *ctx, SPDocument *doc);
unsigned int sp_print_finish (SPPrintContext *ctx);
unsigned int sp_print_bind (SPPrintContext *ctx, const NRMatrixF *transform, float opacity);
unsigned int sp_print_release (SPPrintContext *ctx);
unsigned int sp_print_fill (SPPrintContext *ctx, const NRPath *path, const NRMatrixF *ctm, const SPStyle *style, const NRRectF *pbox, const NRRectF *dbox, const NRRectF *bbox);
unsigned int sp_print_stroke (SPPrintContext *ctx, const NRPath *path, const NRMatrixF *transform, const SPStyle *style, const NRRectF *pbox, const NRRectF *dbox, const NRRectF *bbox);
unsigned int sp_print_image_R8G8B8A8_N (SPPrintContext *ctx, unsigned char *px, unsigned int w, unsigned int h, unsigned int rs, const NRMatrixF *transform, const SPStyle *style);

#endif
