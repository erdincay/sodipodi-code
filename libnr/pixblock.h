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

class PixBlock {
public:
	enum Mode { G8, R8G8B8, R8G8B8A8N, R8G8B8A8P, NUM_MODES };
private:
	NRPixBlock _pb;
	// Not copyable
	PixBlock& operator= (const PixBlock& pixblock) { assert (false); return *this; }
public:
	// Constructor
	PixBlock (void) { nr_pixblock_setup_fast (&_pb, NR_PIXBLOCK_MODE_G8, 0, 0, 0, 0, 0); }
	PixBlock (PixBlock& pb) {
		nr_pixblock_setup_extern (&_pb, pb._pb.mode, pb._pb.area.x0, pb._pb.area.y0, pb._pb.area.x1, pb._pb.area.y1, NR_PIXBLOCK_PX (&pb._pb), pb._pb.rs, pb._pb.empty, 0);
	}
	PixBlock (Mode mode, int x0, int y0, int x1, int y1, bool fast, bool clear) {
		if (fast) {
			nr_pixblock_setup_fast (&_pb, mode, x0, y0, x1, y1, clear);
		} else {
			nr_pixblock_setup (&_pb, mode, x0, y0, x1, y1, clear);
		}
	}
	PixBlock (Mode mode, int x0, int y0, int x1, int y1, unsigned char *px, int rs, bool empty, bool clear) {
		nr_pixblock_setup_extern (&_pb, mode, x0, y0, x1, y1, px, rs, empty, clear);
	}
	// Destructor
	~PixBlock (void) { nr_pixblock_release (&_pb); }
	// Initialization
	void set (Mode mode, int x0, int y0, int x1, int y1, bool fast, bool clear) {
		nr_pixblock_release (&_pb);
		if (fast) {
			nr_pixblock_setup_fast (&_pb, mode, x0, y0, x1, y1, clear);
		} else {
			nr_pixblock_setup (&_pb, mode, x0, y0, x1, y1, clear);
		}
	}
	void set (Mode mode, int x0, int y0, int x1, int y1, unsigned char *px, int rs, bool empty, bool clear) {
		nr_pixblock_release (&_pb);
		nr_pixblock_setup_extern (&_pb, mode, x0, y0, x1, y1, px, rs, empty, clear);
	}
	void set (PixBlock& pb) {
		nr_pixblock_release (&_pb);
		nr_pixblock_setup_extern (&_pb, pb._pb.mode, pb._pb.area.x0, pb._pb.area.y0, pb._pb.area.x1, pb._pb.area.y1, NR_PIXBLOCK_PX (&pb._pb), pb._pb.rs, pb._pb.empty, 0);
	}
	// Access
	bool isZero (void) const { return (_pb.area.x0 >= _pb.area.x1) || (_pb.area.y0 >= _pb.area.y1); }
	unsigned char *getPixels (void) { return NR_PIXBLOCK_PX (&_pb); }
	const unsigned char *getPixels (void) const { return NR_PIXBLOCK_PX (&_pb); }
	Mode getMode (void) const { return (Mode) _pb.mode; }
	int getBPP (void) const { return NR_PIXBLOCK_BPP (&_pb); }
	bool isEmpty (void) const { return _pb.empty == 1; }
	int getRowStride (void) const { return (int) _pb.rs; }
	int getWidth (void) const { return _pb.area.x1 - _pb.area.x0; }
	int getHeight (void) const { return _pb.area.y1 - _pb.area.y0; }
	int getX0 (void) const { return _pb.area.x0; }
	int getX1 (void) const { return _pb.area.x1; }
	int getY0 (void) const { return _pb.area.y0; }
	int getY1 (void) const { return _pb.area.y1; }
	bool hasAlpha (void) const { return nr_pixblock_has_alpha (&_pb) != 0; }
	// Mark pixblock as nonempty
	void markDirty (void) { _pb.empty = 0; }
	// Get row pointer
	unsigned char *getRow (int row) { return  NR_PIXBLOCK_PX (&_pb) + row * _pb.rs; }
	const unsigned char *getRow (int row) const { return  NR_PIXBLOCK_PX (&_pb) + row * _pb.rs; }
	// Get pixel pointer
	unsigned char *getPixel (int row, int col) { return NR_PIXBLOCK_PX (&_pb) + row * _pb.rs + col * NR_PIXBLOCK_BPP (&_pb); }
	const unsigned char *getPixel (int row, int col) const { return NR_PIXBLOCK_PX (&_pb) + row * _pb.rs + col * NR_PIXBLOCK_BPP (&_pb); }
	// Get minimal and maximal values
	void getChannelLimits (unsigned int minv[], unsigned int maxv[]) const { nr_pixblock_get_channel_limits (&_pb, minv, maxv); }
	// Drawing
	void fill (unsigned int rgba32) { nr_pixblock_fill (&_pb, rgba32); }
	// Blitting
	void blit (const PixBlock& source, unsigned int alpha) { nr_blit_pixblock_pixblock_alpha (&_pb, &source._pb, alpha); if (alpha && !source._pb.empty) _pb.empty = 0; }
	void drawMask (const PixBlock& mask, unsigned int rgba32) { nr_blit_pixblock_mask_rgba32 (&_pb, &mask._pb, rgba32); if (rgba32 & 255) _pb.empty = 0; }
};

} // Namespace NR

#endif
