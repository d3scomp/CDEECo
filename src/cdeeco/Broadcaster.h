/*
 * Broadcaster.h
 *
 *  Created on: 7. 5. 2014
 *      Author: Vladimír Matěna
 */

#ifndef BROADCASTER_H
#define BROADCASTER_H

#include "KnowledgeFragment.h"

namespace CDEECO {
	/**
	 * Interface to broadcaster
	 */
	class Broadcaster {
		/**
		 * Broadcast knowledge fragment to the air
		 *
		 * @param fragment Knowledge fragment to broadcast
		 */
	public:
		/**
		 * Broadcast fragment
		 *
		 * @param fragment KnowledgeFragment to broadcast
		 */
		virtual void broadcastFragment(const KnowledgeFragment fragment) = 0;
	};
}

#endif // BROADCASTER_H
