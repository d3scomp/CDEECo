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

#include "Console.h"
#include "KnowledgeCache.h"
#include "RebroadcastStorage.h"

class System {
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
		static_cast<System*>(data)->receiveListener(fragment);
	}

	/** Receive listener */
	void receiveListener(const KnowledgeFragment fragment) {
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

private:
	static const size_t CACHES = 3;
	static const size_t REBROADCAST_SIZE = 8;

	std::array<KnowledgeCache*, CACHES> caches;
	RebroadcastStorage<REBROADCAST_SIZE> rebroadcast;
};

#endif /* SYSTEM_H_ */
