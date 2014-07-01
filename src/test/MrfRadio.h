/*
 * MrfRadio.h
 *
 *  Created on: 24. 6. 2014
 *      Author: Vladimír Matěna
 */

#ifndef MRFRADIO_H
#define MRFRADIO_H

#include "cdeeco/KnowledgeFragment.h"
#include "cdeeco/Knowledge.h"
#include "cdeeco/Radio.h"

class MrfRadio: public CDEECO::Radio {
public:
	MrfRadio(uint8_t channel, uint16_t panId, uint16_t sourceAddress);
	void broadcastFragment(const CDEECO::KnowledgeFragment fragment);

private:
	template<typename T, size_t SIZE>
	class RingBuffer {
	public:
		void put(const T &item) {
			data[start++ % SIZE] = item;
			sem.giveFromISR();
		}

		T get() {
			sem.take();
			return data[end++ % SIZE];
		}

	private:
		size_t start = 0;
		size_t end = 0;
		std::array<T, SIZE> data;
		FreeRTOSSemaphore sem = FreeRTOSSemaphore(10000, 0);
	};

	struct Packet {
		union {
			uint8_t data[CDEECO::KnowledgeFragment::MAX_PACKET_SIZE];
			CDEECO::KnowledgeFragment fragment;
		} data;

		uint8_t size;
		uint8_t srcPanId[2];
		uint8_t srcSAddr[2];
		uint8_t fcs[2];
		uint8_t lqi;
		uint8_t rssi;
		bool valid;
	};

	bool txInProgress = false;
	RingBuffer<CDEECO::KnowledgeFragment, 5> txBuffer;
	RingBuffer<Packet, 5> rxBuffer;
	FreeRTOSSemaphore txSem = FreeRTOSSemaphore(1000, 1);

	class RxThread: FreeRTOSTask {
		friend MrfRadio;
		RxThread(MrfRadio &radio) :
				radio(radio) {
		}
		void run();
		MrfRadio &radio;
	} rxThread = RxThread(*this);

	class TxThread: FreeRTOSTask {
		friend MrfRadio;
		TxThread(MrfRadio &radio) :
				FreeRTOSTask(1024, 2), radio(radio) {
		}
		void run();
		MrfRadio &radio;
	} txThread = TxThread(*this);

	static void broadcastCompleteListenerStatic(void *data, const bool success);
	void broadcastCompleteListener(const bool success);
	static void receiverListenerStatic(void *data);
	void receiveListener();
};

#endif // MRFRADIO_H
