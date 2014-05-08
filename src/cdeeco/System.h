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

		// Print knowledge fragment
		const size_t bufLen = 256;
		char buffer[bufLen];
		int written = sprintf(buffer, "Broadcast Fragment:\nType:%x\nId:%x\nSize:%x\nOffset:%x", fragment.type,
				fragment.id, fragment.size, fragment.offset);
		for(size_t i = 0; i < fragment.size; ++i) {
			written += sprintf(buffer + written, "%x");
			if(i % 8 == 0)
				sprintf(buffer + written, "\n");
		}
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
