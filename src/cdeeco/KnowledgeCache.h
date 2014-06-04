/**
 * KnowledgeCache.h
 *
 *  Created on: 5. 5. 2014
 *      Author: Vladimír Matěna
 */

#ifndef KNOWLEDGECACHE_H_
#define KNOWLEDGECACHE_H_

#include <iterator>
#include <array>

#include "Console.h"
#include "FreeRTOSMutex.h"
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
public:
	typedef uint32_t Timestamp;
	struct CacheRecord {
		KnowledgeFragment::Id id;
		Timestamp timestamp;
		KNOWLEDGE knowledge;
		KNOWLEDGE availability;
		bool complete;
	};

	class Iterator: std::iterator<std::input_iterator_tag, CacheRecord> {
	public:
		Iterator(KnowledgeLibrary<KNOWLEDGE> &library, size_t index): library(library), index(index) {
		}

		Iterator operator ++() {
			index++;
			return *this;
		}

		CacheRecord operator *() {
			library.cacheAccess.lock();
			CacheRecord record = library.cache[index];
			library.cacheAccess.unlock();
			return record;
		}

		bool operator !=(const Iterator &other) {
			return index != other.index;
		}

	private:
		KnowledgeLibrary<KNOWLEDGE> &library;
		size_t index;
	};

	KnowledgeLibrary(CacheRecord *cache, size_t size): cache(cache), cacheSize(size) {
	}

	virtual ~KnowledgeLibrary() {
	}

	Iterator begin() {
		return Iterator(*this, 0);
	}

	Iterator end() {
		return Iterator(*this, cacheSize);
	}

protected:
	CacheRecord *cache;
	size_t cacheSize;
	FreeRTOSMutex cacheAccess;
};

/** Typed cache for storing knowledge */
template<KnowledgeFragment::Type TYPE, typename KNOWLEDGE, size_t SIZE>
class KnowledgeCache: public KnowledgeStorage, public KnowledgeLibrary<KNOWLEDGE> {
public:
	KnowledgeCache(): KnowledgeLibrary<KNOWLEDGE>(cache.data(), cache.size()) {
		// Erase cache
		memset(&cache, 0, sizeof(typename KnowledgeLibrary<KNOWLEDGE>::CacheRecord) * SIZE);
	}

	virtual ~KnowledgeCache() {}

	void storeFragment(const KnowledgeFragment fragment) {
		if(fragment.type != TYPE)
			return; // Not our knowledge type

		Console::log(">>> Storing fragment in cache\n");

		this->cacheAccess.lock();

		// Try to update knowledge and check for oldest knowledge
		size_t oldest = 0;
		for(size_t i = 0; i < SIZE; ++i) {
			// Store data is id matches
			if(cache[i].id == fragment.id) {
				updateCache(i, fragment);
				this->cacheAccess.unlock();
				return;
			}

			// Update oldest
			if(cache[oldest].timestamp > cache[i].timestamp)
				oldest = i;
		}

		// Replace oldest knowledge
		writeCache(oldest, fragment);

		this->cacheAccess.unlock();
	}

private:
	std::array<typename KnowledgeLibrary<KNOWLEDGE>::CacheRecord, SIZE> cache;

	void updateCache(size_t index, const KnowledgeFragment fragment) {
		assert_param(fragment.size + fragment.offset <= sizeof(KNOWLEDGE));

		// Set knowledge data
		memcpy(((char*) &cache[index].knowledge) + fragment.offset, fragment.data, fragment.size);

		// Update availability
		memset(((char*) &cache[index].availability) + fragment.offset, 0xff, fragment.size);

		// Check whenever the knowledge is complete
		bool complete = true;
		for(size_t i = 0; i < sizeof(KNOWLEDGE); ++i)
			if(((char*) &cache[index].availability)[i] != 0xff)
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
