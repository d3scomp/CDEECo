/**
 * \ingroup example
 * @file MrfRadio.h
 *
 * Definition of the MRF24J40 adapter for CDEECo++ radio interface
 *
 * \date 24. 6. 2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 */

#ifndef MRFRADIO_H
#define MRFRADIO_H

#include "cdeeco/KnowledgeFragment.h"
#include "cdeeco/Knowledge.h"
#include "cdeeco/Radio.h"

#include "wrappers/FreeRTOSMutex.h"

/**
 * Simple radio interface provider implementation
 *
 * \ingroup example
 */
class MrfRadio: public CDEECO::Radio {
public:
	/**
	 * Radio constructor
	 *
	 * @param channel Channel to listen and broadcast on
	 * @param panId Network id
	 * @param sourceAddress Source address for broadcasted packets
	 */
	MrfRadio(uint8_t channel, uint16_t panId, uint16_t sourceAddress);

	/**
	 * Broadcast fragment
	 *
	 * @param fragment Fragment to broadcast
	 */
	void broadcastFragment(const CDEECO::KnowledgeFragment fragment);

private:
	/**
	 * Ultra simple ring buffer
	 *
	 * @tparam T Buffer content type
	 * @tparam SIZE Buffer size
	 */
	template<typename T, size_t SIZE>
	class RingBuffer {
	public:
		/**
		 * Put item into buffer
		 *
		 * This do not block and overwrites items in the full buffer
		 */
		void put(const T &item) {
			taskENTER_CRITICAL();
			data[start++ % SIZE] = item;
			taskEXIT_CRITICAL();
			sem.giveFromISR();
		}

		/**
		 * Retrieve item from buffer
		 *
		 * This may block if buffer is empty
		 *
		 * @return Retrieved item
		 */
		T get() {
			sem.take();
			taskENTER_CRITICAL();
			T ret = data[end++ % SIZE];
			taskEXIT_CRITICAL();
			return ret;
		}

	private:
		/// Buffer start position
		size_t start = 0;

		/// Buffer end position
		size_t end = 0;

		/// Buffer data array
		std::array<T, SIZE> data;

		/// Buffer load semaphore
		FreeRTOSSemaphore sem = FreeRTOSSemaphore(10000, 0);
	};

	/**
	 * Radio packet wrapper
	 */
	struct Packet {
		/**
		 * Packed data represented as union in order to be accessible as plain data and
		 * KnowledgeFragment at the same time.
		 */
		union {
			/// Plain data packet access
			uint8_t data[CDEECO::KnowledgeFragment::MAX_PACKET_SIZE];
			/// Access data as KnowledgeFragment
			CDEECO::KnowledgeFragment fragment;
		} data;

		/// Packet size
		uint8_t size;
		/// Source network id
		uint8_t srcPanId[2];
		/// Source address
		uint8_t srcSAddr[2];
		/// Frame check sequence
		uint8_t fcs[2];
		/// Link quality
		uint8_t lqi;
		/// Received signal strength indication
		uint8_t rssi;
		/// Whenever packet is valid
		bool valid;
	};

	/// Whenever the radio is transmitting
	bool txInProgress = false;
	/// Transmit ring buffer
	RingBuffer<CDEECO::KnowledgeFragment, 5> txBuffer;
	/// Receive ring buffer
	RingBuffer<Packet, 5> rxBuffer;
	/// Transmit control semaphore
	FreeRTOSSemaphore txSem = FreeRTOSSemaphore(1000, 1);

	/**
	 * Receiving thread
	 */
	class RxThread: FreeRTOSTask {
		friend MrfRadio;
		RxThread(MrfRadio &radio) :
				radio(radio) {
		}
		void run();
		MrfRadio &radio;
	} rxThread = RxThread(*this);

	/**
	 * Transmitting thread
	 */
	class TxThread: FreeRTOSTask {
		friend MrfRadio;
		TxThread(MrfRadio &radio) :
				FreeRTOSTask(1024, 2), radio(radio) {
		}
		void run();
		MrfRadio &radio;
	} txThread = TxThread(*this);

	/**
	 * Static listener for broadcast complete event
	 *
	 * Casts data into object and calls broadcastCompleteListener on it.
	 */
	static void broadcastCompleteListenerStatic(void *data, const bool success);

	/**
	 * Broadcast complete handler
	 */
	void broadcastCompleteListener(const bool success);

	/**
	 * Static listener for received packet
	 *
	 * Casts data into object and calls receiveListener on it.
	 */
	static void receiverListenerStatic(void *data);

	/**
	 * Receive event handler
	 */
	void receiveListener();
};

#endif // MRFRADIO_H
