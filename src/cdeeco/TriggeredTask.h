/**
 * \ingroup cdeeco
 * @file TriggeredTask.h
 *
 * CDEECO++ base Triggered task template class
 *
 * \date 14.4. 2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 */

#ifndef TRIGERREDTASK_H
#define TRIGERREDTASK_H

#include <stdio.h>
#include <sstream>
#include <string>
#include <type_traits>

#include "Task.h"
#include "ListedTriggerTask.h"
#include "Console.h"
#include "LED.h"
#include "wrappers/FreeRTOSSemaphore.h"

namespace CDEECO {
	/**
	 * \ingroup cdeeco
	 *
	 * Triggered task implementation
	 *
	 * Thanks to C++1y "auto" this implementation compiles even when OUT_KNOWLEDGE = void.
	 *
	 * @tparam KNOWLEDGE Type of knowledge of the component this task belongs to
	 * @tparam TRIGGER_KNOWLEDGE Type of knowledge that triggers this task's execution
	 * @tparam OUT_KNOWLEDGE Type of knowledge this task outputs
	 */
	template<typename KNOWLEDGE, typename TRIGGER_KNOWLEDGE, typename OUT_KNOWLEDGE>
	class TriggeredTask: Task<KNOWLEDGE, OUT_KNOWLEDGE>, ListedTriggerTask, FreeRTOSTask {
	public:
		/**
		 * Triggered task constructor with output knowledge
		 *
		 * @param trigger Reference to trigger knowledge in the component's knowledge
		 * @param component Reference to the component
		 * @param outKnowledge Reference to output knowledge in the component's knowledge
		 * @param stack Task stack size
		 * @param priority Task priority
		 */
		TriggeredTask(TRIGGER_KNOWLEDGE &trigger, auto &component, auto &outKnowledge, size_t stack =
				FreeRTOSTask::DEFAULT_STACK_SIZE, unsigned long priority = FreeRTOSTask::DEFAULT_PRIORITY) :
				Task<KNOWLEDGE, OUT_KNOWLEDGE>(component, outKnowledge), FreeRTOSTask(stack, priority), trigger(
						trigger), triggerSem(MAX_WAITING) {
			console.print(Info, ">> TrigerredTask constructor\n");

			// List task in component check list
			component.addTriggeredTask(*this);
		}

		/**
		 * Triggered task constructor without output knowledge
		 *
		 * @param trigger Reference to trigger knowledge in the component's knowledge
		 * @param component Reference to the component
		 * @param stack Task stack size
		 * @param priority Task priority
		 */
		TriggeredTask(TRIGGER_KNOWLEDGE &trigger, auto &component, size_t stack = FreeRTOSTask::DEFAULT_STACK_SIZE,
				unsigned long priority = FreeRTOSTask::DEFAULT_PRIORITY) :
				Task<KNOWLEDGE, OUT_KNOWLEDGE>(component), FreeRTOSTask(stack, priority), trigger(trigger), triggerSem(
						MAX_WAITING) {
			console.print(Info, ">> TrigerredTask constructor\n");

			// List task in component check list
			component.addTriggeredTask(*this);
		}

	protected:
		/**
		 * Check whenever the trigger condition is met
		 *
		 * This automatically triggers task execution in another thread when the condition is met,
		 * thus return is void.
		 *
		 * @param updateStart Pointer to start of changed area in the knowledge
		 * @param updateEnd Pointer to the end of changed area in the knowledge
		 */
		void checkTriggerConditionData(const void *updateStart, const void* updateEnd) {
			// Check for trigger knowledge update
			if(updateStart >= &trigger && updateStart < &trigger + sizeof(trigger)) {
				triggerSem.give();
			}
		}

	private:
		/// Reference to trigger knowledge in the component's knowledge
		TRIGGER_KNOWLEDGE &trigger;
		/**
		 *  Max triggers waiting
		 *
		 *  This value should not be reached. Actually the triggerSem should not have higher value than 1.
		 *  Higher values means that triggers come faster than task executes, which is bad.
		 */
		const unsigned MAX_WAITING = 10000;
		/**
		 * Trigger semaphore
		 *
		 * Lowered on task execution, rise on trigger event.
		 */
		FreeRTOSSemaphore triggerSem;

		/**
		 * Periodic task body implementation
		 *
		 * Responsible for periodic scheduling.
		 */
		void run() {
			// Schedule the task periodically
			while(1) {
				// Wait for trigger event
				triggerSem.take();

				// Run the task
				this->execute();
			}
		}
	};
}

#endif // TRIGERREDTASK_H
