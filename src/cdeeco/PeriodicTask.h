/*
 * PeriodicTask.h
 *
 *  Created on: 28. 3. 2014
 *      Author: Vladimir Matěna
 */

#ifndef PERIODICTASK_H_
#define PERIODICTASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "LED.h"

#include "Task.h"

#include "console.h"

#include <stdio.h>
#include <sstream>
#include <string>

using namespace std;

template<typename KNOWLEDGE, typename IN_KNOWLEDGE, typename OUT_KNOWLEDGE>
class PeriodicTask: Task<KNOWLEDGE, IN_KNOWLEDGE, OUT_KNOWLEDGE> {
public:
	// Create the periodic task
	PeriodicTask(long period, Component<KNOWLEDGE> &component, const IN_KNOWLEDGE &inKnowledge,
			OUT_KNOWLEDGE &outKnowledge) :
			Task<KNOWLEDGE, IN_KNOWLEDGE, OUT_KNOWLEDGE>(component, inKnowledge, outKnowledge), period(period) {
		Console::log(">> PeriodicTask constructor");

		xTaskCreate(taskBodyLauncher, "PeriodicTask", this->DefaultStackSize, this, this->DefaultPriority, &handle);
	}
private:
	long period;
	TaskHandle_t handle;

	/** Helper for launching task code from RTOS C environment */
	static void taskBodyLauncher(void *data) {
		Console::log(">> PeriodicTask body");

		((PeriodicTask<KNOWLEDGE, IN_KNOWLEDGE, OUT_KNOWLEDGE>*) data)->taskBodyImplementation();

		// Do not let the task run to the end
		while (1) {}
	}

	/** Periodic task body implementation, responsible for periodic scheduling */
	void taskBodyImplementation() {
		// Schedule the task periodically
		while (1) {
			// Run the task
			this->execute();

			// Wait for next execution time
			vTaskDelay(this->period / portTICK_PERIOD_MS);
		}
	}
};

#endif /* PERIODICTASK_H_ */
