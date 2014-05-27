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
	}

	void broadcastFragment(const KnowledgeFragment fragment) {
		mrf.broadcastPacket((uint8_t*)&fragment, (uint8_t)fragment.length());
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
