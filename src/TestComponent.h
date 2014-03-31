/*
 * TestComponent.h
 *
 *  Created on: 28. 3. 2014
 *      Author: vlada
 */

#ifndef TESTCOMPONENT_H_
#define TESTCOMPONENT_H_

#include "Component.h"

class TestKnowledge: Knowledge {
public:
	SimpleKnowledge<int> id;
	SimpleKnowledge<float> value;

	TestKnowledge():Knowledge(NULL), id(this), value(this) {}
};

class TestTask: public PeriodicTask<TestKnowledge, TestKnowledge> {
public:
	TestTask(TestKnowledge *in, TestKnowledge *out): PeriodicTask(500, in, out), state(false) {
		Console::log("TestComponent");

		// Init green led
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

		GPIO_InitTypeDef  gpioInitStruct = {
			GPIO_Pin_12, GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_UP
		};
		GPIO_Init(GPIOD, &gpioInitStruct);
	};

	bool state;

	// Test task code
	TestKnowledge run(TestKnowledge in) {
		if(!state)
			GPIOD->BSRRL = GPIO_Pin_12;
		else
			GPIOD->BSRRH = GPIO_Pin_12;

		state = !state;

		// TODO: Do something with the "in" knowledge return the "out" knowledge
		TestKnowledge out;
		return out;
	}
};

class TestComponent: public Component<TestKnowledge> {
public:
	TestTask task;

	TestComponent(): task(&this->knowledge, &this->knowledge) {}
};

#endif /* TESTCOMPONENT_H_ */
