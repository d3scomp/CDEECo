/*
 * Radio.h
 *
 *  Created on: 27. 5. 2014
 *      Author: Vladimír Matěna
 */

#ifndef RADIO_H_
#define RADIO_H_

#include "Receiver.h"
#include "Broadcaster.h"
#include "Console.h"

#include "drivers/MRF24J40.h"

class Radio: Broadcaster {
public:
	Radio(Receiver &receiver) :
			receiver(receiver) {
		greenLed.init();
		redLed.init();
		greenPulseLed.init();
		redPulseLed.init();

		mrf.setSPIPriority(0,0);
		mrf.setRFPriority(2,0);
		mrf.init();

		mrf.setRecvListener(receiverListenerStatic, this);
	}

	void broadcastFragment(const KnowledgeFragment fragment) {
		mrf.broadcastPacket((uint8_t*)&fragment, (uint8_t)fragment.length());
	}

	static void receiverListenerStatic(void *data) {
		static_cast<Radio*>(data)->receiveListener();
	}

	void receiveListener() {
		Console::log(">>>> About to receive data from RF interface");

		KnowledgeFragment fragment;

		// TODO: Do not copy data

		uint8_t data[128];
		uint8_t size;
		uint8_t srcPanId[2];
		uint8_t srcSAddr[2];
		uint8_t fcs[2];
		uint8_t lqi;
		uint8_t rssi;

		bool ok = mrf.recvPacket(data, size, srcPanId, srcSAddr, fcs, lqi, rssi);

		if(ok) {
			Console::log(">>>> Received knowledge via RF interface");

			memcpy(&fragment, data, sizeof(fragment));

			receiver.receiveFragment(fragment);

		} else {
			Console::log(">>>> Failed to receive knowledge via RF interface");
		}
	}

public:
	Receiver &receiver;

	// Receiver and transmit LEDs
	static LED greenLed;
	static LED redLed;
	static PulseLED greenPulseLed;
	static PulseLED redPulseLed;


	static MRF24J40 mrf;
};

#endif // RADIO_H_
