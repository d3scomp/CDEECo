/*
 * System.h
 *
 *  Created on: 7. 5. 2014
 *      Author: Vladimír Matěna
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "KnowledgeCache.h"

typedef uint32_t Timestamp;

template<size_t CACHES, size_t REBROADCAST_SIZE>
class System {
public:
	System() {
		// Erase caches
		memset(&caches, 0, sizeof(caches));
	}

	/** Broadcast knowledge fragment */
	void broadcastFragment(KnowledgeFragment fragment) {
		// TODO: Enqueue fragment for broadcasting
	}

	/** Process process received fragment */
	void processFragment(KnowledgeFragment fragment) {
		// TODO: Try to store fragment in one of the caches
		// TODO: Store fragment in rebroadcast storage
	}

private:
	KnowledgeCache *caches[CACHES];

	struct ReboadcastRecord {
		Timestamp recieved;
		Timestamp rebroadcast;
		KnowledgeFragment fragment;
	} rebroadcast[REBROADCAST_SIZE];
};

#endif /* SYSTEM_H_ */
