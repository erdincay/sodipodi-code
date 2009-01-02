#define __ARIKKEI_CACHE_C__

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
#include <assert.h>

#ifdef WIN32
/* Disable VS2008 nagging */
#define strdup _strdup
#endif

#include <libarikkei/arikkei-dict.h>

#include "arikkei-cache.h"

struct _ArikkeiCacheEntry {
	ArikkeiCacheEntry *prev;
	ArikkeiCacheEntry *next;
	void *key;
	void *object;
	unsigned int size;
};

static unsigned int
arikkei_string_hash (const void *data)
{
	const unsigned char *p;
	unsigned int hval;
	p = data;
	hval = *p;
	if (hval) {
		for (p += 1; *p; p++) hval = (hval << 5) - hval + *p;
	}
	return hval;
}

static unsigned int
arikkei_string_equal (const void *l, const void *r)
{
	return !strcmp (l, r);
}

static void *
arikkei_string_dup (const void *str)
{
	return strdup ((const char *) str);
}

static void
arikkei_string_free (void *str)
{
	free (str);
}

static unsigned int
arikkei_pointer_hash (const void *data)
{
	unsigned int hval, p;
	hval = 0;
	p = (unsigned int) ((const char *) data - (const char *) 0);
	while (p) {
		hval ^= p;
		p /= 17;
	}
	return hval;
}

static unsigned int
arikkei_pointer_equal (const void *l, const void *r)
{
	return l == r;
}

static unsigned int
arikkei_int_hash (const void *data)
{
	unsigned int hval, p;
	hval = 0;
	p = (unsigned int) ((const char *) data - (const char *) 0);
	while (p) {
		hval ^= p;
		p /= 17;
	}
	return hval;
}

static unsigned int
arikkei_int_equal (const void *l, const void *r)
{
	return (unsigned int) ((const char *) l - (const char *) 0) == (unsigned int) ((const char *) l - (const char *) 0);
}

void arikkei_cache_setup_full (ArikkeiCache *cache, unsigned int size,
			      unsigned int (* key_hash) (const void *),
			      unsigned int (* key_equal) (const void *, const void *),
				  void *(* key_dup) (const void *key),
				  void (* key_free) (void *key),
				  void (* object_free) (const void *object))
{
	arikkei_dict_setup_full (&cache->dict, 313, key_hash, key_equal);
	cache->maxsize = size;
	cache->currentsize = 0;
	cache->entries = NULL;
	cache->last = NULL;
	cache->key_dup = key_dup;
	cache->key_free = key_free;
	cache->object_free = object_free;
}

void
arikkei_cache_setup_string (ArikkeiCache *cache, unsigned int size, void (* object_free) (const void *object))
{
	arikkei_cache_setup_full (cache, size, arikkei_string_hash, arikkei_string_equal, arikkei_string_dup, arikkei_string_free, object_free);
}

void
arikkei_cache_setup_pointer (ArikkeiCache *cache, unsigned int size, void (* object_free) (const void *object))
{
	arikkei_cache_setup_full (cache, size, arikkei_pointer_hash, arikkei_pointer_equal, NULL, NULL, object_free);
}

void
arikkei_cache_setup_int (ArikkeiCache *cache, unsigned int size, void (* object_free) (const void *object))
{
	arikkei_cache_setup_full (cache, size, arikkei_int_hash, arikkei_int_equal, NULL, NULL, object_free);
}

void
arikkei_cache_release (ArikkeiCache *cache)
{
	arikkei_dict_release (&cache->dict);
	while (cache->entries) {
		ArikkeiCacheEntry *e = cache->entries;
		cache->entries = cache->entries->next;
		if (cache->key_free) cache->key_free (e->key);
		if (cache->object_free) cache->object_free (e->object);
		free (e);
	}
}

static void
arikkei_cache_remove_entry (ArikkeiCache *cache, ArikkeiCacheEntry *entry)
{
	if (cache->key_free) cache->key_free (entry->key);
	if (cache->object_free) cache->object_free (entry->object);
	cache->currentsize -= entry->size;
	/* Detach entry */
	if (entry->prev) {
		entry->prev->next = entry->next;
	} else {
		cache->entries = entry->next;
	}
	if (entry->next) {
		entry->next->prev = entry->prev;
	} else {
		cache->last = entry->prev;
	}
	free (entry);
}

static void
arikkei_cache_ensure_space (ArikkeiCache *cache, unsigned int requested)
{
	while (requested > (cache->maxsize - cache->currentsize)) {
		assert (cache->last);
		arikkei_cache_remove_entry (cache, cache->last);
	}
}

void
arikkei_cache_insert (ArikkeiCache *cache, const void *key, void *object, unsigned int size)
{
	ArikkeiCacheEntry *e;
	if (size > cache->maxsize) return;
	e = (ArikkeiCacheEntry *) arikkei_dict_lookup (&cache->dict, key);
	if (e) {
		/* Replace object */
		if (cache->object_free) cache->object_free (e->object);
		cache->currentsize -= e->size;
		/* Detach entry */
		if (e->prev) {
			e->prev->next = e->next;
		} else {
			cache->entries = e->next;
		}
		if (e->next) {
			e->next->prev = e->prev;
		} else {
			cache->last = e->prev;
		}
	} else {
		e = (ArikkeiCacheEntry *) malloc (sizeof (ArikkeiCacheEntry));
	}
	/* Ensure we have enough space */
	arikkei_cache_ensure_space (cache, size);
	/* Set up entry */
	e->key = (cache->key_dup) ? cache->key_dup (key) : (void *) key;
	e->object = object;
	e->size = size;
	/* Attach entry */
	e->next = cache->entries;
	e->prev = NULL;
	cache->entries = e;
}

void
arikkei_cache_remove (ArikkeiCache *cache, const void *key)
{
	ArikkeiCacheEntry *e = (ArikkeiCacheEntry *) arikkei_dict_lookup (&cache->dict, key);
	if (!e) return;
	arikkei_cache_remove_entry (cache, e);
}

const void *
arikkei_cache_lookup (ArikkeiCache *cache, const void *key)
{
	ArikkeiCacheEntry *e;
	e = (ArikkeiCacheEntry *) arikkei_dict_lookup (&cache->dict, key);
	if (e) {
		/* Touch it */
		if (e->prev) {
			e->prev->next = e->next;
		} else {
			cache->entries = e->next;
		}
		if (e->next) {
			e->next->prev = e->prev;
		} else {
			cache->last = e->prev;
		}
		e->prev = NULL;
		e->next = cache->entries;
		cache->entries = e;
	}
	return (e) ? e->object : NULL;
}

const void *
arikkei_cache_lookup_notouch (ArikkeiCache *cache, const void *key)
{
	ArikkeiCacheEntry *e;
	e = (ArikkeiCacheEntry *) arikkei_dict_lookup (&cache->dict, key);
	return (e) ? e->object : NULL;
}

