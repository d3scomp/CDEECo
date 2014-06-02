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
#include "FragmentBuffer.h"
#include "FreeRTOSSemaphore.h"

template<size_t SIZE>
class RebroadcastStorage;

class System: Broadcaster, Receiver, FreeRTOSTask {
public:
	System() :
			rebroadcast(*this), radio(*this), rxSem(MAX_TX_WAITING) {
		// Erase caches
		memset(&caches, 0, sizeof(caches));

		Console::log(">>> System constructor");

		// Init console input
		Console::setFragmentReceiver(this);
	}

	/** Receive listener */
	void receiveFragment(const KnowledgeFragment fragment) {
		Console::log(">>>>>>>>> Received knowledge fragment:");
		Console::logFragment(fragment);

		vTaskSuspendAll();
		rxBuffer.put(fragment);
		rxSem.give();
		xTaskResumeAll();
	}

	/** Broadcast knowledge fragment */
	void broadcastFragment(KnowledgeFragment fragment) {
		Console::log(">>>>>>>>> Sending knowledge fragment:");
		Console::logFragment(fragment);
		radio.broadcastFragment(fragment);
	}

	/** Process process received fragment */
	void processFragment(const KnowledgeFragment fragment) {
		Console::log(">>>>>>>>> Processing knowledge fragment");

		// Store fragment in rebroadcast storage
		rebroadcast.storeFragment(fragment);

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

		Console::log(">>>> OUT OF CACHE STORAGE <<<<");
		assert_param(false);
	}

private:
	static const size_t CACHES = 3;
	static const size_t REBROADCAST_SIZE = 8;
	static const size_t RXBUFF_SIZE = 5;

	std::array<KnowledgeStorage*, CACHES> caches;
	RebroadcastStorage<REBROADCAST_SIZE> rebroadcast;

	Radio radio;
	FragmentBuffer<RXBUFF_SIZE> rxBuffer;
	FreeRTOSSemaphore rxSem;
	static const size_t MAX_TX_WAITING = 100;

	void run() {
		Console::log(">>>> System fragment processing task start");

		while(true) {
			rxSem.take();

			vTaskSuspendAll();
			KnowledgeFragment fragment;
			bool ok = rxBuffer.get(fragment);
			xTaskResumeAll();

			if(ok)
				processFragment(fragment);
		}
	}
};

#endif /* SYSTEM_H_ */
