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
	struct Position {
		int x;
		int y;
	};

	typedef int Id;
	typedef float Value;

	Id id;
	Value value;
	Position position;
};

/**
 * Test component periodic task
 *
 * Uses periodic scheduling.
 * It processes whole component knowledge and outputs whole component knowledge.
 *
 * The task blinks the green LED and changes position knowledge.
 */
class TestPeriodicTask: public PeriodicTask<TestKnowledge, TestKnowledge::Position, TestKnowledge::Position> {
public:
	// Task initialization
	TestPeriodicTask(Component<TestKnowledge> &component, const TestKnowledge::Position &in,
			TestKnowledge::Position &out) :
			PeriodicTask(250, component, in, out), led(green) {
		led.init();
	}

private:
	LED::Properties green { GPIOD, GPIO_Pin_12, RCC_AHB1Periph_GPIOD };
	LED led;

protected:
	// Task code
	TestKnowledge::Position run(const TestKnowledge::Position in) {
		// Visualize knowledge position x
		int x = in.x;
		char num[17] = "> PeriodTask    ";
		for (int i = 0; i < 10 && x > 0; ++i) {
			num[15 - i] = '0' + x % 10;
			x /= 10;
		}
		Console::log(num);

		if (in.x % 2)
			led.off();
		else
			led.on();

		// TODO: Do something with the "in" knowledge return the "out" knowledge
		TestKnowledge::Position out;

		// Change position
		out.x = in.x + 1;
		out.y = in.y + in.x % 2;

		return out;
	}
};

/**
 * Test component triggered task
 */
class TestTriggeredTask: public TriggeredTask<TestKnowledge, TestKnowledge::Position, TestKnowledge::Position,
		TestKnowledge::Value> {
public:
	// Task initialization
	TestTriggeredTask(TestKnowledge::Position &trigger, Component<TestKnowledge> &component,
			const TestKnowledge::Position &inKnowledge, TestKnowledge::Value &outKnowledge) :
			TriggeredTask(trigger, component, inKnowledge, outKnowledge), led(red) {
		led.init();
	}

private:
	LED::Properties red { GPIOD, GPIO_Pin_13, RCC_AHB1Periph_GPIOD };
	LED led;

protected:
	// Task code
	TestKnowledge::Value run(const TestKnowledge::Position in) {
		Console::log("> Triggered task running now");

		if (in.x % 2)
			led.off();
		else
			led.on();

		return 42;
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

	TestComponent() :
			periodicTask(*this, this->knowledge.position, this->knowledge.position), triggeredTask(
					this->knowledge.position, *this, this->knowledge.position, this->knowledge.value) {
		// Initialize knowledge
		memset(&knowledge, 0, sizeof(TestKnowledge));
	}
};

#endif /* TESTCOMPONENT_H_ */
