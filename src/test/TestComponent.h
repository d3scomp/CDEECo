/*
 * TestComponent.h
 *
 *  Created on: 28. 3. 2014
 *      Author: Vladimír Matěna
 *
 *  This file contains cdeeco++ component implementation for testing early system.
 *
 */

#ifndef TESTCOMPONENT_H_
#define TESTCOMPONENT_H_

#include "cdeeco/Component.h"

/**
 * Test component knowledge
 *
 * Holds integer and float values named "id" and "value".
 *
 */
struct TestKnowledge {
	int id;
	float value;
	struct {
		int x;
		int y;
	} position;
};

/**
 * Test component task
 *
 * Uses periodic scheduling.
 * It processes whole component knowledge and outputs whole component knowledge.
 *
 * The task blinks the green LED and increases "id" value in the knowledge.
 */
class TestTask: public PeriodicTask<TestKnowledge, TestKnowledge, TestKnowledge> {
public:
	// Test task initialization
	TestTask(Component<TestKnowledge> &component, const TestKnowledge &in, TestKnowledge &out): PeriodicTask(250, component, in, out), state(false), led(green) {
		Console::log("TestTask");
		led.init();
	};

private:
	LED::Properties green {
		GPIOD, GPIO_Pin_12, RCC_AHB1Periph_GPIOD
	};
	bool state;
	LED led;

protected:
	// Test task code
	TestKnowledge run(const TestKnowledge in) {
		Console::log("TaskRun");
		// Visualize knowledge value
		int id = in.id;
		char num[17] = "0000000000000000";
		for(int i = 0; i < 16; i++)
			if(1 << i & id)
				num[15 - i] = '1';
		Console::log(num);

		if(!state)
			led.off();
		else
			led.on();

		state = !state;

		// TODO: Do something with the "in" knowledge return the "out" knowledge
		TestKnowledge out;

		// Increase ID and copy value
		out.id = in.id + 1;
		out.value = in.value;

		return out;
	}
};

/**
 * Test component container
 *
 * Defines one periodic task.
 */
class TestComponent: public Component<TestKnowledge> {
public:
	TestTask task;

	TestComponent(): task(*this, this->knowledge, this->knowledge) {
		// Initialize knowledge
		memset(&knowledge, 0, sizeof(TestKnowledge));
	}
};

#endif /* TESTCOMPONENT_H_ */
