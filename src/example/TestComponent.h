/**
 * \defgroup example Example application sources
 * CDEECo++ example application components' and ensembles' sources
 */

/**
 * \ingroup example
 * @file TestComponent.h
 *
 * This file contains CDEECo++ component implementation for testing early system
 *
 * \date 28. 3. 2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 *
 */

#ifndef TESTCOMPONENT_H
#define TESTCOMPONENT_H

#include <array>

#include "cdeeco/Component.h"
#include "cdeeco/Knowledge.h"
#include "cdeeco/PeriodicTask.h"
#include "cdeeco/TriggeredTask.h"

/**
 * Test component
 *
 * @see TestComponent::Component
 *
 * \ingroup example
 */
namespace TestComponent {
	/**
	 * Test component knowledge
	 *
	 * \ingroup example
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
	 *
	 * \ingroup example
	 */
	class TestPeriodicTask: public CDEECO::PeriodicTask<Knowledge, Knowledge::NormValue> {
	public:
		/**
		 * Test periodic task constructor
		 */
		TestPeriodicTask(auto &component);

	private:
		/**
		 * Test periodic task code
		 *
		 * @param in Read-only copy of component's knowledge
		 * @return "normal" value (task output)
		 */
		Knowledge::NormValue run(const Knowledge in);
	};

	/**
	 * Test component triggered task
	 *
	 * \ingroup example
	 */
	class TestTriggeredTask: public CDEECO::TriggeredTask<Knowledge, Knowledge::NormValue, Knowledge::TrigValue> {
	public:
		/**
		 * Test triggered task constructor
		 */
		TestTriggeredTask(auto &component);

	private:
		/**
		 * Test triggered task code
		 *
		 * @param in Read-only copy of component's knowledge
		 * @return "trigger" value (task output)
		 */
		Knowledge::TrigValue run(const Knowledge in);
	};

	/**
	 * Test component container
	 *
	 * Defines one periodic and one triggered task. Blinks with blue LED.
	 *
	 * \ingroup example
	 */
	class Component: public CDEECO::Component<Knowledge> {
	public:
		/// Test periodic task instance
		TestPeriodicTask periodicTask = TestPeriodicTask(*this);

		/// Test triggered task instance
		TestTriggeredTask triggeredTask = TestTriggeredTask(*this);

		/// Component's magic number
		static const auto Type = 0xcdec0;

		/**
		 * Test component constructor
		 */
		Component(CDEECO::Broadcaster &broadcaster, const CDEECO::Id id);
	};
}

#endif // TESTCOMPONENT_H
