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
#include "FreeRTOSSemaphore.h"
#include "FreeRTOSTask.h"

#include "drivers/MRF24J40.h"

namespace CDEECO {
	class Radio: Broadcaster {
	public:
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
				uint8_t data[KnowledgeFragment::MAX_PACKET_SIZE];
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

			mrf.setRecvListener(receiverListenerStatic, this);
			mrf.setBroadcastCompleteListener(broadcastCompleteListenerStatic, this);

			mrf.setChannel(0);

			uint8_t panId[] { 43, 44 };
			uint8_t sAddr[] { 45, 46 };

			mrf.setPANId(panId);
			mrf.setSAddr(sAddr);

			mrf.reset();
		}

		void broadcastFragment(const KnowledgeFragment fragment) {
			txBuffer.put(fragment);
		}

		static void broadcastCompleteListenerStatic(void *data, const bool success) {
			static_cast<Radio*>(data)->broadcastCompleteListener(success);
		}

		void broadcastCompleteListener(const bool success) {
			txSem.giveFromISR();
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
				Console::log(">>>> Radio RX thread started\n");
				while(true) {
					Packet packet = rxBuffer.get();
					if(packet.valid && packet.data.fragment.length() == packet.size)
						receiver.receiveFragment(packet.data.fragment, packet.lqi);
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
				Console::log(">>>> Radio TX thread started\n");
				while(true) {
					KnowledgeFragment fragment = txBuffer.get();
					txSem.take();
					taskDISABLE_INTERRUPTS();
					mrf.broadcastPacket((uint8_t*) &fragment, (uint8_t) fragment.length());
					taskENABLE_INTERRUPTS();
				}
			}
			RingBuffer<KnowledgeFragment, 5> &txBuffer;
			FreeRTOSSemaphore &txSem;
		} txThread = TxThread(txBuffer, txSem);
	};
}

#endif // RADIO_H_
