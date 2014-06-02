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

#include "Receiver.h"
#include "Broadcaster.h"
#include "Console.h"
#include "FragmentBuffer.h"

#include "drivers/MRF24J40.h"

class Radio: Broadcaster {
public:
	Radio(Receiver &receiver) :
			receiver(receiver) {
		greenLed.init();
		redLed.init();
		greenPulseLed.init();
		redPulseLed.init();

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	// 2 bits for pre-emption priority, 2 bits for non-preemptive subpriority
		mrf.setSPIPriority(0, 0);
		mrf.setRFPriority(2, 0);
		mrf.init();

		//	mrf.reset();

		mrf.setRecvListener(receiverListenerStatic, this);
		mrf.setBroadcastCompleteListener(broadcastCompleteListenerStatic, this);

		mrf.setChannel(0);

		uint8_t panId[] { 43, 44 };
		uint8_t sAddr[] { 45, 46 };

		mrf.setPANId(panId);
		mrf.setSAddr(sAddr);

		/*EXTI_InitTypeDef EXTI_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

		 // Configure trigger EXTI line
		 EXTI_InitStructure.EXTI_Line = EXTI_Line2;
		 EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		 EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		 EXTI_Init(&EXTI_InitStructure);
		 */
		/*	// Enable and set trigger EXTI Interrupt to the lowest priority
		 NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
		 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		 NVIC_Init(&NVIC_InitStructure);*/
	}

	void broadcastFragment(const KnowledgeFragment fragment) {
		vTaskSuspendAll();
		txBuffer.put(fragment);
		tryBroadcast();
		xTaskResumeAll();
	}

	void tryBroadcast() {
		if(!txBuffer.isEmpty() && !txInProgress) {
			KnowledgeFragment fragment;
			txBuffer.get(fragment);
			txInProgress = true;
			mrf.broadcastPacket((uint8_t*) &fragment, (uint8_t) fragment.length());
		}
	}

	static void broadcastCompleteListenerStatic(void *data, const bool success) {
		static_cast<Radio*>(data)->broadcastCompleteListener(success);
	}

	void broadcastCompleteListener(const bool success) {
		vTaskSuspendAll();
		txInProgress = false;
		tryBroadcast();
		xTaskResumeAll();

		if(!success)
			Console::log(">>>> Broadcast failed ################");
		else
			Console::log(">>>> Broadcast complete ################");
	}

	static void receiverListenerStatic(void *data) {
		static_cast<Radio*>(data)->receiveListener();
	}

	void receiveListener() {
		Console::log(">>>> About to receive data from RF interface #####################");

		union {
			KnowledgeFragment fragment;
			uint8_t data[128];
		} buffer;

		uint8_t size;
		uint8_t srcPanId[2];
		uint8_t srcSAddr[2];
		uint8_t fcs[2];
		uint8_t lqi;
		uint8_t rssi;

		bool ok = mrf.recvPacket(buffer.data, size, srcPanId, srcSAddr, fcs, lqi, rssi);

		// TODO: Check size match, pass link quality, rssi to be processed by rebroadcaster

		if(ok) {
			Console::log(">>>> Received knowledge via RF interface");
			receiver.receiveFragment(buffer.fragment);
		} else {
			Console::log(">>>> Failed to receive knowledge via RF interface");
		}
	}

private:
	Receiver &receiver;
	bool txInProgress = false;
	FragmentBuffer<5> txBuffer;

public: //TODO: Should be private<
	// Receiver and transmit LEDs
	static LED greenLed;
	static LED redLed;
	static PulseLED greenPulseLed;
	static PulseLED redPulseLed;

	static MRF24J40 mrf;
};

#endif // RADIO_H_
