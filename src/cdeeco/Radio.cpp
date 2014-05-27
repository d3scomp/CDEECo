/*
 * Radio.cpp
 *
 *  Created on: 27. 5. 2014
 *      Author: Vladimír Matěna
 */

#include "Radio.h"

LED::Properties greenLedProps { GPIOD, GPIO_Pin_12, RCC_AHB1Periph_GPIOD };
LED::Properties redLedProps { GPIOD, GPIO_Pin_14, RCC_AHB1Periph_GPIOD };
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

LED Radio::greenLed = LED(greenLedProps);
LED Radio::redLed = LED(redLedProps);
PulseLED Radio::greenPulseLed = PulseLED(Radio::greenLed, 1);
PulseLED Radio::redPulseLed = PulseLED(Radio::redLed, 1);
MRF24J40 Radio::mrf = MRF24J40(mrfProps, Radio::greenPulseLed, Radio::redPulseLed);
