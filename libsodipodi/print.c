#define __SP_PRINT_C__

/*
 * Frontend to printing
 *
 * Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#include <string.h>

#include <libnrarena/nr-arena-item.h>

#include "document.h"
#include "sp-item.h"

#include "print.h"

static void sp_print_context_class_init (SPPrintContextClass *klass);
static void sp_print_context_init (SPPrintContext *ctx);
static void sp_print_context_finalize (ArikkeiObject *object);

static ArikkeiObjectClass *parent_class;

unsigned int
sp_print_context_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		type = arikkei_object_register_type (ARIKKEI_TYPE_OBJECT,
						"SPPrintContext",
						sizeof (SPPrintContextClass),
						sizeof (SPPrintContext),
						(void (*) (ArikkeiObjectClass *)) sp_print_context_class_init,
						(void (*) (ArikkeiObject *)) sp_print_context_init);
	}
	return type;
}

static void
sp_print_context_class_init (SPPrintContextClass *klass)
{
	ArikkeiObjectClass *object_class;

	object_class = (ArikkeiObjectClass *) klass;

	parent_class = ((ArikkeiObjectClass *) klass)->parent;

	object_class->finalize = sp_print_context_finalize;
}

static void
sp_print_context_init (SPPrintContext *ctx)
{
}

static void
sp_print_context_finalize (ArikkeiObject *object)
{
	SPPrintContext *ctx;
	ctx = SP_PRINT_CONTEXT(object);
	if (ctx->root) {
		sp_item_invoke_hide (ctx->root, ctx->dkey);
	}
	if (ctx->arenaroot) {
		arikkei_object_unref (ARIKKEI_OBJECT(ctx->arenaroot));
	}
	if (ctx->arena) {
		arikkei_object_unref ((ArikkeiObject *) ctx->arena);
	}

	((ArikkeiObjectClass *) (parent_class))->finalize (object);
}

void
sp_print_context_display (SPPrintContext *ctx, SPDocument *doc, const NRMatrixF *transform)
{
	ctx->root = SP_ITEM(doc->root);
	ctx->arena = (NRArena *) arikkei_object_new (NR_TYPE_ARENA);
	ctx->dkey = sp_item_display_key_new (1);
	ctx->arenaroot = sp_item_invoke_show (ctx->root, ctx->arena, ctx->dkey, SP_ITEM_SHOW_PRINT);
	nr_arena_item_set_transform (ctx->arenaroot, transform);
}

void
sp_print_context_hide (SPPrintContext *ctx)
{
	sp_item_invoke_hide (ctx->root, ctx->dkey);
	ctx->root = NULL;
	ctx->dkey = 0;
	arikkei_object_unref (ARIKKEI_OBJECT(ctx->arenaroot));
	ctx->arenaroot = NULL;
	arikkei_object_unref ((ArikkeiObject *) ctx->arena);
	ctx->arena = NULL;
}

void
sp_print_context_render (SPPrintContext *ctx, const NRRectL *bbox, unsigned char *px, int rs)
{
	NRPixBlock pb;
	NRGC gc;

	/* Update to renderable state */
	nr_matrix_f_set_identity (&gc.transform);
	nr_arena_item_invoke_update (ctx->arenaroot, bbox, &gc, NR_ARENA_ITEM_STATE_ALL, NR_ARENA_ITEM_STATE_NONE);
	nr_pixblock_setup_extern (&pb, NR_PIXBLOCK_MODE_R8G8B8A8N, bbox->x0, bbox->y0, bbox->x1, bbox->y1, px, rs, FALSE, FALSE);
	nr_arena_item_invoke_render (ctx->arenaroot, bbox, &pb, 0);
	nr_pixblock_release (&pb);
}

unsigned int
sp_print_context_write (SPPrintContext *ctx, const unsigned char *str, unsigned int strlen)
{
	if (ctx->write) {
		return ctx->write (ctx, str, strlen);
	}
	return 0;
}

unsigned int
sp_print_context_printf (SPPrintContext *ctx, const char *format, ...)
{
	va_list args;
	char *str;
	int len;

	va_start (args, format);
	str = g_strdup_vprintf (format, args);
	va_end (args);

	len = strlen (str);
	if (ctx->write) {
		len = ctx->write (ctx, str, len);
	}

	g_free (str);

	return len;
}

static unsigned int
sp_print_context_write_stream (SPPrintContext *ctx, const unsigned char *str, unsigned int strlen)
{
	FILE *ofs;
	ofs = (FILE *) ctx->data;
	return fwrite (str, 1, strlen, ofs);
}

void
sp_print_context_set_write_stream (SPPrintContext *ctx, FILE *ofs)
{
	arikkei_return_if_fail (ctx != NULL);
	arikkei_return_if_fail (SP_IS_PRINT_CONTEXT(ctx));

	ctx->data = ofs;
	ctx->write = sp_print_context_write_stream;
}

unsigned int
sp_print_begin (SPPrintContext *ctx, SPDocument *doc)
{
	if (((SPPrintContextClass *) ((ArikkeiObject *) ctx)->klass)->begin)
		return ((SPPrintContextClass *) ((ArikkeiObject *) ctx)->klass)->begin (ctx, doc);
	return 0;

}

unsigned int
sp_print_finish (SPPrintContext *ctx)
{
	if (((SPPrintContextClass *) ((ArikkeiObject *) ctx)->klass)->finish)
		return ((SPPrintContextClass *) ((ArikkeiObject *) ctx)->klass)->finish (ctx);
	return 0;
}

unsigned int
sp_print_bind (SPPrintContext *ctx, const NRMatrixF *transform, float opacity)
{
	if (((SPPrintContextClass *) ((ArikkeiObject *) ctx)->klass)->bind)
		((SPPrintContextClass *) ((ArikkeiObject *) ctx)->klass)->bind (ctx, transform, opacity);
	return 0;
}

unsigned int
sp_print_release (SPPrintContext *ctx)
{
	if (((SPPrintContextClass *) ((ArikkeiObject *) ctx)->klass)->release)
		((SPPrintContextClass *) ((ArikkeiObject *) ctx)->klass)->release (ctx);
	return 0;
}

unsigned int
sp_print_fill (SPPrintContext *ctx, const NRPath *path, const NRMatrixF *ctm, const SPStyle *style, const NRRectF *pbox, const NRRectF *dbox, const NRRectF *bbox)
{
	if (((SPPrintContextClass *) ((ArikkeiObject *) ctx)->klass)->fill)
		((SPPrintContextClass *) ((ArikkeiObject *) ctx)->klass)->fill (ctx, path, ctm, style, pbox, dbox, bbox);
	return 0;
}

unsigned int
sp_print_stroke (SPPrintContext *ctx, const NRPath *path, const NRMatrixF *ctm, const SPStyle *style, const NRRectF *pbox, const NRRectF *dbox, const NRRectF *bbox)
{
	if (((SPPrintContextClass *) ((ArikkeiObject *) ctx)->klass)->stroke)
		((SPPrintContextClass *) ((ArikkeiObject *) ctx)->klass)->stroke (ctx, path, ctm, style, pbox, dbox, bbox);
	return 0;
}

unsigned int
sp_print_image_R8G8B8A8_N (SPPrintContext *ctx, unsigned char *px, unsigned int w, unsigned int h, unsigned int rs, const NRMatrixF *transform, const SPStyle *style)
{
	if (((SPPrintContextClass *) ((ArikkeiObject *) ctx)->klass)->image)
		((SPPrintContextClass *) ((ArikkeiObject *) ctx)->klass)->image (ctx, px, w, h, rs, transform, style);
	return 0;
}

