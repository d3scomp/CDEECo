/*
 * TrigerredTask.h
 *
 *  Created on: 14.4. 2014
 *      Author: Vladimir Matěna
 */

#ifndef TRIGERREDTASK_H_
#define TRIGERREDTASK_H_

#include <stdio.h>
#include <sstream>
#include <string>
#include <type_traits>

#include "Task.h"
#include "ListedTriggerTask.h"
#include "Console.h"
#include "LED.h"
#include "FreeRTOSSemaphore.h"

namespace CDEECO {
	/**
	 * Triggered task implementation with output knowledge
	 */
	template<typename KNOWLEDGE, typename TRIGGER_KNOWLEDGE, typename OUT_KNOWLEDGE>
	class TriggeredTask: Task<KNOWLEDGE, OUT_KNOWLEDGE>, ListedTriggerTask, FreeRTOSTask {
	public:
		// Create the triggered task
		TriggeredTask(TRIGGER_KNOWLEDGE &trigger, auto &component, auto &outKnowledge) :
				Task<KNOWLEDGE, OUT_KNOWLEDGE>(component, outKnowledge), trigger(trigger), triggerSem(MAX_WAITING) {
			console.log(">> TrigerredTask constructor\n");

			// List task in component check list
			component.addTriggeredTask(*this);
		}

		// Create the triggered task
		TriggeredTask(TRIGGER_KNOWLEDGE &trigger, auto &component) :
				Task<KNOWLEDGE, OUT_KNOWLEDGE>(component), trigger(trigger), triggerSem(MAX_WAITING) {
			console.log(">> TrigerredTask constructor\n");

			// List task in component check list
			component.addTriggeredTask(*this);
		}

	protected:
		// Check trigger condition
		void checkTriggerConditionData(void *updateStart, void* updateEnd) {
			// Check for trigger knowledge update
			if(updateStart >= &trigger && updateStart < &trigger + sizeof(trigger)) {
				triggerSem.give();
			}
		}

	private:
		TRIGGER_KNOWLEDGE &trigger;
		const unsigned MAX_WAITING = 100;
		FreeRTOSSemaphore triggerSem;

		/** Periodic task body implementation, responsible for periodic scheduling */
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

#endif /* TRIGERREDTASK_H_ */
