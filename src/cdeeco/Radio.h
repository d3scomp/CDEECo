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
			receiver(receiver), greenLed(greenLedProps), redLed(redLedProps), greenPulseLed(greenLed, 1), redPulseLed(
					redLed, 1), mrf(mrfProps, greenPulseLed, redPulseLed) {
		greenLed.init();
		redLed.init();
	}

	void broadcastFragment(const KnowledgeFragment fragment) {
		// TODO: Broadcast fragment

	}

private:
	Receiver &receiver;

	// Receiver and transmit LEDs
	LED::Properties greenLedProps { GPIOD, GPIO_Pin_12, RCC_AHB1Periph_GPIOD };
	LED::Properties redLedProps { GPIOD, GPIO_Pin_14, RCC_AHB1Periph_GPIOD };
	LED greenLed;
	LED redLed;
	PulseLED greenPulseLed;
	PulseLED redPulseLed;

	// MRF24J40
	MRF24J40::Properties mrfProps {
		GPIOE, GPIOE, GPIOB, GPIOD,
		SPI3,
		GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_2,
		GPIO_PinSource4, GPIO_PinSource5, GPIO_PinSource3, GPIO_PinSource4, GPIO_PinSource5,
		RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOD,
		RCC_APB1PeriphClockCmd, RCC_APB1Periph_SPI3,
		GPIO_AF_SPI3,
		EXTI_Line2, EXTI_PortSourceGPIOD, EXTI_PinSource2, EXTI2_IRQn,
		SPI3_IRQn
	};
	MRF24J40 mrf;
};

#endif // RADIO_H_
