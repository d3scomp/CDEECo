/*
 * TrigerredTask.h
 *
 *  Created on: 14.4. 2014
 *      Author: Vladimir Matěna
 */

#ifndef TRIGERREDTASK_H_
#define TRIGERREDTASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include <stdio.h>
#include <sstream>
#include <string>


#include "Task.h"
#include "ListedTriggerTask.h"
#include "Console.h"
#include "LED.h"

using namespace std;

template<typename KNOWLEDGE, typename TRIGGER_KNOWLEDGE, typename OUT_KNOWLEDGE>
class TriggeredTask: Task<KNOWLEDGE, OUT_KNOWLEDGE>, ListedTriggerTask, FreeRTOSTask<> {
public:
	// Create the triggered task
	TriggeredTask(TRIGGER_KNOWLEDGE &trigger, Component<KNOWLEDGE> &component, OUT_KNOWLEDGE &outKnowledge) :
			Task<KNOWLEDGE, OUT_KNOWLEDGE>(component, outKnowledge), trigger(trigger), triggerSem(
					xSemaphoreCreateCounting(TriggerMaxWaiting, 0)) {
		Console::log(">> TrigerredTask constructor");

		// List task in component check list
		component.addTriggeredTask(*this);
	}

	virtual ~TriggeredTask() {
	}

protected:
	// Check trigger condition
	void checkTriggerConditionData(void *updateStart, void* updateEnd) {
		// Check for trigger knowledge update
		if(updateStart >= &trigger && updateStart < &trigger + sizeof(trigger)) {
			// TODO check knowledge for real change if required

			// Trigger task execution
			xSemaphoreGive(triggerSem);
		}
	}
private:
	TRIGGER_KNOWLEDGE &trigger;
	SemaphoreHandle_t triggerSem;
	const unsigned TriggerMaxWaiting = 100;

	/** Periodic task body implementation, responsible for periodic scheduling */
	void run() {
		// Schedule the task periodically
		while(1) {
			// Wait for trigger event
			xSemaphoreTake(triggerSem, portMAX_DELAY);

			// Run the task
			this->execute();
		}
	}
};

#endif /* TRIGERREDTASK_H_ */
