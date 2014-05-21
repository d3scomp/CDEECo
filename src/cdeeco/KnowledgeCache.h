/**
 * KnowledgeCache.h
 *
 *  Created on: 5. 5. 2014
 *      Author: Vladimír Matěna
 */

#ifndef KNOWLEDGECACHE_H_
#define KNOWLEDGECACHE_H_

#include <assert.h>

#include "Console.h"

#include "KnowledgeFragment.h"

/** Interface to store knowledge in cache */
class KnowledgeStorage {
public:
	virtual ~KnowledgeStorage() {}

	// TODO: Pass reference to the fragment?
	virtual void storeFragment(const KnowledgeFragment fragment) = 0;
};

/** Interface to retrieve knowledge from cache */
template<typename KNOWLEDGE>
class KnowledgeLibrary {

};

/** Typed cache for storing knowledge */
template<ComponentType TYPE, typename KNOWLEDGE, size_t SIZE>
class KnowledgeCache: public KnowledgeStorage, public KnowledgeLibrary<KNOWLEDGE> {
	typedef uint32_t Timestamp;

public:
	KnowledgeCache() {
		// Erase cache
		memset(&cache, 0, sizeof(CacheRecord) * SIZE);
	}

	virtual ~KnowledgeCache() {}

	void storeFragment(const KnowledgeFragment fragment) {
		if(fragment.type != TYPE)
			return; // Not our knowledge type

		Console::log("Storing fragment in cache");

		// TODO: Locking?

		// Try to update knowledge and check for oldest knowledge
		size_t oldest = 0;
		for(size_t i = 0; i < SIZE; ++i) {
			// Store data is id matches
			if(cache[i].id == fragment.id) {
				updateCache(i, fragment);
				return;
			}

			// Update oldest
			if(cache[oldest].timestamp > cache[i].timestamp)
				oldest = i;
		}

		// Replace oldest knowledge
		writeCache(oldest, fragment);
	}

private:
	struct CacheRecord {
		ComponentId id;
		Timestamp timestamp;
		KNOWLEDGE data;
		KNOWLEDGE availability;
		bool complete;
	};

	CacheRecord cache[SIZE];

	void updateCache(size_t index, const KnowledgeFragment fragment) {
		assert(fragment.size + fragment.offset <= sizeof(KNOWLEDGE));

		// Set knowledge data
		memcpy(((char*) &cache[index].data) + fragment.offset, fragment.data, fragment.size);

		// Update availability
		memset(((char*) &cache[index].availability) + fragment.offset, 0xff, fragment.size);

		// Check whenever the knowledge is complete
		bool complete = true;
		for(size_t i = 0; i < sizeof(KNOWLEDGE); ++i)
			if(((char*) &cache[index])[i] != 0xff)
				complete = false;
		if(complete)
			cache[index].complete = true;

		// Set last updated time-stamp
		cache[index].timestamp = 0; // TODO: Set time-stamp
	}

	void writeCache(size_t index, const KnowledgeFragment fragment) {
		cache[index].id = fragment.id;
		memset(&cache[index].availability, 0, sizeof(KNOWLEDGE));
		cache[index].complete = false;
		updateCache(index, fragment);
	}
};

#endif // KNOWLEDGECACHE_H_
