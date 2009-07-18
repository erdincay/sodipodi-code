#ifndef __NR_PIXBLOCK_TEMPLATE_H__
#define __NR_PIXBLOCK_TEMPLATE_H__

/*
 * Pixel buffer rendering library
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#include <libnr/nr-pixblock.h>

// C++ Wrapper

#include <libnr/nr-blit.h>
#include <libnr/nr-pixblock-pattern.h>

namespace NR {

struct PixBlock {
public:
	enum Mode { G8, R8G8B8, R8G8B8A8N, R8G8B8A8P, INVALID };
private:
	// Not copyable
	PixBlock& operator= (const PixBlock& pixblock) { assert (false); return *this; }
public:
	NRPixBlock pb;

	// Constructor
	PixBlock (void) { nr_pixblock_setup_fast (&pb, NR_PIXBLOCK_MODE_G8, 0, 0, 0, 0, 0); }
	PixBlock (PixBlock& ppb) {
		nr_pixblock_setup_extern (&pb, ppb.pb.mode, ppb.pb.area.x0, ppb.pb.area.y0, ppb.pb.area.x1, ppb.pb.area.y1, NR_PIXBLOCK_PX (&ppb.pb), ppb.pb.rs, ppb.pb.empty, 0);
	}
	PixBlock (Mode mode, int x0, int y0, int x1, int y1, bool fast, bool clear) {
		if (fast) {
			nr_pixblock_setup_fast (&pb, mode, x0, y0, x1, y1, clear);
		} else {
			nr_pixblock_setup (&pb, mode, x0, y0, x1, y1, clear);
		}
	}
	PixBlock (Mode mode, int x0, int y0, int x1, int y1, unsigned char *px, int rs, bool empty, bool clear) {
		nr_pixblock_setup_extern (&pb, mode, x0, y0, x1, y1, px, rs, empty, clear);
	}
	// Destructor
	~PixBlock (void) { nr_pixblock_release (&pb); }
	// Initialization
	void clear (void) {
		nr_pixblock_release (&pb);
		nr_pixblock_setup_fast (&pb, G8, 0, 0, 0, 0, 0);
	}
	void set (Mode mode, int x0, int y0, int x1, int y1, bool fast, bool clear) {
		nr_pixblock_release (&pb);
		if (fast) {
			nr_pixblock_setup_fast (&pb, mode, x0, y0, x1, y1, clear);
		} else {
			nr_pixblock_setup (&pb, mode, x0, y0, x1, y1, clear);
		}
	}
	void set (Mode mode, int x0, int y0, int x1, int y1, unsigned char *px, int rs, bool empty, bool clear) {
		nr_pixblock_release (&pb);
		nr_pixblock_setup_extern (&pb, mode, x0, y0, x1, y1, px, rs, empty, clear);
	}
	void set (PixBlock& ppb) {
		nr_pixblock_release (&pb);
		nr_pixblock_setup_extern (&pb, ppb.pb.mode, ppb.pb.area.x0, ppb.pb.area.y0, ppb.pb.area.x1, ppb.pb.area.y1, NR_PIXBLOCK_PX (&ppb.pb), ppb.pb.rs, ppb.pb.empty, 0);
	}
	// Access
	bool isZero (void) const { return (pb.area.x0 >= pb.area.x1) || (pb.area.y0 >= pb.area.y1); }
	unsigned char *getPixels (void) { return NR_PIXBLOCK_PX (&pb); }
	const unsigned char *getPixels (void) const { return NR_PIXBLOCK_PX (&pb); }
	Mode getMode (void) const { return (Mode) pb.mode; }
	int getBPP (void) const { return NR_PIXBLOCK_BPP (&pb); }
	bool isEmpty (void) const { return pb.empty == 1; }
	int getRowStride (void) const { return (int) pb.rs; }
	int getWidth (void) const { return pb.area.x1 - pb.area.x0; }
	int getHeight (void) const { return pb.area.y1 - pb.area.y0; }
	int getX0 (void) const { return pb.area.x0; }
	int getX1 (void) const { return pb.area.x1; }
	int getY0 (void) const { return pb.area.y0; }
	int getY1 (void) const { return pb.area.y1; }
	bool hasAlpha (void) const { return nr_pixblock_has_alpha (&pb) != 0; }
	// Mark pixblock as nonempty
	void markDirty (void) { pb.empty = 0; }
	// Get row pointer
	unsigned char *getRow (int row) { return  NR_PIXBLOCK_PX (&pb) + row * pb.rs; }
	const unsigned char *getRow (int row) const { return  NR_PIXBLOCK_PX (&pb) + row * pb.rs; }
	// Get pixel pointer
	unsigned char *getPixel (int row, int col) { return NR_PIXBLOCK_PX (&pb) + row * pb.rs + col * NR_PIXBLOCK_BPP (&pb); }
	const unsigned char *getPixel (int row, int col) const { return NR_PIXBLOCK_PX (&pb) + row * pb.rs + col * NR_PIXBLOCK_BPP (&pb); }
	// Get minimal and maximal values
	void getChannelLimits (unsigned int minv[], unsigned int maxv[]) const { nr_pixblock_get_channel_limits (&pb, minv, maxv); }
	// Drawing
	void fill (unsigned int rgba32) { nr_pixblock_fill (&pb, rgba32); }
	// Blitting
	void blit (const PixBlock& source, unsigned int alpha) { nr_blit_pixblock_pixblock_alpha (&pb, &source.pb, alpha); if (alpha && !source.pb.empty) pb.empty = 0; }
	void drawMask (const PixBlock& mask, unsigned int rgba32) { nr_blit_pixblock_mask_rgba32 (&pb, &mask.pb, rgba32); if (rgba32 & 255) pb.empty = 0; }

	// Helper
	static int modeBPP (Mode mode) { return (mode == G8) ? 1 : (mode == R8G8B8) ? 3 : 4; }
};

} // Namespace NR

#endif
