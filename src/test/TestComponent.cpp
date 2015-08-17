/**
 * \ingroup example
 * @file TestComponent.cpp
 *
 * This file contains CDEECo++ component implementation for testing early system
 *
 * \date 9. 7. 2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 *
 */

#include "../test/TestComponent.h"

namespace TestComponent {
	TestPeriodicTask::TestPeriodicTask(auto &component) :
			PeriodicTask(429, component, component.knowledge.normVal) {
	}

	Knowledge::NormValue TestPeriodicTask::run(const Knowledge in) {
		// Blink blue led
		if(in.normVal % 2)
			blueLED.off();
		else
			blueLED.on();

		return in.normVal + 1;
	}

	TestTriggeredTask::TestTriggeredTask(auto &component) :
			TriggeredTask(component.knowledge.normVal, component, component.knowledge.trigVal) {
	}

	Knowledge::TrigValue TestTriggeredTask::run(const Knowledge in) {
		return in.normVal * 2;
	}

	Component::Component(CDEECO::Broadcaster &broadcaster, const CDEECO::Id id) :
			CDEECO::Component<Knowledge>(id, 0x42, broadcaster) {
		// Initialize knowledge
		memset(&knowledge, 0, sizeof(Knowledge));
	}
}

namespace CDEECO {
	/// Define allowed offsets to guarantee knowledge consistency
	template<>
	struct KnowledgeTrait<Knowledge> {
		static constexpr std::array<size_t, 2> offsets = { { offsetof(TestComponent::Knowledge, normVal), offsetof(
				TestComponent::Knowledge, trigVal) } };
	};
	// This declaration do not require array size to be specified twice, but drives eclipse crazy.
	//constexpr decltype(KnowledgeTrait<TestKnowledge>::offsets) KnowledgeTrait<TestKnowledge>::offsets;
	constexpr std::array<size_t, 2> KnowledgeTrait<Knowledge>::offsets;
}
