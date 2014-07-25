/**
 * \ingroup cdeeco
 * @file Broadcaster.h
 *
 * Interface for knowledge broadcasting access
 *
 * \date 7. 5. 2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 */

#ifndef BROADCASTER_H
#define BROADCASTER_H

#include "KnowledgeFragment.h"

/**
 * \ingroup cdeeco
 *
 * Core system CDEECo++ classes
 */
namespace CDEECO {
	/**
	 * Interface to broadcaster
	 *
	 * \ingroup cdeeco
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
