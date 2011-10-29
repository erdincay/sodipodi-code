#ifndef __ARIKKEI_DICT_TEMPLATE_H__
#define __ARIKKEI_DICT_TEMPLATE_H__

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

#include <assert.h>

#include <libarikkei/arikkei-dict.h>

namespace Arikkei {

template <typename R>
inline R p2a (const void *P) { return (R) P; }
template <>
inline unsigned int p2a (const void *P) { return (unsigned int) ((char *) P - (char *) 0); }
template <>
inline int p2a (const void *P) { return (int) ((char *) P - (char *) 0); }

template <typename K, typename V>
class Dict {
private:
	// Forbidden
	Dict (const Dict& dict) { assert (false); }
	Dict& operator= (const Dict& dict) { assert (false); return *this; }
public:
	ArikkeiDict dict;

	Dict (unsigned int hashsize, unsigned int (* hash) (K), unsigned int (* equal) (K, K)) {
		// arikkei_dict_setup_full (&_dict, hashsize, unsigned int (* hash) (const void *), unsigned int (* equal) (const void *, const void *));
		arikkei_dict_setup_full (&dict, hashsize, (unsigned int (*) (const void *)) hash, (unsigned int (*) (const void *, const void *)) equal);
	}
	~Dict (void) {
		arikkei_dict_release (&dict);
	}
	bool exists (K key) { return arikkei_dict_exists (&dict, (const void *) key) != 0; }
	V lookup (K key) { return p2a<V> (arikkei_dict_lookup (&dict, (const void *) key)); }
	void insert (K key, V val) { arikkei_dict_insert (&dict, (const void *) key, (const void *) val); }
	void remove (K key) { arikkei_dict_remove (&dict, (const void *) key); }
	void forall (unsigned int (* callback) (K, V, void *), void *data) { arikkei_dict_forall (&dict, (unsigned int (*) (const void *, const void *, void *)) callback); }
};

template <typename V>
class Dict<const char *, V> {
private:
	// Forbidden
	Dict (const Dict& dict) { assert (false); }
	Dict& operator= (const Dict& dict) { assert (false); return *this; }
public:
	ArikkeiDict dict;

	Dict (unsigned int hashsize) {
		arikkei_dict_setup_string (&dict, hashsize);
	}
	~Dict (void) {
		arikkei_dict_release (&dict);
	}
	bool exists (const char *key) { return arikkei_dict_exists (&dict, (const void *) key) != 0; }
	V lookup (const char *key) { return p2a<V> (arikkei_dict_lookup (&dict, (const void *) key)); }
	void insert (const char *key, V val) { arikkei_dict_insert (&dict, (const void *) key, (const void *) val); }
	void remove (const char *key) { arikkei_dict_remove (&dict, (const void *) key); }
	void forall (unsigned int (* callback) (const char *, V, void *), void *data) { arikkei_dict_forall (&dict, (unsigned int (*) (const void *, const void *, void *)) callback); }
};

template <typename V>
class Dict<int, V> {
private:
	// Forbidden
	Dict (const Dict& dict) { assert (false); }
	Dict& operator= (const Dict& dict) { assert (false); return *this; }
public:
	ArikkeiDict dict;

	Dict (unsigned int hashsize) {
		arikkei_dict_setup_int (&dict, hashsize);
	}
	~Dict (void) {
		arikkei_dict_release (&dict);
	}
	bool exists (int key) { return arikkei_dict_exists (&dict, (const void *) key) != 0; }
	V lookup (int key) { return p2a<V> (arikkei_dict_lookup (&dict, (const void *) key)); }
	void insert (int key, const V val) { arikkei_dict_insert (&dict, (const void *) key, (const void *) val); }
	void remove (int key) { arikkei_dict_remove (&dict, (const void *) key); }
};

} // Namespace Arikkei

#endif





