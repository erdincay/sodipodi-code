#ifndef __ARIKKEI_STRLIB_H__
#define __ARIKKEI_STRLIB_H__

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

#ifdef __cplusplus
extern "C" {
#endif

unsigned int arikkei_strtod_simple (const unsigned char *str, unsigned int len, double *val);
unsigned int arikkei_strtod_exp (const unsigned char *str, unsigned int len, double *val);

/*
 * tprec - minimum number of significant digits
 * fprec - minimum number of fractional digits
 * padf - pad end with zeroes to get tprec or fprec
 *
 */

unsigned int arikkei_itoa (unsigned char *buf, unsigned int len, int val);
unsigned int arikkei_dtoa_simple (unsigned char *buf, unsigned int len, double val,
				  unsigned int tprec, unsigned int fprec, unsigned int padf);
unsigned int arikkei_dtoa_exp (unsigned char *buf, unsigned int len, double val,
			       unsigned int tprec, unsigned int padf);


unsigned int arikkei_unicode_utf8_bytes (unsigned int uval);
unsigned int arikkei_utf8_strlen (const unsigned char *str);
unsigned int arikkei_utf8_ucs2_strcpy (const unsigned char *s, unsigned short *d);
unsigned short *arikkei_utf8_ucs2_strdup (const unsigned char *s);
unsigned int arikkei_ucs2_strlen (const unsigned short *str);
unsigned int arikkei_ucs2_utf8_strcpy (const unsigned short *s, unsigned char *d);
unsigned char *arikkei_ucs2_utf8_strdup (const unsigned short *s);
unsigned int arikkei_ucs2_utf8_bytelen (const unsigned short *str);

unsigned short *arikkei_ucs2_strdup (const unsigned short *s);
unsigned int arikkei_ucs2_strncpy (const unsigned short *s, unsigned short *d, unsigned int maxlen);

#ifdef __cplusplus
};
#endif

#ifdef __cplusplus

namespace Arikkei {

// Suppress MSVC 2005 nagging
#ifdef WIN32
inline char *strdup (const char *str) { return ::_strdup (str); }
#else
inline char *strdup (const char *str) { return ::strdup (str); }
#endif

class UTEXT {
private:
	unsigned char *_utf8;
	unsigned short *_ucs2;
public:
	// Constructor
	UTEXT (void) : _utf8(0), _ucs2(0) {}
	UTEXT (const char *utf8) : _ucs2(0) { _utf8 = (unsigned char *) strdup (utf8); }
	UTEXT (const unsigned char *utf8) : _ucs2(0) { _utf8 = (unsigned char *) strdup ((const char *) utf8); }
	UTEXT (const unsigned short *ucs2) : _utf8(0) { _ucs2 = arikkei_ucs2_strdup (ucs2); }
	UTEXT (const UTEXT& utext) {
		_utf8 = (utext._utf8) ? (unsigned char *) strdup ((const char *) utext._utf8) : NULL;
		_ucs2 = (utext._ucs2 && !_utf8) ? arikkei_ucs2_strdup (utext._ucs2) : NULL;
	}
	// Destructor
	~UTEXT (void) { if (_utf8) free (_utf8); if (_ucs2) free (_ucs2); }
	// Operators
	operator const char * (void) { if (!_utf8 && _ucs2) _utf8 = arikkei_ucs2_utf8_strdup (_ucs2); return (const char *) _utf8; }
	operator const unsigned char * (void) { if (!_utf8 && _ucs2) _utf8 = arikkei_ucs2_utf8_strdup (_ucs2); return _utf8; }
	operator const unsigned short * (void) { if (!_ucs2 && _utf8) _ucs2 = arikkei_utf8_ucs2_strdup (_utf8); return _ucs2; }
	UTEXT& operator= (const UTEXT& utext) {
		if (_utf8) free (_utf8);
		_utf8 = (utext._utf8) ? (unsigned char *) strdup ((const char *) utext._utf8) : NULL;
		if (_ucs2) free (_ucs2);
		_ucs2 = (utext._ucs2 && !_utf8) ? arikkei_ucs2_strdup (utext._ucs2) : NULL;
		return *this;
	}
	operator bool (void) const { return _utf8 || _ucs2; }
	friend bool operator&& (const UTEXT& lhs, const UTEXT& rhs) { return (bool) lhs && (bool) rhs; }
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


#endif
