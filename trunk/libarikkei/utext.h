#ifndef __ARIKKEI_UTEXT_H__
#define __ARIKKEI_UTEXT_H__

/*
 * Arikkei
 *
 * Basic datatypes and code snippets
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 *
 */

#include <string.h>

#include <libarikkei/arikkei-strlib.h>

namespace Arikkei {

// Suppress MSVC 2005 nagging
#ifdef WIN32
inline char *strdup (const char *str) { return ::_strdup (str); }
#else
inline char *strdup (const char *str) { return ::strdup (str); }
#endif

class UText {
private:
	unsigned char *_utf8;
	unsigned short *_ucs2;
public:
	// Constructor
	UText (void) : _utf8(0), _ucs2(0) {}
	UText (const char *utf8) : _ucs2(0) { _utf8 = (unsigned char *) strdup (utf8); }
	UText (const unsigned char *utf8) : _ucs2(0) { _utf8 = (unsigned char *) strdup ((const char *) utf8); }
	UText (const unsigned char *utf8, size_t nbytes) : _ucs2(0) {
		_utf8 = (unsigned char *) malloc (nbytes + 1);
		memcpy (_utf8, utf8, nbytes);
		_utf8[nbytes] = 0;
	}
	UText (const unsigned short *ucs2) : _utf8(0) { _ucs2 = arikkei_ucs2_strdup (ucs2); }
	UText (const unsigned short *ucs2, size_t len) : _utf8(0) {
		_ucs2 = (unsigned short *) malloc ((len + 1) * sizeof (unsigned short));
		memcpy (_ucs2, ucs2, len * sizeof (unsigned short));
		_ucs2[len] = 0;
	}
	UText (const UText& utext) {
		_utf8 = (utext._utf8) ? (unsigned char *) strdup ((const char *) utext._utf8) : NULL;
		_ucs2 = (utext._ucs2 && !_utf8) ? arikkei_ucs2_strdup (utext._ucs2) : NULL;
	}
	// Destructor
	~UText (void) { if (_utf8) free (_utf8); if (_ucs2) free (_ucs2); }
	// Operators
	operator const char * (void) { if (!_utf8 && _ucs2) _utf8 = arikkei_ucs2_utf8_strdup (_ucs2); return (const char *) _utf8; }
	operator const unsigned char * (void) { if (!_utf8 && _ucs2) _utf8 = arikkei_ucs2_utf8_strdup (_ucs2); return _utf8; }
	operator const unsigned short * (void) { if (!_ucs2 && _utf8) _ucs2 = arikkei_utf8_ucs2_strdup (_utf8); return _ucs2; }
	UText& operator= (const UText& utext) {
		if (_utf8) free (_utf8);
		_utf8 = (utext._utf8) ? (unsigned char *) strdup ((const char *) utext._utf8) : NULL;
		if (_ucs2) free (_ucs2);
		_ucs2 = (utext._ucs2 && !_utf8) ? arikkei_ucs2_strdup (utext._ucs2) : NULL;
		return *this;
	}
	operator bool (void) const { return _utf8 || _ucs2; }
	friend bool operator&& (const UText& lhs, const UText& rhs) { return (bool) lhs && (bool) rhs; }
	// Methods
	size_t getLengthChars (void) const {
		if (_ucs2) return (size_t) arikkei_ucs2_strlen (_ucs2);
		if (_utf8) return arikkei_utf8_strlen (_utf8);
		return 0;
	}
	size_t getLengthUTF8Bytes (void) const {
		if (_utf8) return strlen ((const char *) _utf8);
		if (_ucs2) return (size_t) arikkei_ucs2_utf8_bytelen (_ucs2);
		return 0;
	}
};

} // Namespace Arikkei

#endif





