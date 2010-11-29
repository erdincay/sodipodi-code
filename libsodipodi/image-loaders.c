#define __SP_IMAGE_LOADERS_C__

/*
 * Bitmap format helpers
 *
 * Copyright (C) 2010 Lauris Kaplinski
 */

#include <stdlib.h>

#include <libarikkei/arikkei-iolib.h>

#include <libpng/png.h>

#include "image-loaders.h"

unsigned int
sp_image_load_from_file (NRPixBlock *px, const unsigned char *filename)
{
	const unsigned char *cdata;
	size_t csize;
	unsigned int result;

	cdata = arikkei_mmap (filename, &csize, NULL);
	if (!cdata) return 0;
	result = sp_image_load_from_data (px, cdata, csize);
	arikkei_munmap (cdata, csize);

	return result;
}

unsigned int
sp_image_load_from_data (NRPixBlock *px, const unsigned char *cdata, size_t csize)
{
	if (sp_image_load_png_from_data (px, cdata, csize)) return 1;

	return 0;
}

unsigned int
sp_image_load_png_from_file (NRPixBlock *px, const unsigned char *filename)
{
	const unsigned char *cdata;
	size_t csize;
	unsigned int result;

	cdata = arikkei_mmap (filename, &csize, NULL);
	if (!cdata) return 0;
	result = sp_image_load_png_from_data (px, cdata, csize);
	arikkei_munmap (cdata, csize);

	return result;
}

static void
png_cpexcept_error (png_structp png_ptr, png_const_charp msg) 
{ 
   if (png_ptr) fprintf (stderr, "PNG ERROR: %s", msg);
} 

struct FileData {
	const unsigned char *cdata;
	size_t cpos;
	size_t csize;
};

static void
user_read_data_fcn (png_structp png_ptr, png_bytep data, png_size_t length)
{
	struct FileData *fdata;
	
	fdata = (struct FileData *) png_ptr->io_ptr;
	if ((fdata->cpos + length) > fdata->csize) {
		png_error (png_ptr, "overflow");
		return;
	}
	memcpy (data, fdata->cdata + fdata->cpos, length);
	fdata->cpos += length;
} 

unsigned int
sp_image_load_png_from_data (NRPixBlock *px, const unsigned char *cdata, size_t csize)
{
	png_structp png_ptr;
	png_infop info_ptr;
	struct FileData fdata;
	unsigned int width, height;
	int bitdepth, colortype, interlace, compression, filter;
	int type, bpp;
	unsigned int y;

	if (!cdata || (csize < 8)) return 0;
	if (!png_check_sig ((png_bytep) cdata, 8)) return 0;

	/* Allocate the png read struct */
	png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, 0 , (png_error_ptr) png_cpexcept_error, (png_error_ptr) 0); 
	if (!png_ptr) {
		fprintf (stderr, "sp_image_load_png_from_data: Cannot allocate PNG read struct\n");
		return 0;
	}

	/* Allocate the png info struct */
	info_ptr = png_create_info_struct (png_ptr);
	if (!info_ptr) {
		fprintf (stderr, "sp_image_load_png_from_data: Cannot allocate PNG info struct\n");
		png_destroy_read_struct (&png_ptr, 0, 0); 
		return 0;
	}

	/* FileData fdata ((const char *) cdata, 8, csize); */
	fdata.cdata = cdata;
	fdata.csize = csize;
	fdata.cpos = 8;

	png_set_read_fn (png_ptr, &fdata, user_read_data_fcn); 
	/* Tell png that we read the signature */
	png_set_sig_bytes (png_ptr, 8);
	/* Read the info section of the png file */
	png_read_info (png_ptr, info_ptr);

	/* Extract info */
	png_get_IHDR (png_ptr, info_ptr, (png_uint_32 *) &width, (png_uint_32 *) &height, &bitdepth, &colortype, &interlace, &compression, &filter);

	if (bitdepth != 8) {
		fprintf (stderr, "sp_image_load_png_from_data: Only 8-bit channels supported\n");
		png_destroy_read_struct (&png_ptr, &info_ptr, 0); 
		return 0;
	}
	
	if (colortype == PNG_COLOR_TYPE_RGBA) {
		type = NR_PIXBLOCK_MODE_R8G8B8A8N;
		bpp = 4;
	} else if (colortype == PNG_COLOR_TYPE_RGB) {
		type = NR_PIXBLOCK_MODE_R8G8B8;
		bpp = 3;
	} else if (colortype == PNG_COLOR_TYPE_GRAY) {
		type = NR_PIXBLOCK_MODE_G8;
		bpp = 1;
	} else {
		fprintf (stderr, "sp_image_load_png_from_data: Only RGB and RGBA formats supported\n");
		png_destroy_read_struct (&png_ptr, 0, 0); 
		return 0;
	}
	
	if (interlace != PNG_INTERLACE_NONE) {
		fprintf (stderr, "sp_image_load_png_from_data: Interlaced formats not supported\n");
		png_destroy_read_struct (&png_ptr, &info_ptr, 0); 
		return 0;
	}

	/* Update the changes */
	png_read_update_info (png_ptr, info_ptr); 

	png_get_IHDR (png_ptr, info_ptr, (png_uint_32*) &width, (png_uint_32*) &height, &bitdepth,&colortype, &interlace, &compression, &filter);

	/* Check the number of bytes per row */
	/* u32 bytes_per_row = png_get_rowbytes (png_ptr, info_ptr);*/

	/* pxb.set ((NR::PixBlock::Mode) type, 0, 0, width, height, false, false); */
	nr_pixblock_setup (px, type, 0, 0, width, height, 0);

	for (y = 0; y < height; y++) {
		png_read_row (png_ptr, (png_bytep) NR_PIXBLOCK_ROW (px, y), 0);
	}

	png_destroy_read_struct (&png_ptr, &info_ptr, 0);

	px->empty = 0;

	return 1;
}




