/**
 * KnowledgeCache.h
 *
 *  Created on: 5. 5. 2014
 *      Author: Vladimír Matěna
 *
 * @file
 * This file includes implementation of the knowledge caching system. It contains KnowledgeCache template which is
 * accompanioned by two interfaces. KnowledgeStorage for storing knowledge fragments and KnowledgeLibrary for
 * listing records.
 *
 *
 * Unlike rebroadcast cache the knowledge cache is a bit more complicated. The actual
 * storage is implemented by class KnowledgeCache which is also template. It
 * takes three template arguments. The first one specifies component type magic number.
 * The second one is the knowledge type. The last one is size of the cache. Each type
 * of knowledge is handled by custom instance of KnowledgeCache class template.
 *
 * The cache is also formed by fixed array of records. Each record holds knowledge data,
 * mask that show valid regions of the data, time-stamp and complete flag. Each time
 * new knowledge fragment of matching cache type and record id is processed it's data
 * are added to the record and the availability mask is updated. When the mask shown
 * that knowledge is complete the complete flag is set to true. When the cache
 * is full the oldest record is replaced.
 *
 * The KnowledgeCache class inherits from two helper classes. The first one is the
 * KnowledgeStorage class which is an interface for storing fragments in the cache. It
 * is not a template thus it can be used to store array of caches in the System and
 * store fragments received in them. The second one is the KnowledgeLibrary template.
 * It has the only template argument which specifies knowledge type. KnowledgeLibrary
 * can iterate over the complete records in the cache. Thus it allows ensembles to
 * query complete cache records for membership and possible knowledge exchange. The
 * library interface simplifies cache handling as the access to the library is
 * possible without knowing cache size and knowledge magic, but still the library
 * has the knowledge type so it can return properly typed data.
 */

#ifndef KNOWLEDGECACHE_H
#define KNOWLEDGECACHE_H

#include <iterator>
#include <array>

#include "main.h"
#include "wrappers/FreeRTOSMutex.h"
#include "KnowledgeFragment.h"

namespace CDEECO {
	/**
	 * Interface to store knowledge in the knowledge cache
	 *
	 * It is used to hide template arguments of the KnowledgeCache template, thus simplifies keeping references to
	 * caches in the System class.
	 */
	class KnowledgeStorage {
	public:
		virtual ~KnowledgeStorage() {
		}

		/**
		 * Store knowledge fragment in cache
		 *
		 * @param fragment Knowledge fragment to store
		 */
		virtual void storeFragment(const KnowledgeFragment fragment) = 0;
	};

	/**
	 * Interface to retrieve knowledge from knowledge cache
	 *
	 * it is used to hide template arguments of the KnowledgeCache template, thus simplifies keeping references to
	 * caches in the Ensemble related classes. It also provides provides iterator to iterate over records in cache.
	 */
	template<typename KNOWLEDGE>
	class KnowledgeLibrary {
	public:
		/// Time-stamps used by caching system
		typedef uint32_t Timestamp;
		/**
		 * Cache record structure for keeping cached knowledge
		 */
		struct CacheRecord {
			/** Id of the component which produced the knowledge */
			CDEECO::Id id;
			/** Time when the knowledge was received */
			Timestamp timestamp;
			/** Knowledge data combined from received fragments */
			KNOWLEDGE knowledge;
			/** Map of knowledge data availability. 0x00 means not available 0xff means that the byte is valid. */
			KNOWLEDGE availability;
			/**
			 * Flag that indicated whenever all the data are available.
			 * True means that the knowledge field contains valid knowledge.
			 */
			bool complete;
		};

		/**
		 * Iterator to the knowledge library
		 *
		 * Allows iterating over the records in the cache.
		 */
		class Iterator: std::iterator<std::input_iterator_tag, CacheRecord> {
		public:
			/**
			 * Iterator constructor
			 *
			 * @param library Knowledge library to iterate over
			 * @param index Start index in the library
			 */
			Iterator(KnowledgeLibrary<KNOWLEDGE> &library, size_t index) :
					library(library), index(index) {
			}

			/**
			 * Move iterator to next library record
			 *
			 * @return Returns the modified iterator
			 */
			Iterator operator ++() {
				index++;
				return *this;
			}

