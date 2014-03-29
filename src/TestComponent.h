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

	};

	bool state;

	// Test task code
	TestKnowledge run(TestKnowledge in) {
		LED::Properties greenLedProps {
			GPIOD, GPIO_Pin_12, RCC_AHB1Periph_GPIOD
		};
		LED green(greenLedProps);

		green.init();
		if(state)
			green.on();
		else
			green.off();

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
