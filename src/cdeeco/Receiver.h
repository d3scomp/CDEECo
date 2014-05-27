/*
 * Receiver.h
 *
 *  Created on: 27. 5. 2014
 *      Author: Vladimír Matěna
 */
#ifndef RECEIVER_H
#define RECEIVER_H

#include "KnowledgeFragment.h"

/**
 * Interface to knowledge reciever
 */
class Receiver {
public:
	/**
	 * Receiver knowledge fragment
	 *
	 * @param fragment Knowledge fragment to broadcast
	 */
	virtual void receiveFragment(const KnowledgeFragment fragment) = 0;
	virtual ~Receiver() {
	}
};

#endif // RECEIVER_H
