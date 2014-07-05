/*
 * TestComponent.h
 *
 *  Created on: 28. 3. 2014
 *      Author: Vladimír Matěna
 *
 *  This file contains cdeeco++ component implementation for testing early system.
 *
 */

#ifndef TESTCOMPONENT_H
#define TESTCOMPONENT_H

#include <array>

#include "cdeeco/Component.h"
#include "cdeeco/Knowledge.h"
#include "cdeeco/PeriodicTask.h"
#include "cdeeco/TriggeredTask.h"

namespace TestComponent {
	/**
	 * Test component knowledge
	 *
	 * Holds integer and float values named "id" and "value".
	 *
	 */
	struct Knowledge: CDEECO::Knowledge {
		typedef int NormValue;
		typedef int TrigValue;
		NormValue normVal;
		TrigValue trigVal;
	};

	/**
	 * Test component periodic task
	 *
	 * Uses periodic scheduling.
	 * It processes whole component knowledge and outputs whole component knowledge.
	 *
	 * The task blinks the green LED and changes position knowledge.
	 */
	class TestPeriodicTask: public CDEECO::PeriodicTask<Knowledge, Knowledge::NormValue> {
	public:
		// Task initialization
		TestPeriodicTask(auto &component) :
			PeriodicTask(429, component, component.knowledge.normVal) {
		}

	private:
		Knowledge::NormValue run(const Knowledge in) {
			// Blink blue led
			if(in.normVal % 2)
				blueLED.off();
			else
				blueLED.on();

			return in.normVal + 1;
		}
	};

	/**
	 * Test component triggered task
	 */
	class TestTriggeredTask: public CDEECO::TriggeredTask<Knowledge, Knowledge::NormValue, Knowledge::TrigValue> {
	public:
		// Task initialization
		TestTriggeredTask(auto &component) :
				TriggeredTask(component.knowledge.normVal, component, component.knowledge.trigVal) {
		}

	private:
		Knowledge::TrigValue run(const Knowledge in) {
			console.print(TaskInfo, "Test triggered task\n\n");
			return in.normVal * 2;
		}
	};

	/**
	 * Test component container
	 *
	 * Defines one periodic task.
	 */
	class Component: public CDEECO::Component<Knowledge> {
	public:
		TestPeriodicTask periodicTask = TestPeriodicTask(*this);
		TestTriggeredTask triggeredTask = TestTriggeredTask(*this);

		static const auto Type = 0xcdec0;

		Component(CDEECO::Broadcaster &broadcaster, const CDEECO::Id id) :
				CDEECO::Component<Knowledge>(id, 0x42, broadcaster) {
			// Initialize knowledge
			memset(&knowledge, 0, sizeof(Knowledge));
		}
	};

}

namespace CDEECO {
	/// Define allowed offsets to guarantee knowledge consistency
	template<>
	struct KnowledgeTrait<Knowledge> {
		static constexpr std::array<size_t, 2> offsets = { {
				offsetof(TestComponent::Knowledge, normVal),
				offsetof(TestComponent::Knowledge, trigVal)
		} };
	};
	// This declaration do not require array size to be specified twice, but drives eclipse crazy.
	//constexpr decltype(KnowledgeTrait<TestKnowledge>::offsets) KnowledgeTrait<TestKnowledge>::offsets;
	constexpr std::array<size_t, 2> KnowledgeTrait<Knowledge>::offsets;
}

#endif /* TESTCOMPONENT_H */
