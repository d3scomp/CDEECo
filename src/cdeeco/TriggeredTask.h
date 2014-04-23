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

#include "LED.h"

#include "Task.h"
#include "ListedTriggerTask.h"

#include "console.h"

#include <stdio.h>
#include <sstream>
#include <string>

using namespace std;

template<typename KNOWLEDGE, typename TRIGGER_KNOWLEDGE, typename IN_KNOWLEDGE, typename OUT_KNOWLEDGE>
class TriggeredTask: Task<KNOWLEDGE, IN_KNOWLEDGE, OUT_KNOWLEDGE>, ListedTriggerTask {
public:
	// Create the triggered task
	TriggeredTask(TRIGGER_KNOWLEDGE &trigger, Component<KNOWLEDGE> &component, const IN_KNOWLEDGE &inKnowledge,
			OUT_KNOWLEDGE &outKnowledge) :
			Task<KNOWLEDGE, IN_KNOWLEDGE, OUT_KNOWLEDGE>(component, inKnowledge, outKnowledge), trigger(trigger), triggerSem(xSemaphoreCreateCounting(TriggerMaxWaiting, 0)) {
		Console::log(">> TrigerredTask constructor");

		// Create task
		xTaskCreate(taskBodyLauncher, "TrigerredTask", this->DefaultStackSize, this, this->DefaultPriority, &handle);

		// List task in component check list
		component.addTriggeredTask(*this);
	}

	virtual ~TriggeredTask() {}

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
	TaskHandle_t handle;
	TRIGGER_KNOWLEDGE &trigger;
	SemaphoreHandle_t triggerSem;
	const unsigned TriggerMaxWaiting = 100;

	/** Helper for launching task code from RTOS C environment */
	static void taskBodyLauncher(void *data) {
		Console::log(">> TriggerTask body");

		((TriggeredTask<KNOWLEDGE, TRIGGER_KNOWLEDGE, IN_KNOWLEDGE, OUT_KNOWLEDGE>*) data)->taskBodyImplementation();

		// Do not let the task run to the end
		while (1) {}
	}

	/** Periodic task body implementation, responsible for periodic scheduling */
	void taskBodyImplementation() {
		// Schedule the task periodically
		while (1) {
			// Wait for trigger event
			xSemaphoreTake(triggerSem, portMAX_DELAY);

			// Run the task
			this->execute();
		}
	}
};

#endif /* TRIGERREDTASK_H_ */
