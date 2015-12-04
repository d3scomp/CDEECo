/**
 * \ingroup cdeeco
 * @file RebroadcastStorage.h
 *
 * Class implementing CDEECo++ knowledge fragment rebroadcast
 *
 * \date 10.7.2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 */
#ifndef REBROADCAST_STORAGE_H
#define REBROADCAST_STORAGE_H

#include <array>
#include <random>

#include "KnowledgeFragment.h"
#include "Broadcaster.h"
#include "wrappers/FreeRTOSTask.h"
#include "wrappers/FreeRTOSMutex.h"
#include "CDEECo.h"

namespace CDEECO {
	/**
	 * Rebroadcast storage
	 *
	 * This class template is responsible for store and rebroadcast of received knowledge fragments.
	 *
	 * @tparam SIZE Number of slots in cache. Each knowledge fragment takes one slot.
	 *
	 * \ingroup cdeeco
	 */
	template<size_t SIZE>
	class RebroadcastStorage: FreeRTOSTask {
	private:
		/// Cache record time-stamp type
		typedef long Timestamp;
		/// Cache record index type
		typedef size_t Index;
		/// Period in between rebroadcast checks
		static const Timestamp PERIOD = 50;

		/// Rebroadcast record
		struct ReboadcastRecord {
			bool used;
			Timestamp received;
			Timestamp rebroadcast;
			KnowledgeFragment fragment;
		};

	public:
		/// Stochastic TTL settings. Drop every Nth packet.
		static const auto STOCHASTIC_DROP_EVERY = 2;
		/// Do not rebroadcast packets received with lqi higher than this
		static const auto NO_REBROADCAST_LQI = 115;

		/**
		 * Construct rebroadcast storage
		 *
		 * @param broadcaster Instance of broadcaster used to rebroadcast packets
		 */
		RebroadcastStorage(Broadcaster &broadcaster) :
				broadcaster(broadcaster) {
			memset(&records, 0, sizeof(records));
		}

		/**
		 * Store fragment in rebroadcast cache
		 *
		 * @param fragment Received knowledge fragment
		 * @param lqi Link quality for received knowledge fragment
		 */
		void storeFragment(const KnowledgeFragment fragment, uint8_t lqi) {
			// Stochastic Time to live implementation
			if(gen() % STOCHASTIC_DROP_EVERY)
				return;

			// Do not rebroadcast strong signals at all
			if(lqi > NO_REBROADCAST_LQI)
				return;

			// Max rebroadcast interval
			const Timestamp RebroadcastInterval = 5000;

			// Calculate rebroadcast delay
			const Timestamp rebroadcastDelay = 255 * RebroadcastInterval / (255 - lqi);

			recordsMutex.lock();
			Index free = getFree();
			records[free].used = true;
			records[free].received = FreeRTOSTask::getTickCount();
			records[free].rebroadcast = records[free].received + rebroadcastDelay;
			records[free].fragment = fragment;
			recordsMutex.unlock();
		}

		/**
		 * Get index of free slot
		 *
		 * This can force rebroadcast of another record if there is no slot.
		 */
		Index getFree() {
			for(Index i = 0; i < records.size(); ++i)
				if(!records[i].used)
					return i;
			return pushRecord();
		}

		/**
		 * Force rebroadcast oldest record
		 *
		 * This frees one slot.
		 */
		Index pushRecord() {
			console.print(Error, ">>>>>>>> Out of rebroadcast storage - force rebroadcast of oldest record\n");
			Index oldest = 0;
			for(Index i = 0; i < records.size(); ++i)
				if(records[i].received < records[oldest].received)
					oldest = i;
			rebroadcast(oldest);
			return oldest;
		}

		/**
		 * Reboradcst record
		 *
		 * @param index Index of record to rebroadcast
		 */
		void rebroadcast(Index index) {
			console.print(Debug, ">>> Rebroadcasting fragment\n");
			broadcaster.broadcastFragment(records[index].fragment);
			records[index].used = false;
		}

		/**
		 * Check records and rebroadcast them every PERIOD
		 */
		void run() {
			while(1) {
				FreeRTOSTask::mDelay(PERIOD);

				const Timestamp now = FreeRTOSTask::getTickCount();
				recordsMutex.lock();
				for(Index i = 0; i < records.size(); ++i)
					if(records[i].used && records[i].rebroadcast <= now)
						rebroadcast(i);
				recordsMutex.unlock();
			}
		}

	private:
		/// Instance of broadcaster used for rebroadcast
		Broadcaster &broadcaster;
		/// random engine used for stochastic TTL
		std::default_random_engine gen;

		/// Mutex for records access protection
		FreeRTOSMutex recordsMutex;
		/// Array of cache records
		std::array<RebroadcastStorage::ReboadcastRecord, SIZE> records;
	};
}

#endif // REBROADCAST_STORAGE_H
