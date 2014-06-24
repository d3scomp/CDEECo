/*
 * Radio.cpp
 *
 *  Created on: 27. 5. 2014
 *      Author: Vladimír Matěna
 */

#include "Radio.h"

namespace CDEECO {
	void Radio::setReceiver(Receiver *receiver) {
		this->receiver = receiver;
	}
}
