/*
 * System.cpp
 *
 *  Created on: 24. 6. 2014
 *      Author: Vladimír Matěna
 */

#include "System.h"

namespace CDEECO {
	System::System() :
			rebroadcast(*this), radio(*this) {
		// Erase caches
		memset(&caches, 0, sizeof(caches));

		console.log(">>> System constructor\n");

		// Init console input
		console.setFragmentReceiver(this);
	}

	void System::receiveFragment(const KnowledgeFragment fragment, uint8_t lqi) {
		processFragment(fragment, lqi);
	}

	void System::broadcastFragment(KnowledgeFragment fragment) {
		console.print(Debug, ">>>>>>>>> Sending knowledge fragment:\n");
		console.logFragment(fragment);
		radio.broadcastFragment(fragment);
	}

	void System::processFragment(const KnowledgeFragment fragment, uint8_t lqi) {
		console.print(Debug, ">>>>>>>>> Processing knowledge fragment:\n");
		console.logFragment(fragment);

		// Store fragment in rebroadcast storage
		rebroadcast.storeFragment(fragment, lqi);

		storeFragment(fragment);
	}

	void System::storeFragment(const KnowledgeFragment fragment) {
		// Try to store fragment in one of the caches
		for(size_t i = 0; i < caches.size() && caches[i]; ++i)
			caches[i]->storeFragment(fragment);
	}

	void System::registerCache(KnowledgeStorage *cache) {
		assert_param(caches[caches.size() - 1] == NULL);

		for(size_t i = 0; i < CACHES; ++i)
			if(caches[i] == NULL) {
				caches[i] = cache;
				return;
			}

		console.print(Error, ">>>> OUT OF CACHE STORAGE <<<<\n");
		assert_param(false);
	}
}
