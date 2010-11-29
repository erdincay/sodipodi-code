#ifndef __SP_IMAGE_LOADERS_H__
#define __SP_IMAGE_LOADERS_H__

/*
 * Bitmap format helpers
 *
 * Copyright (C) 2010 Lauris Kaplinski
 */

#include <stdlib.h>

#include <libnr/nr-pixblock.h>

unsigned int sp_image_load_from_file (NRPixBlock *px, const unsigned char *filename);
unsigned int sp_image_load_from_data (NRPixBlock *px, const unsigned char *cdata, size_t csize);

unsigned int sp_image_load_png_from_file (NRPixBlock *px, const unsigned char *filename);
unsigned int sp_image_load_png_from_data (NRPixBlock *px, const unsigned char *cdata, size_t csize);

#endif
