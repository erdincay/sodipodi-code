#ifndef __ARIKKEI_CACHE_TEMPLATE_H__
#define __ARIKKEI_CACHE_TEMPLATE_H__

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

#include <libarikkei/arikkei-cache.h>

namespace Arikkei {

template <typename K, typename V>
class Cache {
private:
	ArikkeiCache cache;
	// Forbidden
	Cache (const Cache& cache) { assert (false); }
	Cache& operator= (const Cache& cache) { assert (false); return *this; }
public:
	Cache (unsigned int size, unsigned int (* key_hash) (K), unsigned int (* key_equal) (K, K), K (* key_dup) (const K), void (* key_free) (K), void (* object_free) (V)) {
		arikkei_cache_setup_full (&cache, size, (unsigned int (*) (const void *)) key_hash, (unsigned int (*) (const void *, const void *)) key_equal,
			(void *(*) (const void *)) key_dup, (void (*) (void *)) key_free, (void (*) (const void *)) object_free);
	}
	void insert (K key, V object, unsigned int size) { arikkei_cache_insert (&cache, (const void *) key, (void *) object, size); }
	void remove (K key) { arikkei_cache_remove (&cache, (const void *) key); }

	V lookup (K key) { return (V) arikkei_cache_lookup (&cache, (const void *) key); }
	V lookup_notouch (K key) { return (V) arikkei_cache_lookup_notouch (&cache, (const void *) key); }
};

} // Namespace Arikkei

#endif





