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
		Radio(uint8_t channel, uint16_t panId, uint16_t sourceAddress) {
			mrf.setRecvListener(receiverListenerStatic, this);
			mrf.setBroadcastCompleteListener(broadcastCompleteListenerStatic, this);

			mrf.setChannel(channel);

			mrf.setPANId((uint8_t*) &panId);
			mrf.setSAddr((uint8_t*) &sourceAddress);

			mrf.reset();
		}

		void setReceiver(Receiver *receiver) {
			this->receiver = receiver;
		}

		void broadcastFragment(const KnowledgeFragment fragment) {
			txBuffer.put(fragment);
		}

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

		Receiver *receiver = NULL;
		bool txInProgress = false;
		RingBuffer<KnowledgeFragment, 5> txBuffer;
		RingBuffer<Packet, 5> rxBuffer;
		FreeRTOSSemaphore txSem = FreeRTOSSemaphore(1000, 1);

		class RxThread: FreeRTOSTask {
			friend Radio;
			RxThread(Radio &radio) :
					radio(radio) {
			}
			void run() {
				console.print(Info, ">>>> Radio RX thread started\n");
				while(true) {
					Packet packet = radio.rxBuffer.get();
					if(packet.valid && packet.data.fragment.length() == packet.size && radio.receiver)
						radio.receiver->receiveFragment(packet.data.fragment, packet.lqi);
				}
			}
			Radio &radio;
		} rxThread = RxThread(*this);

		class TxThread: FreeRTOSTask {
			friend Radio;
			TxThread(Radio &radio) :
					FreeRTOSTask(1024, 2), radio(radio) {
			}
			void run() {
				console.print(Info, ">>>> Radio TX thread started\n");
				while(true) {
					KnowledgeFragment fragment = radio.txBuffer.get();
					radio.txSem.take();
					taskDISABLE_INTERRUPTS();
					mrf.broadcastPacket((uint8_t*) &fragment, (uint8_t) fragment.length());
					taskENABLE_INTERRUPTS();
				}
			}
			Radio &radio;
		} txThread = TxThread(*this);

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

	};
}

#endif // RADIO_H_
