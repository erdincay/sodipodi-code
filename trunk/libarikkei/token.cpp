#define __TOKEN_CPP__

//
// Krieg3
//
// Copyright (C) Lauris Kaplinski 2004
//

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "arikkei-iolib.h"

#include "arikkei-token.h"

#ifndef WIN32
#define _strnicmp strncasecmp
#endif

#ifndef MIN
#define MIN(l,r) (((r) < (l)) ? (r) : (l))
#endif

namespace Arikkei {

void
Token::set (const Token& token)
{
	if ((_cdata != token.getCData ()) || (_len != token.getLength ())) {
		if (_pdata) free (_pdata);
		_pdata = NULL;
		_cdata = token.getCData ();
		_len = token.getLength ();
	}
}

void
Token::set (const char *cdata, size_t start, size_t end)
{
	if (_pdata) free (_pdata);
	_pdata = NULL;
	_cdata = cdata + start;
	_len = end - start;
}

void
Token::set (size_t start, size_t end)
{
	if (_pdata) free (_pdata);
	_pdata = NULL;
	_cdata = _cdata + start;
	_len = end - start;
}


bool
Token::getBool (void) const
{
	if (isEmpty ()) return false;
	if (!stricmp ("yes")) return true;
	if (!stricmp ("no")) return false;
	if (!stricmp ("true")) return true;
	if (!stricmp ("false")) return false;
	char c[64];
	strncpy (c, 64);
	return atoi (c) != 0;
}

// Member operators

Token::operator int (void) const
{
	char c[64];
	strncpy (c, 64);
	return atoi (c);
}

Token::operator unsigned int (void) const
{
	char c[64];
	strncpy (c, 64);
#ifdef WIN32
	return (unsigned int) _atoi64 (c);
#else
	return (unsigned int) strtoul (c, NULL, 10);
#endif
}

Token::operator float (void) const
{
	char c[64];
	strncpy (c, 64);
	return (float) atof (c);
}

Token::operator double (void) const
{
	char c[64];
	strncpy (c, 64);
	return atof (c);
}

Token::operator const char * (void)
{
	if (!_pdata && _cdata) {
		_pdata = (char *) malloc (_len + 1);
		if (_len > 0) ::strncpy (_pdata, _cdata, _len);
		_pdata[_len] = 0;
	}
	return _pdata;
}

// Friend operators

bool
operator== (const Token& lhs, const Token& rhs)
{
	if (!lhs.isValid ()) return false;
	if (!rhs.isValid ()) return false;
	if (!lhs.isEmpty ()) {
		if (!rhs.isEmpty ()) {
			size_t len = lhs.getLength ();
			if (len != rhs.getLength ()) return false;
			return !::strncmp (lhs.getCData (), rhs.getCData (), len);
		} else {
			return false;
		}
	} else {
		if (!rhs.isEmpty ()) {
			return false;
		} else {
			return true;
		}
	}
}

// Standard string methods

char *
Token::strdup (void) const
{
	if (!_cdata) return NULL;
	char *b = (char *) malloc (_len + 1);
	if (_len) ::strncpy (b, _cdata, _len);
	b[_len] = 0;
	return b;
}

size_t
Token::strcpy (char *str) const
{
	if (_len) ::strncpy (str, _cdata, _len);
	str[_len] = 0;
	return _len;
}

size_t
Token::strncpy (char  *str, size_t size) const
{
	if (size < 1) return 0;
	size_t len = MIN (_len, size - 1);
	if (len) ::strncpy (str, _cdata, len);
	str[len] = 0;
	return len;
}

int
Token::strcmp (const char *str) const
{
	if (!_cdata) return (str) ? -1 : 0;
	if (!str) return 1;
	int cval = strncmp (str, _len);
	if (cval) return cval;
	if (_len < strlen (str)) return -1;
	return 0;
}

int
Token::strcmp (const Token& token) const
{
	if (!_cdata) return (token.isValid ()) ? -1 : 0;
	if (!token.isValid ()) return 1;
	size_t len = MIN (_len, token.getLength ());
	int cval = ::strncmp (_cdata, token.getCData (), len);
	if (cval) return cval;
	if (len < token.getLength ()) return -1;
	if (len < _len) return 1;
	return 0;
}

int
Token::strncmp (const char *str, size_t size) const
{
	if (!_cdata) return (str) ? -1 : 0;
	if (!str) return 1;
	if (!_len) return (size && *str) ? -1 : 0;
	if (!size || !*str) return 1;

	size_t alen = strlen (str);
	size_t len = MIN (alen, _len);
	len = MIN (len, size);
	int cval = ::strncmp (_cdata, str, len);
	if (cval) return cval;
	if (len >= size) return 0;
	if (_len < alen) return -1;
	if (_len > alen) return 1;
	return 0;
}

int
Token::strncmp (const Token& token, size_t size) const
{
	if (!_cdata) return (token.isValid ()) ? -1 : 0;
	if (!token.isValid ()) return 1;
	if (!_len) return (size && token.getLength ()) ? -1 : 0;
	if (!size || !token.getLength ()) return 1;

	size_t alen = token.getLength ();
	size_t len = MIN (alen, _len);
	len = MIN (len, size);
	int cval = ::strncmp (_cdata, token.getCData (), len);
	if (cval) return cval;
	if (len >= size) return 0;
	if (_len < alen) return -1;
	if (_len > alen) return 1;
	return 0;
}

int
Token::stricmp (const char *str) const
{
	if (!_cdata) return (str) ? -1 : 0;
	if (!str) return 1;
	int cval = strnicmp (str, _len);
	if (cval) return cval;
	if (_len < strlen (str)) return -1;
	return 0;
}

int
Token::stricmp (const Token& token) const
{
	if (!_cdata) return (token.isValid ()) ? -1 : 0;
	if (!token.isValid ()) return 1;
	size_t len = MIN (_len, token.getLength ());
	int cval = ::_strnicmp (_cdata, token.getCData (), len);
	if (cval) return cval;
	if (len < token.getLength ()) return -1;
	if (len < _len) return 1;
	return 0;
}

int
Token::strnicmp (const char *str, size_t size) const
{
	if (!_cdata) return (str) ? -1 : 0;
	if (!str) return 1;
	if (!_len) return (size && *str) ? -1 : 0;
	if (!size || !*str) return 1;

	size_t alen = strlen (str);
	size_t len = MIN (alen, _len);
	len = MIN (len, size);
	int cval = ::_strnicmp (_cdata, str, len);
	if (cval) return cval;
	if (len >= size) return 0;
	if (_len < alen) return -1;
	if (_len > alen) return 1;
	return 0;
}

int
Token::strnicmp (const Token& token, size_t size) const
{
	if (!_cdata) return (token.isValid ()) ? -1 : 0;
	if (!token.isValid ()) return 1;
	if (!_len) return (size && token.getLength ()) ? -1 : 0;
	if (!size || !token.getLength ()) return 1;

	size_t alen = token.getLength ();
	size_t len = MIN (alen, _len);
	len = MIN (len, size);
	int cval = ::_strnicmp (_cdata, token.getCData (), len);
	if (cval) return cval;
	if (len >= size) return 0;
	if (_len < alen) return -1;
	if (_len > alen) return 1;
	return 0;
}

// Parsing

Token
Token::getLine (void) const
{
	// Test if empty
	if (isEmpty ()) return *this;
	// Drop sign
	unsigned char *p = (unsigned char *) _cdata;
	// Search end
	size_t e = 0;
	while ((e < _len) && ((p[e] >= 32) || (p[e] == 9))) e += 1;
	return Token (_cdata, e);
}

Token
Token::nextLine (const Token& line) const
{
	if (isEmpty ()) return *this;
	if (!line.isValid ()) return getLine ();
	// Drop sign
	unsigned char *p = (unsigned char *) _cdata;
	size_t s = (line.getCData () - _cdata) + line.getLength ();
	while ((s < _len) && (p[s] < 32) && (p[s] != 9)) s += 1;
	size_t e = s;
	while ((e < _len) && ((p[e] >= 32) || (p[e] == 9))) e += 1;
	return Token (_cdata, s, e);
}

int
Token::tokenize (Token *tokenz, int ntokenz, bool space_is_separator, bool multi, bool quote) const
{
	bool inquote = false;
	int idx = 0;
	// Drop sign
	unsigned char *p = (unsigned char *) _cdata;
	size_t s = 0;
	while ((s < _len) && (idx < ntokenz)) {
		// Find end of token
		size_t e = s;
		while ((e < _len) && ((p[e] > 32) || ((p[e] == 32) && (!space_is_separator || inquote)))) {
			if (quote && ((p[e] == '\'') || (p[e] == '\"'))) inquote = !inquote;
			e += 1;
		}
		// If the limit of tokens is reached go to the end of line
		if (idx == (ntokenz - 1)) {
			while ((e < _len) && ((p[e] >= 32) || (p[e] == 9))) e += 1;
		}
		tokenz[idx].set (_cdata, s, e);
		s = e + 1;
		if (multi) {
			// Skip whitespace
			while ((s < _len) && ((p[s] < 32) || ((p[s] == 32) && space_is_separator))) s += 1;
		}
		idx += 1;
	}
	return idx;
}

int
Token::tokenize (Token *tokenz, int ntokenz, const Token& separators, bool multi) const
{
	int idx = 0;
	size_t s = 0;
	while ((s < _len) && (idx < ntokenz)) {
		// Find end of token
		size_t e;
		if (idx != (ntokenz - 1)) {
			e = s;
			while (e < _len) {
				size_t i;
				for (i = 0; i < separators.getLength (); i++) {
					if (_cdata[e] == separators[i]) break;
				}
				if (i < separators.getLength ()) break;
				e += 1;
			}
		} else {
			e = _len;
		}
		tokenz[idx].set (_cdata, s, e);
		s = e + 1;
		if (multi) {
			while (s < _len) {
				size_t i;
				for (i = 0; i < separators.getLength (); i++) {
					if (_cdata[s] == separators[i]) break;
				}
				if (i >= separators.getLength ()) break;
				s += 1;
			}
		}
		idx += 1;
	}
	return idx;
}

Token
Token::stripStart (void) const
{
	size_t s = 0;
	while ((s < _len) && isspace (*((unsigned char *) _cdata + s))) s += 1;
	return Token (_cdata, s, _len);
}

Token
Token::stripStart (const Token& ws) const
{
	size_t s = 0;
	while (s < _len) {
		size_t i;
		for (i = 0; i < ws.getLength (); i++) {
			if (_cdata[s] == ws[i]) break;
		}
		if (i >= ws.getLength ()) break;
		s += 1;
	}
	return Token (_cdata, s, _len);
}

Token
Token::stripEnd (void) const
{
	size_t e = _len;
	while ((e > 0) && isspace (*((unsigned char *) _cdata + e - 1))) e -= 1;
	return Token (_cdata, e);
}

Token
Token::stripEnd (const Token& ws) const
{
	size_t e = _len;
	while (e > 0) {
		size_t i;
		for (i = 0; i < ws.getLength (); i++) {
			if (_cdata[e - 1] == ws[i]) break;
		}
		if (i >= ws.getLength ()) break;
		e -= 1;
	}
	return Token (_cdata, e);
}

Token
Token::strip (void) const
{
	size_t s = 0;
	size_t e = _len;
	while ((s < _len) && isspace (*((unsigned char *) _cdata + s))) s += 1;
	while ((e > s) && isspace (*((unsigned char *) _cdata + e - 1))) e -= 1;
	return Token (_cdata, s, e);
}

Token
Token::strip (const Token& ws) const
{
	Token t = stripStart (ws);
	return t.stripEnd (ws);
}

void
Token::skipUTF8Signature (void)
{
	unsigned char *p = (unsigned char *) _cdata;
	if ((p[0] == 0xef) && (p[1] == 0xbb) && (p[2] == 0xbf)) {
		_cdata += 3;
		_len -= 3;
	}
}

char *
Token::strconcat (const Token *tokens, int size, const Token& separator)
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
TokenLine::eof (void)
{
	return (getCData () >= (_master.getCData () + _master.getLength ()));
}

bool
TokenLine::forward (bool skipempty)
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

TokenLine&
TokenLine::operator= (const TokenLine& token)
{
	set (token);
	_master = token._master;
	return (*this);
}

TokenLine&
TokenLine::operator= (const Token& token)
{
	set (token);
	_master = token;
	return (*this);
}

TokenLine&
TokenLine::operator++ (void)
{
	Token n = _master.nextLine (*this);
	set (n);
	return *this;
}

TokenLine
TokenLine::operator++ (int v)
{
	TokenLine orig = *this;
	Token n = _master.nextLine (*this);
	set (n);
	return orig;
}

void
TokenLine::rewind (void)
{
	set (_master.getLine ());
}

bool
TokenLine::parse (const char *id, int& v0)
{
	Token tokenz[3];
	int ntokenz = tokenize (tokenz, 3, true, true, true);
	if (ntokenz != 2) return false;
	if (tokenz[0].strip () != id) return false;
	v0 = (int) tokenz[1].strip ();
	forward ();
	return true;
}

bool
TokenLine::parse (const char *id, int& v0, int& v1)
{
	Token tokenz[4];
	int ntokenz = tokenize (tokenz, 4, true, true, true);
	if (ntokenz != 3) return false;
	if (tokenz[0].strip () != id) return false;
	v0 = (int) tokenz[1].strip ();
	v1 = (int) tokenz[2].strip ();
	forward ();
	return true;
}

bool
TokenLine::parse (const char *id, float& v0)
{
	Token tokenz[3];
	int ntokenz = tokenize (tokenz, 3, true, true, true);
	if (ntokenz != 2) return false;
	if (tokenz[0].strip () != id) return false;
	v0 = (float) tokenz[1].strip ();
	forward ();
	return true;
}

bool
TokenLine::parse (const char *id, float& v0, float& v1)
{
	Token tokenz[4];
	int ntokenz = tokenize (tokenz, 4, true, true, true);
	if (ntokenz != 3) return false;
	if (tokenz[0].strip () != id) return false;
	v0 = (float) tokenz[1].strip ();
	v1 = (float) tokenz[2].strip ();
	forward ();
	return true;
}

TokenMMap::TokenMMap (const Token& filename, const Token& mapid)
: Token(), _filename(filename.strdup ())
{
	char b[32];
	if (mapid.isEmpty ()) {
		static int mapc = 0;
		sprintf (b, "EZD_mapid_%d", mapc++);
		_mapid = ::strdup (b);
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
Token::parseKeywordString (Token& lt, const char *keyword, Token& text, bool removequotes)
{
	Token st(lt.strip ());
	Token tokenz[2];
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
Token::parseKeywordUInt (Token& lt, const char *keyword, unsigned int& val)
{
	Token st(lt.strip ());
	Token tokenz[2];
	int ntokenz = st.tokenize (tokenz, 3, true, true, true);
	if (ntokenz != 2) return false;
	if (tokenz[0] != keyword) return false;
	val = (unsigned int) tokenz[1].strip ();
	return true;
}

bool
Token::parseKeywordInt (Token& lt, const char *keyword, int& val)
{
	Token st(lt.strip ());
	Token tokenz[2];
	int ntokenz = st.tokenize (tokenz, 3, true, true, true);
	if (ntokenz != 2) return false;
	if (tokenz[0] != keyword) return false;
	val = (int) tokenz[1].strip ();
	return true;
}

} // Namespace Arikkei
