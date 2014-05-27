/*
 * Radio.h
 *
 *  Created on: 27. 5. 2014
 *      Author: Vladimír Matěna
 */

#ifndef RADIO_H_
#define RADIO_H_

#include "Receiver.h"
#include "Broadcaster.h"

class Radio: Broadcaster {
public:
	Radio(Receiver &receiver): receiver(receiver) {
	}

	void broadcastFragment(const KnowledgeFragment fragment) {
		// TODO: Broadcast fragment
	}

private:
	Receiver &receiver;
};

#endif // RADIO_H_
