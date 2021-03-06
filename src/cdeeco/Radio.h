/**
 * \ingroup cdeeco
 * @file Radio.h
 *
 * CDEECo++ unified radio interface
 *
 * \date 27. 5. 2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 */

#ifndef RADIO_H
#define RADIO_H

#include <array>

#include "Receiver.h"
#include "Broadcaster.h"
#include "Console.h"
#include "wrappers/FreeRTOSSemaphore.h"
#include "wrappers/FreeRTOSTask.h"

namespace CDEECO {
	/**
	 * Common radio interface
	 *
	 * This abstract class shall be implemented by user in order to provide radio to the CDEECo++ system.
	 *
	 * \ingroup cdeeco
	 */
	class Radio: Broadcaster {
	public:
		/**
		 * Set receiver for packets
		 *
		 * @param receiver Pointer to the instance of receiver
		 */
		void setReceiver(Receiver *receiver);

		/**
		 * Broadcast knowledge fragment
		 *
		 * To be implemented by user
		 */
		virtual void broadcastFragment(const KnowledgeFragment fragment) = 0;

	protected:
		/**
		 * Process knowledge fragment
		 *
		 * Knowledge fragment is processed by receiver previously set by setReceiver method call.
		 */
		void receiveFragment(const KnowledgeFragment fragment, const uint8_t lqi);

	private:
		/**
		 * Pointer to receiver instance
		 *
		 * When fragment is received this instance is used to process the knowledge fragment received.
		 */
		Receiver *receiver = NULL;
	};
}

#endif // RADIO_H
