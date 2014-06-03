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

#include "main.h"
#include "Receiver.h"
#include "Broadcaster.h"
#include "Console.h"
#include "FragmentBuffer.h"
#include "FreeRTOSSemaphore.h"
#include "FreeRTOSTask.h"

#include "drivers/MRF24J40.h"

class Radio: Broadcaster {
public:
	template<typename T, size_t SIZE>
	class RingBuffer {
	public:
		void put(const T &item) {
			data[start++ % SIZE] = item;
			//sem.give();
			sem.giveFromISR();
		}

		T* ptrPut() {
			//		sem.give();
			sem.giveFromISR();
			return &data[start++ % SIZE];
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
			uint8_t data[128];
			KnowledgeFragment fragment;
		} data;

		uint8_t size;
		uint8_t srcPanId[2];
		uint8_t srcSAddr[2];
		uint8_t fcs[2];
		uint8_t lqi;
		uint8_t rssi;
		bool valid;
	};

	Radio(Receiver &receiver) :
			receiver(receiver) {
		greenLed.init();
		redLed.init();
		greenPulseLed.init();
		redPulseLed.init();

		mrf.setSPIPriority(0, 0);
		mrf.setRFPriority(1, 0);

		mrf.setRecvListener(receiverListenerStatic, this);
		mrf.setBroadcastCompleteListener(broadcastCompleteListenerStatic, this);

		mrf.init();

		mrf.setChannel(0);

		uint8_t panId[] { 43, 44 };
		uint8_t sAddr[] { 45, 46 };

		mrf.setPANId(panId);
		mrf.setSAddr(sAddr);

		mrf.reset();
	}

	void broadcastFragment(const KnowledgeFragment fragment) {
		txBuffer.put(fragment);

		/*	vTaskSuspendAll();
		 txBuffer.put(fragment);
		 tryBroadcast();
		 xTaskResumeAll();*/
		/*
		 taskDISABLE_INTERRUPTS();
		 if(!txInProgress) {
		 txInProgress = true;
		 mrf.broadcastPacket((uint8_t*) &fragment, (uint8_t) fragment.length());
		 } else {
		 txBuffer.put(fragment);
		 }
		 taskENABLE_INTERRUPTS();
		 */

		//	bool go = false;
		//go = __sync_bool_compare_and_swap(&txInProgress, false, true);
		//	vTaskSuspendAll();
		//	go = __atomic_exchange_n(&txInProgress, true, __ATOMIC_SEQ_CST);
//		taskDISABLE_INTERRUPTS();
		//	if(!txInProgress) {
		//		txInProgress = true;
		//		go = true;
		//	}
		//	taskENABLE_INTERRUPTS();
		//	if(go)
		//		mrf.broadcastPacket((uint8_t*) &fragment, (uint8_t) fragment.length());
		//	xTaskResumeAll();
	}

	/*	void tryBroadcast() {
	 if(!txBuffer.isEmpty() && !txInProgress) {
	 KnowledgeFragment fragment;
	 txBuffer.get(fragment);
	 txInProgress = true;
	 mrf.broadcastPacket((uint8_t*) &fragment, (uint8_t) fragment.length());
	 }
	 }*/

	static void broadcastCompleteListenerStatic(void *data, const bool success) {
		static_cast<Radio*>(data)->broadcastCompleteListener(success);
	}

	void broadcastCompleteListener(const bool success) {
		//	vTaskSuspendAll();
		//	txSem.giveFromISR();

		/*		taskDISABLE_INTERRUPTS();

		 KnowledgeFragment fragment;
		 if(txBuffer.get(fragment)) {
		 mrf.broadcastPacket((uint8_t*) &fragment, (uint8_t) fragment.length());
		 } else {
		 txInProgress = false;
		 }

		 /*		txInProgress = false;
		 tryBroadcast();*/

//		taskENABLE_INTERRUPTS();
		//	xTaskResumeAll();
		//	if(!success)
		//		Console::log(">>>> Broadcast failed");
		/*
		 KnowledgeFragment fragment;
		 fragment.size = 112;
		 mrf.broadcastPacket((uint8_t*) &fragment, (uint8_t) fragment.length());*/
	}

	static void receiverListenerStatic(void *data) {
		static_cast<Radio*>(data)->receiveListener();
	}

	void receiveListener() {
		Packet packet;
		packet.valid = mrf.recvPacket(packet.data.data, packet.size, packet.srcPanId, packet.srcSAddr, packet.fcs,
				packet.lqi, packet.rssi);
		if(packet.valid)
			rxBuffer.put(packet);
	}

private:
	Receiver &receiver;
	bool txInProgress = false;
	RingBuffer<KnowledgeFragment, 5> txBuffer;
	RingBuffer<Packet, 5> rxBuffer;
	FreeRTOSSemaphore txSem = FreeRTOSSemaphore(1000, 1);

	class RxThread: FreeRTOSTask {
		friend Radio;
		RxThread(Receiver &receiver, RingBuffer<Packet, 5> &rxBuffer) :
				receiver(receiver), rxBuffer(rxBuffer) {
		}
		void run() {
			Console::log(">>>> System fragment processing task start");
			while(true) {
				Packet packet = rxBuffer.get();
				// TODO: Check size match, pass link quality, rssi to be processed by rebroadcaster
				receiver.receiveFragment(packet.data.fragment);
			}
		}
		Receiver &receiver;
		RingBuffer<Packet, 5> &rxBuffer;
	} rxThread = RxThread(receiver, rxBuffer);

	class TxThread: FreeRTOSTask {
		friend Radio;
		TxThread(RingBuffer<KnowledgeFragment, 5> &txBuffer, FreeRTOSSemaphore &txSem) :
				FreeRTOSTask(1024, 2), txBuffer(txBuffer), txSem(txSem) {
		}
		void run() {
			while(true) {
				//taskDISABLE_INTERRUPTS();
				txSem.take();
			//	delayTimer.mDelay(100);
				KnowledgeFragment fragment = txBuffer.get();
				mrf.broadcastPacket((uint8_t*) &fragment, (uint8_t) fragment.length());
				//taskENABLE_INTERRUPTS();
			}
		}
		RingBuffer<KnowledgeFragment, 5> &txBuffer;
		FreeRTOSSemaphore &txSem;
	} txThread = TxThread(txBuffer, txSem);

public:
//TODO: Should be private<
// Receiver and transmit LEDs
	static LED greenLed;
	static LED redLed;
	static PulseLED greenPulseLed;
	static PulseLED redPulseLed;

	static MRF24J40 mrf;
};

#endif // RADIO_H_
