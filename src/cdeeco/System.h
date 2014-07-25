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

namespace CDEECO {
	/**
	 * CDEECo++ system class template
	 *
	 * This provides unified access to the radio interface.
	 *
	 * @tparam CACHES Number of cache slots in the system object
	 * @tparam REBROADCAST_SIZE Size of rebroadcast cache
	 */
	template <size_t CACHES = 3, size_t REBROADCAST_SIZE = 32>
	class System: public Broadcaster, Receiver {
	public:
		/**
		 * System constructor
		 *
		 * @param radio Reference to Radio implementation
		 */
		System(Radio &radio) :
				rebroadcast(*this), radio(radio) {
			// Erase caches
			memset(&caches, 0, sizeof(caches));

			console.print(Info, ">>> System constructor\n");

			radio.setReceiver(this);

			// Initialize console input
			console.setFragmentReceiver(this);
		}

		/**
		 * Receive listener
		 *
		 * @param fragment Received knowledge fragment
		 * @param lqi Link quality for received knowledge fragment
		 */
		void receiveFragment(const KnowledgeFragment fragment, uint8_t lqi) {
			processFragment(fragment, lqi);
		}

		/**
		 * Broadcast knowledge fragment
		 *
		 * @param fragment Knowledge fragment to broadcast
		 */
		void broadcastFragment(KnowledgeFragment fragment) {
			console.print(Debug, ">>>>>>>>> Sending knowledge fragment:\n");
			console.logFragment(fragment);
			radio.broadcastFragment(fragment);

			/* TODO: This is local loop-back for registering fragment from local components. Doing this makes no
			 *  sense in real application. */
			storeFragment(fragment);
		}

		/**
		 * Register knowledge cache
		 *
		 * This adds cache to fixed array of caches. When there is out of space
		 * in the array the application is stopped on assert. Please set system
		 * CACHES template argument properly.
		 *
		 * @param cache Knowledge cache to be added
		 */
		void registerCache(KnowledgeStorage *cache) {
			assert_param(caches[caches.size() - 1] == NULL);

			for(size_t i = 0; i < CACHES; ++i)
				if(caches[i] == NULL) {
					caches[i] = cache;
					return;
				}

			console.print(Error, ">>>> OUT OF CACHE STORAGE <<<<\n");
			assert_param(false);
		}

	private:
		/**
		 * Process process received fragment
		 *
		 * @param fragment Received knowledge fragment
		 * @param lqi Link quality for received knowledge fragment
		 */
		void processFragment(const KnowledgeFragment fragment, uint8_t lqi) {
			console.print(Debug, ">>>>>>>>> Processing knowledge fragment:\n");
			console.logFragment(fragment);

			// Store fragment in rebroadcast storage
			rebroadcast.storeFragment(fragment, lqi);

			storeFragment(fragment);
		}

		/**
		 * Store fragment in knowledge cache
		 *
		 * @param fragment Knowledge fragment to store
		 */
		void storeFragment(const KnowledgeFragment fragment) {
			// Try to store fragment in one of the caches
			for(size_t i = 0; i < caches.size() && caches[i]; ++i)
				caches[i]->storeFragment(fragment);
		}

		/// Array with assigned caches
		std::array<KnowledgeStorage*, CACHES> caches;

		/// Rebroadcast storage instance
		RebroadcastStorage<REBROADCAST_SIZE> rebroadcast;

		/// Radio interface instance
		Radio &radio;
	};
}

#endif // SYSTEM_H
