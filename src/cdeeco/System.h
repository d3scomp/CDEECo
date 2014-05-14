/*
 * System.h
 *
 *  Created on: 7. 5. 2014
 *      Author: Vladimír Matěna
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <stdio.h>

#include "console.h"
#include "KnowledgeCache.h"

typedef uint32_t Timestamp;

class System {
public:
	System() {
		// Erase caches
		memset(&caches, 0, sizeof(caches));

		// Init console input
		Console::log("Setting receive listener");
		Console::serial.setRecvListener(staticReceiveListener, this);
		Console::log("Enabling receive events");
		Console::serial.setPriority(2, 2);
		Console::serial.enableRecvEvents();
	}

	/** Static receive listener */
	static void staticReceiveListener(void *data) {
		static_cast<System*>(data)->receiveListener();
	}

	/** Receive listener */
	void receiveListener() {
		char recv = Console::serial.recv();
		if(recv == 'X') {
			Console::log(">>>> Recieved <<<<");
		}
	}

	/** Broadcast knowledge fragment */
	void broadcastFragment(KnowledgeFragment fragment) {
		// TODO: Enqueue fragment for broadcasting

		// Print knowledge fragment
		const size_t bufLen = 256;
		char buffer[bufLen];
		size_t written = sprintf(buffer, "Broadcast Fragment:\n\tType:%lx\n\tId:%lx\n\tSize:%x\n\tOffset:%x", fragment.type,
				fragment.id, fragment.size, fragment.offset);
		for(size_t i = 0; i < fragment.size; ++i) {
			// Hex output formating
			if(i % 16 == 0) {
				buffer[written++] = '\n';
				buffer[written++] = '\t';
			}
			if(i % 2 == 0)
				buffer[written++] = ' ';

			// Print single byte
			written += sprintf(buffer + written, "%02x", fragment.data[i]);

			// Stop printing when running out of buffer
			if(written > bufLen - 32) {
				sprintf(buffer + written, "...");
				break;
			}
		}

		Console::log(buffer);
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
