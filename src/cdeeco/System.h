/*
 * System.h
 *
 *  Created on: 7. 5. 2014
 *      Author: Vladimír Matěna
 */

#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdio.h>
#include <array>
#include <algorithm>

#include "RebroadcastStorage.h"
#include "KnowledgeCache.h"
#include "Broadcaster.h"
#include "Receiver.h"
#include "Radio.h"

template<size_t SIZE>
class RebroadcastStorage;

namespace CDEECO {
	class System: public Broadcaster, Receiver {
	public:
		System();

		/** Receive listener */
		void receiveFragment(const KnowledgeFragment fragment, uint8_t lqi);

		/** Broadcast knowledge fragment */
		void broadcastFragment(KnowledgeFragment fragment);

		/** Register knowledge cache */
		void registerCache(KnowledgeStorage *cache);

	private:
		/** Process process received fragment */
		void processFragment(const KnowledgeFragment fragment, uint8_t lqi);

		/** Store fragment in knowledge cache */
		void storeFragment(const KnowledgeFragment fragment);

		static const size_t CACHES = 3;
		static const size_t REBROADCAST_SIZE = 32;

		std::array<KnowledgeStorage*, CACHES> caches;
		RebroadcastStorage<REBROADCAST_SIZE> rebroadcast;

		Radio radio;
	};
}

#endif /* SYSTEM_H */
