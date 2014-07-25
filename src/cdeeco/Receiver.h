/**
 * \ingroup cdeeco
 * @file Receiver.h
 *
 * Interface for receiving knowledge fragments
 *
 * \date 27. 5. 2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 */
#ifndef RECEIVER_H
#define RECEIVER_H

#include "KnowledgeFragment.h"

namespace CDEECO {
	/**
	 * Interface to knowledge receiver
	 *
	 * \ingroup cdeeco
	 */
	class Receiver {
	public:
		/**
		 * Receiver knowledge fragment
		 *
		 * @param fragment Knowledge fragment to broadcast
		 * @param lqi Link auality of the packet containing the fragment
		 */
		virtual void receiveFragment(const KnowledgeFragment fragment, uint8_t lqi) = 0;
		virtual ~Receiver() {
		}
	};
}

#endif // RECEIVER_H
