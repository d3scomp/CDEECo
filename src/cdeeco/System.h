/*
 * System.h
 *
 *  Created on: 7. 5. 2014
 *      Author: Vladimír Matěna
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <stdio.h>

#include "Console.h"
#include "KnowledgeCache.h"

typedef uint32_t Timestamp;

class System {
public:
	System() {
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
	void receiveListener(KnowledgeFragment fragment) {
		Console::log(">>>> Received knowledge fragment <<<<");
	}

	/** Broadcast knowledge fragment */
	void broadcastFragment(KnowledgeFragment fragment) {
		// TODO: Enqueue fragment for broadcasting

		Console::logFragment(fragment);
	}

	/** Process process received fragment */
	void processFragment(KnowledgeFragment fragment) {
		// TODO: Try to store fragment in one of the caches
		// TODO: Store fragment in rebroadcast storage
	}

private:
	static const size_t CACHES = 3;
	static const size_t REBROADCAST_SIZE = 8;

	KnowledgeCache *caches[CACHES];

	struct ReboadcastRecord {
		Timestamp recieved;
		Timestamp rebroadcast;
		KnowledgeFragment fragment;
	} rebroadcast[REBROADCAST_SIZE];
};

#endif /* SYSTEM_H_ */
