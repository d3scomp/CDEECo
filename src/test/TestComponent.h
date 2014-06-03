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

#include <array>

#include "cdeeco/Component.h"
#include "cdeeco/Knowledge.h"
#include "cdeeco/PeriodicTask.h"
#include "cdeeco/TriggeredTask.h"

/**
 * Test component knowledge
 *
 * Holds integer and float values named "id" and "value".
 *
 */
struct TestKnowledge: CDEECO::Knowledge {
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

namespace CDEECO {
	/// Define allowed offsets to guarantee knowledge consistency
	template<>
	struct KnowledgeTrait<TestKnowledge> {
		static constexpr std::array<size_t, 2> offsets = { { offsetof(TestKnowledge, id), offsetof(TestKnowledge,
				position) } };
	};
	// This declaration do not require array size to be specified twice, but drives eclipse crazy.
	//constexpr decltype(KnowledgeTrait<TestKnowledge>::offsets) KnowledgeTrait<TestKnowledge>::offsets;
	constexpr std::array<size_t, 2> KnowledgeTrait<TestKnowledge>::offsets;
}

/**
 * Test component periodic task
 *
 * Uses periodic scheduling.
 * It processes whole component knowledge and outputs whole component knowledge.
 *
 * The task blinks the green LED and changes position knowledge.
 */
class TestPeriodicTask: public CDEECO::PeriodicTask<TestKnowledge, TestKnowledge::Position> {
public:
	// Task initialization
	TestPeriodicTask(CDEECO::Component<TestKnowledge> &component, TestKnowledge::Position &out) :
			PeriodicTask(1000, component, out), led(blue) {
		led.init();
	}

private:
	LED::Properties blue { GPIOD, GPIO_Pin_15, RCC_AHB1Periph_GPIOD };
	LED led;

protected:
	// Task code
	TestKnowledge::Position run(const TestKnowledge in) {
		// Visualize knowledge position x
		char buff[20];
		sprintf(buff, "> Periodic task %d", in.position.x);
		Console::log(buff);

		if(in.position.x % 2)
			led.off();
		else
			led.on();

		// TODO: Do something with the "in" knowledge return the "out" knowledge
		TestKnowledge::Position out;

		// Change position
		out.x = in.position.x + 1;
		out.y = in.position.y + in.position.x % 2;

		return out;
	}
};

/**
 * Test component triggered task
 */
class TestTriggeredTask: public CDEECO::TriggeredTask<TestKnowledge, TestKnowledge::Position, TestKnowledge::Value> {
public:
	// Task initialization
	TestTriggeredTask(TestKnowledge::Position &trigger, CDEECO::Component<TestKnowledge> &component,
			TestKnowledge::Value &outKnowledge) :
			TriggeredTask(trigger, component, outKnowledge), led(red) {
		led.init();
	}

private:
	LED::Properties red { GPIOD, GPIO_Pin_13, RCC_AHB1Periph_GPIOD };
	LED led;

protected:
	// Task code
	TestKnowledge::Value run(const TestKnowledge in) {
		Console::log("> Triggered task running now");

		if(in.position.x % 2)
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
class TestComponent: public CDEECO::Component<TestKnowledge> {
public:
	TestPeriodicTask periodicTask;
	TestTriggeredTask triggeredTask;

	TestComponent(System &system) :
			CDEECO::Component<TestKnowledge>(0x42, system), periodicTask(*this, this->knowledge.position), triggeredTask(
					this->knowledge.position, *this, this->knowledge.value) {
		// Initialize knowledge
		memset(&knowledge, 0, sizeof(TestKnowledge));
	}
};

#endif /* TESTCOMPONENT_H_ */
