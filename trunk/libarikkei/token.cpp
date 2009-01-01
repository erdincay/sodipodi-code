#define __ARIKKEI_TOKEN_CPP__

//
// Krieg3
//
// Copyright (C) Lauris Kaplinski 2004
//

// Disable VS2005 nagging
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include <stdlib.h>
#include <stdio.h>

#include "arikkei-iolib.h"

#include "token.h"

#ifndef WIN32
#define _strnicmp strncasecmp
#define g_strdup strdup
#else
#define g_strdup _strdup
#endif

#ifndef MIN
#define MIN(l,r) (((r) < (l)) ? (r) : (l))
#endif

namespace Arikkei {

char *
Token<char>::_tdup (const char *cdata)
{
#ifdef WIN32
	return ::_strdup (cdata);
#else
	return ::strdup (cdata);
#endif
}

// Member operators

const char *
Token<char>::getPData (void)
{
	if (!_pdata && _cdata) {
		_pdata = (char *) malloc ((_len + 1) * sizeof (char));
		if (_len > 0) memcpy (_pdata, _cdata, _len * sizeof (char));
		_pdata[_len] = 0;
	}
	return _pdata;
}

int
Token<char>::getInt (void) const
{
	char c[64];
	strncpy (c, 64);
	return atoi (c);
}

unsigned int
Token<char>::getUInt (void) const
{
	char c[64];
	strncpy (c, 64);
#ifdef WIN32
	return (unsigned int) _atoi64 (c);
#else
	return (unsigned int) strtoul (c, NULL, 10);
#endif
}

float
Token<char>::getFloat (void) const
{
	char c[64];
	strncpy (c, 64);
	return (float) atof (c);
}

double
Token<char>::getDouble (void) const
{
	char c[64];
	strncpy (c, 64);
	return atof (c);
}

// Parsing

template<>
Token<char>
Token<char>::getLine (void) const
{
	// Test if empty
	if (isEmpty ()) return *this;
	// Drop sign
	unsigned char *p = (unsigned char *) _cdata;
	// Search end
	size_t e = 0;
	while ((e < _len) && ((p[e] >= 32) || (p[e] == 9))) e += 1;
	return Token<char> (_cdata, e);
}

Token<unsigned short>
Token<unsigned short>::getLine (void) const
{
	// Test if empty
	if (isEmpty ()) return *this;
	// Drop sign
	unsigned short *p = (unsigned short *) _cdata;
	// Search end
	size_t e = 0;
	while ((e < _len) && ((p[e] >= 32) || (p[e] == 9))) e += 1;
	return Token<unsigned short> (_cdata, e);
}

Token<char>
Token<char>::nextLine (const Token& line) const
{
	if (isEmpty ()) return *this;
	if (!line.isValid ()) return getLine ();
	// Drop sign
	unsigned char *p = (unsigned char *) _cdata;
	size_t s = (line.getCData () - _cdata) + line.getLength ();
	while ((s < _len) && (p[s] < 32) && (p[s] != 9)) s += 1;
	size_t e = s;
	while ((e < _len) && ((p[e] >= 32) || (p[e] == 9))) e += 1;
	return Token<char> (_cdata, s, e);
}

Token<unsigned short>
Token<unsigned short>::nextLine (const Token& line) const
{
	if (isEmpty ()) return *this;
	if (!line.isValid ()) return getLine ();
	// Drop sign
	unsigned short *p = (unsigned short *) _cdata;
	size_t s = (line.getCData () - _cdata) + line.getLength ();
	while ((s < _len) && (p[s] < 32) && (p[s] != 9)) s += 1;
	size_t e = s;
	while ((e < _len) && ((p[e] >= 32) || (p[e] == 9))) e += 1;
	return Token<unsigned short> (_cdata, s, e);
}

void
Token<char>::skipUTF8Signature (void)
{
	unsigned char *p = (unsigned char *) _cdata;
	if ((p[0] == 0xef) && (p[1] == 0xbb) && (p[2] == 0xbf)) {
		_cdata += 3;
		_len -= 3;
	}
}

char *
Token<char>::strconcat (const Token<char> *tokens, int size, const Token<char>& separator)
{
	size_t len = 1;
	for (int i = 0; i < size; i++) len += tokens[i].getLength ();
	len += (size - 1) * separator.getLength ();
	char *str = (char *) ::malloc (len);
	char *p = str;
	for (int i = 0; i < size; i++) {
		if (i > 0) p += separator.strcpy (p);
		p += tokens[i].strcpy (p);
	}
	return str;
}

// Line iterator

bool
TokenLine<char>::forward (bool skipempty)
{
	if (eof ()) return false;
	Token n = _master.nextLine (*this);
	set (n);
	if (eof ()) return false;
	if (skipempty) {
		Token st = strip ();
		while (st.isEmpty ()) {
			Token n = _master.nextLine (*this);
			set (n);
			Token st = strip ();
			if (eof ()) return false;
		}
	}
	return true;
}

bool
TokenLine<unsigned short>::forward (bool skipempty)
{
	if (eof ()) return false;
	Token n = _master.nextLine (*this);
	set (n);
	if (eof ()) return false;
	if (skipempty) {
		Token st = strip ();
		while (st.isEmpty ()) {
			Token n = _master.nextLine (*this);
			set (n);
			Token st = strip ();
			if (eof ()) return false;
		}
	}
	return true;
}

// TokenMMap

TokenMMap::TokenMMap (const CToken& filename, const CToken& mapid)
: CToken(), _filename(filename.strdup ())
{
	char b[32];
	if (mapid.isEmpty ()) {
		static int mapc = 0;
		sprintf (b, "EZD_mapid_%d", mapc++);
		_mapid = g_strdup (b);
	} else {
		_mapid = mapid.strdup ();
	}
	_data = (char *) arikkei_mmap ((const unsigned char *) _filename, &_size, (const unsigned char *) _mapid);
	set (_data, 0, _size);
}

TokenMMap::~TokenMMap (void)
{
	if (_data) arikkei_munmap ((const unsigned char *) _data, _size);
	if (_mapid) free (_mapid);
	if (_filename) free (_filename);
}

void
TokenMMap::skipSignature (void)
{
	const unsigned char *cdata = (const unsigned char *) getCData ();
	if ((cdata[0] == 0xef) && (cdata[1] == 0xbb) && (cdata[2] == 0xbf)) {
		set (3, getLength ());
	}
}

bool
Token<char>::parseKeywordString (Token<char>& lt, const char *keyword, Token<char>& text, bool removequotes)
{
	Token<char> st(lt.strip ());
	Token<char> tokenz[2];
	int ntokenz = st.tokenize (tokenz, 3, true, true, true);
	if (ntokenz != 2) return false;
	if (tokenz[0] != keyword) return false;
	if (removequotes) {
		text = tokenz[1].strip (Token("'\""));
	} else {
		text = tokenz[1];
	}
	return true;
}

bool
Token<char>::parseKeywordUInt (Token<char>& lt, const char *keyword, unsigned int& val)
{
	Token<char> st(lt.strip ());
	Token<char> tokenz[2];
	int ntokenz = st.tokenize (tokenz, 3, true, true, true);
	if (ntokenz != 2) return false;
	if (tokenz[0] != keyword) return false;
	val = (unsigned int) tokenz[1].strip ();
	return true;
}

bool
Token<char>::parseKeywordInt (Token<char>& lt, const char *keyword, int& val)
{
	Token<char> st(lt.strip ());
	Token<char> tokenz[2];
	int ntokenz = st.tokenize (tokenz, 3, true, true, true);
	if (ntokenz != 2) return false;
	if (tokenz[0] != keyword) return false;
	val = (int) tokenz[1].strip ();
	return true;
}

} // Namespace Arikkei
