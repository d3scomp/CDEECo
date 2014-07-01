/*
 * Radio.h
 *
 *  Created on: 27. 5. 2014
 *      Author: Vladimír Matěna
 */

#ifndef RADIO_H_
#define RADIO_H_

#include "FreeRTOS.h"
#include "task.h"

#include <array>

#include "main.h"
#include "Receiver.h"
#include "Broadcaster.h"
#include "Console.h"
#include "wrappers/FreeRTOSSemaphore.h"
#include "wrappers/FreeRTOSTask.h"

namespace CDEECO {
	class Radio: Broadcaster {
	public:
		void setReceiver(Receiver *receiver);
		virtual void broadcastFragment(const KnowledgeFragment fragment) = 0;

	protected:
		void receiveFragment(const KnowledgeFragment fragment, const uint8_t lqi);

	private:
		Receiver *receiver = NULL;
	};
}

#endif // RADIO_H_