			/**
			 * Retrieve current cache record
			 *
			 * @return Cache record at the current iterator position
			 */
			CacheRecord operator *() {
				library.cacheAccess.lock();
				CacheRecord record = library.cache[index];
				library.cacheAccess.unlock();
				return record;
			}

			/**
			 * Check whenever library iterators are unequal
			 *
			 * @param other The other iterator to compare
			 * @return True when unequal, false otherwise
			 */
			bool operator !=(const Iterator &other) {
				return index != other.index;
			}

		private:
			/// Library this iterator iterates over
			KnowledgeLibrary<KNOWLEDGE> &library;
			/// Current position in the library
			size_t index;
		};

		/**
		 * Knowledge library constructor
		 *
		 * @param cache Pointer to cache record array this library will be attached to
		 * @param size Size of the cache record array
		 */
		KnowledgeLibrary(CacheRecord *cache, size_t size) :
				cache(cache), cacheSize(size) {
		}

		virtual ~KnowledgeLibrary() {
		}

		/**
		 * Get start iterator for the library
		 *
		 * @return Iterator set to first element in the library
		 */
		Iterator begin() {
			return Iterator(*this, 0);
		}

		/**
		 * Get end iterator for the library
		 *
		 * @return Iterator set behind the last element in the library
		 */
		Iterator end() {
			return Iterator(*this, cacheSize);
		}

	protected:
		/// Pointer to the first element in the cache this library belongs to
		CacheRecord *cache;
		/// Size of the cache this library belongs to
		size_t cacheSize;
		/**
		 * Mutex for accessing cache records
		 * It is placed here in order to be visible in the KnowledgeCache class too.
		 */
		FreeRTOSMutex cacheAccess;
	};

	/**
	 * Typed knowledge cache for storing knowledge
	 *
	 * @tparam TYPE Magic number of component producing knowledge of interest
	 * @tparam KNOWLEDGE Knowledge type for the cache
	 * @tparam SIZE Size of the cache
	 *
	 * This template provides knowledge cache implementation. it is intended to be used
	 * via interfaces for writing KnowledgeStorage and reading KnowledgeLibrary.
	 *
	 */
	template<Type TYPE, typename KNOWLEDGE, size_t SIZE>
	class KnowledgeCache: public KnowledgeStorage, public KnowledgeLibrary<KNOWLEDGE> {
	public:
		/**
		 * Knowledge cache constructor
		 *
		 * Responsible for cache initialization and passing cache data pointer and
		 * size to base class.
		 */
		KnowledgeCache() :
				KnowledgeLibrary<KNOWLEDGE>(cache.data(), cache.size()) {
			// Erase cache
			memset(&cache, 0, sizeof(typename KnowledgeLibrary<KNOWLEDGE>::CacheRecord) * SIZE);
		}

		virtual ~KnowledgeCache() {
		}

		void storeFragment(const KnowledgeFragment fragment) {
			if(fragment.type != TYPE)
				return; // Not our knowledge type

			console.print(Debug, ">>> Storing fragment in cache\n");

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
		/// Static array of cache records
		std::array<typename KnowledgeLibrary<KNOWLEDGE>::CacheRecord, SIZE> cache;

		/**
		 * Update cache record
		 *
		 * This patches the cached knowledge with the new data and updates availability mask.
		 * The complete flag is also updated when needed.
		 *
		 * @param index Index of cache record to update
		 * @param fragment Knowledge fragment to update the record with
		 */
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
			cache[index].timestamp = xTaskGetTickCount();
		}

		/**
		 * Overwrite cache record with new fragment
		 *
		 * Initializes availability and complete to 0 and calls update to store the new fragment.
		 *
		 * @param index Record index to overwrite
		 * @param fragment Initial fragment to store, used for id and initial update
		 */
		void writeCache(size_t index, const KnowledgeFragment fragment) {
			cache[index].id = fragment.id;
			memset(&cache[index].availability, 0, sizeof(KNOWLEDGE));
			cache[index].complete = false;
			updateCache(index, fragment);
		}
	};
}

#endif // KNOWLEDGECACHE_H
