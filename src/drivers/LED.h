/*
 * LED.h
 *
 *  Created on: 15. 9. 2013
 *      Author: Tomas Bures <bures@d3s.mff.cuni.cz>
 */

#ifndef LED_H_
#define LED_H_

#include "stm32f4xx.h"

class LED {
public:
	struct Properties {
		GPIO_TypeDef* gpio;
		uint32_t pin;
		uint32_t clk;
	};

	LED(Properties& initProps);
	~LED();

	void on();
	void off();
	void init();

private:
	Properties props;
};

class PulseLED {
public:
	struct Properties {
		uint32_t periph;
		TIM_TypeDef *timer;
		IRQn irq;
		uint8_t priority;
		uint8_t subPriority;
	};

	PulseLED(LED& led, int minimalOnTimeTicks);
	~PulseLED();

	void pulse();
	void init();

	static void tickInterruptHandler();
	static void initTimer(Properties &props);

private:
	LED& led;
	int minimalOnTimeTicks;
	int onTicks;

	void tick();

	static constexpr auto MAX_TICK_LISTENERS = 5;
	static PulseLED* tickListeners[MAX_TICK_LISTENERS];
	static int tickListenersNum;
};

#endif /* LED_H_ */
