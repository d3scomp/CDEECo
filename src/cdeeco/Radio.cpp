/**
 * \ingroup cdeeco
 * @file Radio.cpp
 *
 * CDEECo++ radio wrapper implementation
 *
 * \date 27. 5. 2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 */

#include "Radio.h"

namespace CDEECO {
	void Radio::setReceiver(Receiver *receiver) {
		this->receiver = receiver;
	}

	void Radio::receiveFragment(const KnowledgeFragment fragment, const uint8_t lqi) {
		if(receiver)
			receiver->receiveFragment(fragment, lqi);
	}
}
