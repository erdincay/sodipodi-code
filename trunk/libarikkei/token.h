#ifndef __ARIKKEI_TOKEN_TEMPLATE_H__
#define __ARIKKEI_TOKEN_TEMPLATE_H__

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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <libarikkei/arikkei-strlib.h>

#ifndef MIN
#define MIN(l,r) (((r) < (l)) ? (r) : (l))
#endif

// C++ tokens

namespace Arikkei {

template <typename T> class Token {
private:
	const T *_cdata;
	size_t _len;
	T *_pdata;
	// Private helpers
	void freePData (void) {
		if (_pdata) free (_pdata);
		_pdata = NULL;
	};
	unsigned int _U (size_t idx) const;
	bool _S (size_t idx) const;
	static size_t _tlen (const T *cdata);
	static int _tncmp (const T *lhs, const T *rhs, size_t len);
	static int _tnicmp (const T *lhs, const T *rhs, size_t len);
	static T *_tdup (const T *cdata);
public:
	// Constructors
	Token (void) : _cdata(NULL), _len(0), _pdata(NULL) {};
	Token (const Token& token) : _cdata(token._cdata), _len(token._len), _pdata(NULL) {};
	Token (const T *cdata) : _cdata(cdata), _len(cdata ? _tlen(cdata) : 0), _pdata(NULL) {};
	Token (const T *cdata, size_t length) : _cdata(cdata), _len(length), _pdata(NULL) {};
	Token (const T *cdata, size_t start, size_t end) : _cdata(cdata + start), _len(end - start), _pdata(NULL) {};
	Token (const Token& token, size_t start, size_t end) : _cdata(token._cdata + start), _len(end - start), _pdata(NULL) {};
	// Destructor
	~Token (void) { if (_pdata) free (_pdata); }
	// Access
	const T *getCData (void) const { return _cdata; }
	size_t getLength (void) const { return _len; }
	bool isValid (void) const { return _cdata != NULL; }
	bool isEmpty (void) const { return !_cdata || (_len == 0); }
	const T *getPData (void);
	// Methods
	void set (const Token& token) {
		if ((_cdata != token.getCData ()) || (_len != token.getLength ())) {
			freePData ();
			_cdata = token.getCData ();
			_len = token.getLength ();
		}
	}
	void set (const T *cdata, size_t start, size_t end) {
		freePData ();
		_cdata = cdata + start;
		_len = end - start;
	}
	void set (size_t start, size_t end) {
		freePData ();
		_cdata = _cdata + start;
		_len = end - start;
	}
	bool getBool (void) const {
		if (isEmpty ()) return false;
		if (!stricmp ("yes")) return true;
		if (!stricmp ("no")) return false;
		if (!stricmp ("true")) return true;
		if (!stricmp ("false")) return false;
		return getInt () != 0;
	}
	int getInt (void) const;
	unsigned int getUInt (void) const;
	float getFloat (void) const;
	double getDouble (void) const;
	// Member operators
	Token& operator= (const Token& token) { set (token); return *this; }
	operator int (void) const { return getInt (); }
	operator unsigned int (void) const { return getUInt (); }
	operator float (void) const { return getFloat (); }
	operator double (void) const { return getDouble (); }
	operator const T * (void) { return getPData (); }
	const T operator[] (int pos) const { return _cdata[pos]; }
	const T operator[] (size_t pos) const { return _cdata[pos]; }
	// Friend operators
	friend bool operator== (const Token& lhs, const Token& rhs) {
		if (!lhs.isValid ()) return false;
		if (!rhs.isValid ()) return false;
		if (!lhs.isEmpty ()) {
			if (rhs.isEmpty ()) return false;
			size_t len = lhs.getLength ();
			if (len != rhs.getLength ()) return false;
			for (size_t i = 0; i < len; i++) if (lhs[i] != rhs[i]) return false;
		} else {
			if (!rhs.isEmpty ()) return false;
		}
		return true;
	}
	friend bool operator== (const Token& lhs, const T *rhs) { return lhs.isValid () && !lhs.strcmp (rhs); }
	friend bool operator== (const T *lhs, const Token& rhs) { return rhs.isValid () && !rhs.strcmp (lhs); }
	friend bool operator!= (const Token& lhs, const Token& rhs) { return !(lhs == rhs); }
	friend bool operator!= (const Token& lhs, const T *rhs) { return !(lhs == rhs); }
	friend bool operator!= (const T *lhs, const Token& rhs) { return !(lhs == rhs); }
	// Semistandard string methods
	// Invalid is NULL, empty is ""
	T *strdup (void) const {
		if (!_cdata) return NULL;
		char *b = (T *) malloc ((_len + 1) * sizeof (T));
		if (_len) memcpy (b, _cdata, _len * sizeof (T));
		b[_len] = 0;
		return b;
	}
	size_t strcpy (T *str) const {
		if (_len) memcpy (str, _cdata, _len * sizeof (T));
		str[_len] = 0;
		return _len;
	}
	size_t strncpy (T *str, size_t size) const {
		if (size < 1) return 0;
		size_t len = MIN (_len, size - 1);
		if (len) memcpy (str, _cdata, len * sizeof (T));
		str[len] = 0;
		return len;
	}
	int strcmp (const T *str) const {
		if (!_cdata) return (str) ? -1 : 0;
		if (!str) return 1;
		int cval = strncmp (str, _len);
		if (cval) return cval;
		if (_len < _tlen (str)) return -1;
		return 0;
	}
	int strcmp (const Token& token) const {
		if (!_cdata) return (token.isValid ()) ? -1 : 0;
		if (!token.isValid ()) return 1;
		size_t len = MIN (_len, token.getLength ());
		int cval = _tncmp (_cdata, token.getCData (), len);
		if (cval) return cval;
		if (len < token.getLength ()) return -1;
		if (len < _len) return 1;
		return 0;
	}
	int strncmp (const T *str, size_t size) const {
		if (!_cdata) return (str) ? -1 : 0;
		if (!str) return 1;
		if (!_len) return (size && *str) ? -1 : 0;
		if (!size || !*str) return 1;
		size_t alen = strlen (str);
		size_t len = MIN (alen, _len);
		len = MIN (len, size);
		int cval = _tncmp (_cdata, str, len);
		if (cval) return cval;
		if (len >= size) return 0;
		if (_len < alen) return -1;
		if (_len > alen) return 1;
		return 0;
	}
	int strncmp (const Token& token, size_t size) const {
		if (!_cdata) return (token.isValid ()) ? -1 : 0;
		if (!token.isValid ()) return 1;
		if (!_len) return (size && token.getLength ()) ? -1 : 0;
		if (!size || !token.getLength ()) return 1;
		size_t alen = token.getLength ();
		size_t len = MIN (alen, _len);
		len = MIN (len, size);
		int cval = _tncmp (_cdata, token.getCData (), len);
		if (cval) return cval;
		if (len >= size) return 0;
		if (_len < alen) return -1;
		if (_len > alen) return 1;
		return 0;
	}
	int stricmp (const T *str) const {
		if (!_cdata) return (str) ? -1 : 0;
		if (!str) return 1;
		int cval = strnicmp (str, _len);
		if (cval) return cval;
		if (_len < _tlen (str)) return -1;
		return 0;
	}
	int stricmp (const Token& token) const {
		if (!_cdata) return (token.isValid ()) ? -1 : 0;
		if (!token.isValid ()) return 1;
		size_t len = MIN (_len, token.getLength ());
		int cval = _tnicmp (_cdata, token.getCData (), len);
		if (cval) return cval;
		if (len < token.getLength ()) return -1;
		if (len < _len) return 1;
		return 0;
	}
	int strnicmp (const T *str, size_t size) const {
		if (!_cdata) return (str) ? -1 : 0;
		if (!str) return 1;
		if (!_len) return (size && *str) ? -1 : 0;
		if (!size || !*str) return 1;
		size_t alen = strlen (str);
		size_t len = MIN (alen, _len);
		len = MIN (len, size);
		int cval = _tnicmp (_cdata, str, len);
		if (cval) return cval;
		if (len >= size) return 0;
		if (_len < alen) return -1;
		if (_len > alen) return 1;
		return 0;
	}
	int strnicmp (const Token& token, size_t size) const {
		if (!_cdata) return (token.isValid ()) ? -1 : 0;
		if (!token.isValid ()) return 1;
		if (!_len) return (size && token.getLength ()) ? -1 : 0;
		if (!size || !token.getLength ()) return 1;
		size_t alen = token.getLength ();
		size_t len = MIN (alen, _len);
		len = MIN (len, size);
		int cval = _tnicmp (_cdata, token.getCData (), len);
		if (cval) return cval;
		if (len >= size) return 0;
		if (_len < alen) return -1;
		if (_len > alen) return 1;
		return 0;
	}

	Token getLine (void) const;
	Token nextLine (const Token& line) const;

	Token nextToken (size_t& cpos, const Token* itokens, int nitokens)
	{
		// Skip whitespace
		size_t s = cpos;
		while ((s < _len) && (_U(s) <= 32)) s += 1;
		if (s >= _len) {
			cpos = _len;
			return Token();
		}
		// Test individual tokens
		size_t r = _len - s;
		for (int i = 0; i < nitokens; i++) {
			size_t l = itokens[i].getLength ();
			if ((r >= l) && !itokens[i].strncmp (_cdata + s, l)) {
				cpos = s + l;
				return Token (_cdata, s, s + l);
			}
		}
		// Get next token
		size_t e = s;
		while ((e < _len) && (_U(e) > 32)) e += 1;
		cpos = e;
		return Token(_cdata, s, e);
	}

	int tokenize (Token *tokenz, int ntokenz, bool space_is_separator = false, bool multi = false, bool quote = false) const {
		bool inquote = false;
		int idx = 0;
		// Drop sign
		size_t s = 0;
		while ((s < _len) && (idx < ntokenz)) {
			// Find end of token
			size_t e = s;
			while ((e < _len) && ((_U(e) > 32) || ((_U(e) == 32) && (!space_is_separator || inquote)))) {
				if (quote && ((_U(e) == '\'') || (_U(e) == '\"'))) inquote = !inquote;
				e += 1;
			}
			// If the limit of tokens is reached go to the end of line
			if (idx == (ntokenz - 1)) {
				while ((e < _len) && ((_U(e) >= 32) || (_U(e) == 9))) e += 1;
			}
			tokenz[idx].set (_cdata, s, e);
			s = e + 1;
			if (multi) {
				// Skip whitespace
				while ((s < _len) && ((_U(s) < 32) || ((_U(s) == 32) && space_is_separator))) s += 1;
			}
			idx += 1;
		}
		return idx;
	}
	int tokenize (Token *tokenz, int ntokenz, const Token& separators, bool multi = false) const {
		int idx = 0;
		size_t s = 0;
		while ((s <= _len) && (idx < ntokenz)) {
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
			// e -> first separator   - found separator
			// e -> len               - end of token
			tokenz[idx].set (_cdata, s, e);
			e += 1;
			if (multi) {
				while (e < _len) {
					size_t i;
					for (i = 0; i < separators.getLength (); i++) {
						if (_cdata[s] == separators[i]) break;
					}
					if (i >= separators.getLength ()) break;
					s += 1;
				}
			}
			// s -> first separator + 1   - found separator
			// e -> len + 1               - end of token
			s = e;
			idx += 1;
		}
		return idx;
	}
	Token stripStart (void) const {
		size_t s = 0;
		while ((s < _len) && _S(s)) s += 1;
		return Token (_cdata, s, _len);
	}
	Token stripStart (const Token& wspace) const {
		size_t s = 0;
		while (s < _len) {
			size_t i;
			for (i = 0; i < wspace.getLength (); i++) {
				if (_cdata[s] == wspace[i]) break;
			}
			if (i >= wspace.getLength ()) break;
			s += 1;
		}
		return Token (_cdata, s, _len);
	}
	Token stripEnd (void) const {
		size_t e = _len;
		while ((e > 0) && _S(e - 1)) e -= 1;
		return Token (_cdata, e);
	}
	Token stripEnd (const Token& wspace) const {
		size_t e = _len;
		while (e > 0) {
			size_t i;
			for (i = 0; i < wspace.getLength (); i++) {
				if (_cdata[e - 1] == wspace[i]) break;
			}
			if (i >= wspace.getLength ()) break;
			e -= 1;
		}
		return Token (_cdata, e);
	}
	Token strip (void) const {
		size_t s = 0;
		size_t e = _len;
		while ((s < _len) && _S(s)) s += 1;
		while ((e > s) && _S(e - 1)) e -= 1;
		return Token (_cdata, s, e);
	}
	Token strip (const Token& wspace) const {
		Token t = stripStart (wspace);
		return t.stripEnd (wspace);
	}

	void skipUTF8Signature (void);

	// Static methods
	static char *strconcat (const Token *tokens, int size, const Token& separator);

	static bool parseKeywordString (Token& lt, const char *keyword, Token& text, bool removequotes);
	static bool parseKeywordUInt (Token& lt, const char *keyword, unsigned int& val);
	static bool parseKeywordInt (Token& lt, const char *keyword, int& val);
};

// Specialization

template <>
inline unsigned int Token<char>::_U (size_t idx) const { return *((unsigned char *) _cdata + idx); }
template <>
inline unsigned int Token<unsigned short>::_U (size_t idx) const { return _cdata[idx]; }
template <>
inline bool Token<char>::_S (size_t idx) const { return isspace (*((unsigned char *) _cdata + idx)) != 0; }
template <>
inline bool Token<unsigned short>::_S (size_t idx) const { return (_cdata[idx] < 256) && isspace (_cdata[idx]); }

template <>
inline size_t
Token<char>::_tlen (const char *cdata)
{
	return ::strlen (cdata);
}

template <>
inline size_t
Token<unsigned short>::_tlen (const unsigned short *cdata)
{
	return arikkei_ucs2_strlen (cdata);
}

template <>
inline int
Token<char>::_tncmp (const char *lhs, const char *rhs, size_t len)
{
	return ::strncmp (lhs, rhs, len);
}

template <>
inline int
Token<char>::_tnicmp (const char *lhs, const char *rhs, size_t len)
{
#ifdef WIN32
	return ::_strnicmp (lhs, rhs, len);
#else
	return ::strncasecmp (lhs, rhs, len);
	// for (size_t i = 0; i < len; i++) {
	// 	if (tolower(lhs[i] < tolower(rhs[i]))) return -1;
	// 	if (tolower(lhs[i] > tolower(rhs[i]))) return 1;
	// }
	// return 0;
#endif
}

typedef Token<char> CharToken;
typedef Token<char> CToken;
typedef Token<char> TokenChar;
typedef Token<unsigned short> USToken;
typedef Token<unsigned short> TokenUS;

// Line iterator

template <typename T> class TokenLine : public Token<T> {
private:
	Token<T> _master;
public:
	TokenLine (void) : Token<T>(), _master() {}
	TokenLine (const Token<T>& master, size_t start = 0) : Token<T>(master.getLine ()) { _master = master; }

	bool eof (void) const { return (this->getCData () >= (_master.getCData () + _master.getLength ())); }

	bool forward (bool skipempty = false);

	TokenLine& operator= (const TokenLine& token) {	set (token); _master = token._master; return (*this); }
	TokenLine& operator= (const Token<T>& token) { set (token); _master = token; return (*this); }
	TokenLine& operator++ (void) { Token<T> n = _master.nextLine (*this); set (n); return *this; }
	TokenLine operator++ (int v) { TokenLine orig = *this; Token<T> n = _master.nextLine (*this); set (n); return orig; }

	void rewind (void) { set (_master.getLine ()); }
};

typedef TokenLine<char> TokenLineChar;

// TokenMMap

class TokenMMap : public TokenChar {
private:
	char *_filename;
	char *_mapid;
	const char *_data;
	size_t _size;
	TokenMMap (const TokenMMap& token) {};
public:
	TokenMMap (const CToken& filename, const CToken& mapid = CToken());
	~TokenMMap (void);
	void skipSignature (void);
};

typedef TokenMMap TokenMMapChar;

} // Namespace Arikkei

#endif





