/*
 * Receiver.h
 *
 *  Created on: 27. 5. 2014
 *      Author: Vladimír Matěna
 */
#ifndef RECEIVER_H
#define RECEIVER_H

#include "KnowledgeFragment.h"

namespace CDEECO {
	/**
	 * Interface to knowledge receiver
	 */
	class Receiver {
	public:
		/**
		 * Receiver knowledge fragment
		 *
		 * @param fragment Knowledge fragment to broadcast
		 */
		virtual void receiveFragment(const KnowledgeFragment fragment, uint8_t lqi) = 0;
		virtual ~Receiver() {
		}
	};
};

#endif // RECEIVER_H
