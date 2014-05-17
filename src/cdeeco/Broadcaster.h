#ifndef BROADCASTER_H
#define BROADCASTER_H

#include "KnowledgeFragment.h"

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
	virtual void broadcastFragment(const KnowledgeFragment fragment) = 0;
	virtual ~Broadcaster() {
	}
};

#endif // BROADCASTER_H
