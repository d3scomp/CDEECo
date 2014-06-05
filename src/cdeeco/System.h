/*
 * System.h
 *
 *  Created on: 7. 5. 2014
 *      Author: Vladimír Matěna
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <stdio.h>
#include <array>
#include <algorithm>

#include "RebroadcastStorage.h"
#include "Console.h"
#include "KnowledgeCache.h"
#include "Broadcaster.h"
#include "Receiver.h"
#include "Radio.h"
#include "FreeRTOSTask.h"
#include "FreeRTOSSemaphore.h"

template<size_t SIZE>
class RebroadcastStorage;

namespace CDEECO {

	class System: Broadcaster, Receiver {
	public:
		System(KnowledgeFragment::Id id) :
				id(id), rebroadcast(*this), radio(*this) {
			// Erase caches
			memset(&caches, 0, sizeof(caches));

			Console::log(">>> System constructor\n");

			// Init console input
			Console::setFragmentReceiver(this);
		}

		/** Receive listener */
		void receiveFragment(const KnowledgeFragment fragment, uint8_t lqi) {
			// If fragment is not local process it
			if(fragment.id != id)
				processFragment(fragment, lqi);
		}

		/** Broadcast knowledge fragment */
		void broadcastFragment(KnowledgeFragment fragment) {
			Console::print(Info, ">>>>>>>>> Sending knowledge fragment:\n");
			Console::logFragment(fragment);
			radio.broadcastFragment(fragment);
		}

		/** Process process received fragment */
		void processFragment(const KnowledgeFragment fragment, uint8_t lqi) {
			Console::print(Info, ">>>>>>>>> Processing knowledge fragment:\n");
			Console::logFragment(fragment);

			// Store fragment in rebroadcast storage
			rebroadcast.storeFragment(fragment, lqi);

			storeFragment(fragment);
		}

		/** Store fragment in knowledge cache */
		void storeFragment(const KnowledgeFragment fragment) {
			// Try to store fragment in one of the caches
			for(size_t i = 0; i < caches.size() && caches[i]; ++i)
				caches[i]->storeFragment(fragment);
		}

		/** Register knowledge cache */
		void registerCache(KnowledgeStorage *cache) {
			assert_param(caches[caches.size() - 1] == NULL);

			for(size_t i = 0; i < CACHES; ++i)
				if(caches[i] == NULL) {
					caches[i] = cache;
					return;
				}

			Console::print(Error, ">>>> OUT OF CACHE STORAGE <<<<\n");
			assert_param(false);
		}

		KnowledgeFragment::Id getId() {
			return id;
		}

	private:
		// Used as Id of all system components
		const KnowledgeFragment::Id id;

		static const size_t CACHES = 3;
		static const size_t REBROADCAST_SIZE = 32;

		std::array<KnowledgeStorage*, CACHES> caches;
		RebroadcastStorage<REBROADCAST_SIZE> rebroadcast;

		Radio radio;
	};
}

#endif /* SYSTEM_H_ */
