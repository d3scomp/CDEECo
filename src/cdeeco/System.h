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

template<size_t SIZE>
class RebroadcastStorage;

class System: Broadcaster, Receiver {
public:
	System(): rebroadcast(*this) {
		// Erase caches
		memset(&caches, 0, sizeof(caches));

		// Init console input
		Console::log("Setting fragment listener");
		Console::setRecvFragmentListener(staticReceiveListener, this);
	}

	/** Static receive listener */
	static void staticReceiveListener(void *data, KnowledgeFragment fragment) {
		static_cast<System*>(data)->receiveFragment(fragment);
	}

	/** Receive listener */
	void receiveFragment(const KnowledgeFragment fragment) {
		Console::log(">>>>>>>>> Received knowledge fragment:");
		Console::logFragment(fragment);
		processFragment(fragment);
	}

	/** Broadcast knowledge fragment */
	void broadcastFragment(KnowledgeFragment fragment) {
		// TODO: Enqueue fragment for broadcasting
		Console::log(">>>>>>>>> Sending knowledge fragment:");
		Console::logFragment(fragment);
	}

	/** Process process received fragment */
	void processFragment(const KnowledgeFragment fragment) {
		// Store fragment in rebroadcast storage
		rebroadcast.storeFragment(fragment);

		// Try to store fragment in one of the caches
		for(size_t i = 0; i < caches.size() && caches[i]; ++i)
			caches[i]->storeFragment(fragment);
	}

	/** Register knowledge cache */
	void registerCache(KnowledgeStorage *cache) {
		assert(caches[caches.size() - 1] == NULL);

		for(size_t i = 0; i < CACHES; ++i)
			if(caches[i] == NULL) {
				caches[i] = cache;
				return;
			}

		Console::log(">>>> OUT OF CACHE STORAGE <<<<");
		assert(false);
	}

private:
	static const size_t CACHES = 3;
	static const size_t REBROADCAST_SIZE = 8;

	std::array<KnowledgeStorage*, CACHES> caches;
	RebroadcastStorage<REBROADCAST_SIZE> rebroadcast;
};

#endif /* SYSTEM_H_ */
