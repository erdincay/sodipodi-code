#ifndef __ARIKKEI_TOKEN_H__
#define __ARIKKEI_TOKEN_H__

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

#ifdef __cplusplus
extern "C" {
#endif

/*
 * As you can guess, it was C++ library intially
 * In plain C is is not half as nice, but still quite useful
 */

typedef struct _ArikkeiToken ArikkeiToken;

struct _ArikkeiToken {
	const unsigned char *cdata;
	size_t len;
};

ArikkeiToken *arikkei_token_set_from_string (ArikkeiToken *token, const unsigned char *cdata);
ArikkeiToken *arikkei_token_set_from_data (ArikkeiToken *token, const unsigned char *cdata, size_t start, size_t end);
ArikkeiToken *arikkei_token_set_from_token (ArikkeiToken *token, const ArikkeiToken *src);

#define arikkei_token_is_valid(t) ((t)->cdata != 0)
#define arikkei_token_is_empty(t) (!(t)->cdata || ((t)->len == 0))

unsigned int arikkei_token_is_equal (const ArikkeiToken *token, const ArikkeiToken *other);
unsigned int arikkei_token_is_equal_string (const ArikkeiToken *token, const unsigned char *str);

unsigned char *arikkei_token_strdup (const ArikkeiToken *token);
size_t arikkei_token_strcpy (const ArikkeiToken *token, unsigned char *b);
size_t arikkei_token_strncpy (const ArikkeiToken *token, unsigned char *b, size_t size);
int arikkei_token_strcmp (const ArikkeiToken *token, const unsigned char *str);
int arikkei_token_strncmp (const ArikkeiToken *token, const unsigned char *str, size_t size);

ArikkeiToken *arikkei_token_get_first_line (const ArikkeiToken *token, ArikkeiToken *dst);
ArikkeiToken *arikkei_token_get_line (const ArikkeiToken *token, ArikkeiToken *dst, size_t start);
ArikkeiToken *arikkei_token_next_line (const ArikkeiToken *token, ArikkeiToken *dst, const ArikkeiToken *line);
ArikkeiToken *arikkei_token_get_token (const ArikkeiToken *token, ArikkeiToken *dst, size_t start, unsigned int space_is_separator);
ArikkeiToken *arikkei_token_next_token (const ArikkeiToken *token, ArikkeiToken *dst, const ArikkeiToken *prev, unsigned int space_is_separator);

int arikkei_token_tokenize (ArikkeiToken *token, ArikkeiToken *tokens, int maxtokens, unsigned int space_is_separator, unsigned int multi);
int arikkei_token_tokenize_ws (ArikkeiToken *token, ArikkeiToken *tokens, int maxtokens, const unsigned char *ws, unsigned int multi);

ArikkeiToken *arikkei_token_strip_start (ArikkeiToken *token, ArikkeiToken *dst);
ArikkeiToken *arikkei_token_strip_start_ws (ArikkeiToken *token, ArikkeiToken *dst, const unsigned char *ws);
ArikkeiToken *arikkei_token_strip_end (ArikkeiToken *token, ArikkeiToken *dst);
ArikkeiToken *arikkei_token_strip_end_ws (ArikkeiToken *token, ArikkeiToken *dst, const unsigned char *ws);
ArikkeiToken *arikkei_token_strip (ArikkeiToken *token, ArikkeiToken *dst);
ArikkeiToken *arikkei_token_strip_ws (ArikkeiToken *token, ArikkeiToken *dst, const unsigned char *ws);

unsigned char *arikkei_token_strconcat (const ArikkeiToken *tokens, int size, const unsigned char *separator);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

// C++ tokens

#include <string.h>
#include <malloc.h>

namespace Arikkei {

class Token {
private:
	const char *_cdata;
	size_t _len;
	char *_pdata;
public:
	// Constructors
	Token (void) : _cdata(NULL), _len(0), _pdata(NULL) {};
	Token (const Token& token) : _cdata(token.getCData ()), _len(token.getLength ()), _pdata(NULL) {};
	Token (const char *cdata) : _cdata(cdata), _len(cdata ? ::strlen(cdata) : 0), _pdata(NULL) {};
	Token (const char *cdata, size_t length) : _cdata(cdata), _len(length), _pdata(NULL) {};
	Token (const char *cdata, size_t start, size_t end) : _cdata(cdata + start), _len(end - start), _pdata(NULL) {};
	Token (const Token& token, size_t start, size_t end) : _cdata(token.getCData () + start), _len(end - start), _pdata(NULL) {};
	// Destructor
	~Token (void) { if (_pdata) free (_pdata); }
	// Access
	const char *getCData (void) const { return _cdata; }
	size_t getLength (void) const { return _len; }
	bool isValid (void) const { return _cdata != NULL; }
	bool isEmpty (void) const { return !_cdata || (_len == 0); }
	// Methods
	void set (const Token& token);
	void set (const char *cdata, size_t start, size_t end);
	void set (size_t start, size_t end);
	bool getBool (void) const;
	// Member operators
	Token& operator= (const Token& token) { set (token); return *this; }
	operator int (void) const;
	operator unsigned int (void) const;
	operator float (void) const;
	operator double (void) const;
	operator const char * (void);
	const char operator[] (int pos) const { return _cdata[pos]; }
	const char operator[] (size_t pos) const { return _cdata[pos]; }
	// Friend operators
	friend bool operator== (const Token& lhs, const Token& rhs);
	friend bool operator== (const Token& lhs, const char *rhs) { return lhs.isValid () && !lhs.strcmp (rhs); }
	friend bool operator== (const char *lhs, const Token& rhs) { return rhs.isValid () && !rhs.strcmp (lhs); }
	friend bool operator!= (const Token& lhs, const Token& rhs) { return !(lhs == rhs); }
	friend bool operator!= (const Token& lhs, const char *rhs) { return !(lhs == rhs); }
	friend bool operator!= (const char *lhs, const Token& rhs) { return !(lhs == rhs); }

	// Semistandard string methods
	// Invalid is NULL, empty is ""
	char *strdup (void) const;
	size_t strcpy (char *str) const;
	size_t strncpy (char  *str, size_t size) const;
	int strcmp (const char *str) const;
	int strcmp (const Token& token) const;
	int strncmp (const char *str, size_t size) const;
	int strncmp (const Token& token, size_t size) const;
	int stricmp (const char *str) const;
	int stricmp (const Token& token) const;
	int strnicmp (const char *str, size_t size) const;
	int strnicmp (const Token& token, size_t size) const;

	Token getLine (void) const;
	Token nextLine (const Token& line) const;

	int tokenize (Token *tokenz, int ntokenz, bool space_is_separator = false, bool multi = false, bool quote = false) const;
	int tokenize (Token *tokenz, int ntokenz, const Token& separators, bool multi = false) const;

	Token stripStart (void) const;
	Token stripStart (const Token& wspace) const;
	Token stripEnd (void) const;
	Token stripEnd (const Token& wspace) const;
	Token strip (void) const;
	Token strip (const Token& wspace) const;

	void skipUTF8Signature (void);

	// Static methods
	static char *strconcat (const Token *tokens, int size, const Token& separator);

	static bool parseKeywordString (Token& lt, const char *keyword, Token& text, bool removequotes);
	static bool parseKeywordUInt (Token& lt, const char *keyword, unsigned int& val);
	static bool parseKeywordInt (Token& lt, const char *keyword, int& val);
};

// Line iterator

class TokenLine : public Token {
private:
	Token _master;
public:
	TokenLine (const Token& master, size_t start = 0) : Token (master.getLine ()) { _master = master; }

	bool eof (void);

	bool forward (bool skipempty = false);

	TokenLine& operator= (const TokenLine& token);
	TokenLine& operator= (const Token& token);
	TokenLine& operator++ (void);
	TokenLine operator++ (int v);

	void rewind (void);

	// Parsing
	bool parse (const char *id, int& v0);
	bool parse (const char *id, int& v0, int& v1);
	bool parse (const char *id, float& v0);
	bool parse (const char *id, float& v0, float& v1);
};

// TokenMMap

class TokenMMap : public Token {
private:
	char *_filename;
	char *_mapid;
	const char *_data;
	size_t _size;
	TokenMMap (const TokenMMap& token) {};
public:
	TokenMMap (const Token& filename, const Token& mapid = Token());
	~TokenMMap (void);
	void skipSignature (void);
};

} // Namespace Arikkei

#endif

#endif





