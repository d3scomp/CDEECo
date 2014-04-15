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
#include "cdeeco/TriggeredTask.h"

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
 * Test component periodic task
 *
 * Uses periodic scheduling.
 * It processes whole component knowledge and outputs whole component knowledge.
 *
 * The task blinks the green LED and increases "id" value in the knowledge.
 */
class TestPeriodicTask: public PeriodicTask<TestKnowledge, TestKnowledge, TestKnowledge> {
public:
	// Task initialization
	TestPeriodicTask(Component<TestKnowledge> &component, const TestKnowledge &in, TestKnowledge &out): PeriodicTask(250, component, in, out), state(false), led(green) {
		led.init();
	};

private:
	LED::Properties green {
		GPIOD, GPIO_Pin_12, RCC_AHB1Periph_GPIOD
	};
	bool state;
	LED led;

protected:
	// Task code
	TestKnowledge run(const TestKnowledge in) {
		Console::log("PeriodicTaskRun");
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
 * Test component triggered task
 */
class TestTriggeredTask: public TriggeredTask<TestKnowledge, TestKnowledge, TestKnowledge, TestKnowledge> {
public:
	// Task initialization
	TestTriggeredTask(TestKnowledge &trigger, Component<TestKnowledge> &component, const TestKnowledge &inKnowledge,
			TestKnowledge &outKnowledge): TriggeredTask(trigger, component, inKnowledge, outKnowledge), state(false), led(red) {
		led.init();
	};

private:
	LED::Properties red {
		GPIOD, GPIO_Pin_13, RCC_AHB1Periph_GPIOD
	};
	bool state;
	LED led;

protected:
	// Task code
	TestKnowledge run(const TestKnowledge in) {
		Console::log("TriggerTaskRun");

		if(!state)
			led.off();
		else
			led.on();

		state = !state;

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
	TestPeriodicTask periodicTask;
	TestTriggeredTask triggeredTask;

	TestComponent(): periodicTask(*this, this->knowledge, this->knowledge), triggeredTask(this->knowledge, *this, this->knowledge, this->knowledge) {
		// Initialize knowledge
		memset(&knowledge, 0, sizeof(TestKnowledge));
	}
};

#endif /* TESTCOMPONENT_H_ */
