/*
 * PeriodicTask.h
 *
 *  Created on: 28. 3. 2014
 *      Author: vlada
 */

#ifndef PERIODICTASK_H_
#define PERIODICTASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "LED.h"

#include "Task.h"

template<typename IN_KNOWLEDGE, typename OUT_KNOWLEDGE>
class PeriodicTask: Task<IN_KNOWLEDGE, OUT_KNOWLEDGE> {
public:
	PeriodicTask(long period, IN_KNOWLEDGE *inKnowledge, OUT_KNOWLEDGE *outKnowledge):
		Task<IN_KNOWLEDGE, OUT_KNOWLEDGE>(inKnowledge, outKnowledge), period(period) {
		// Create the task
		void (*body)(void *) = (void (*)(void*))PeriodicTask<IN_KNOWLEDGE, OUT_KNOWLEDGE>::taskBodyLauncher;
		xTaskCreate(body, "PeriodicTask", this->DefaultStackSize, this, this->DefaultPriority, &handle);
	}
	virtual ~PeriodicTask() {};

	virtual OUT_KNOWLEDGE run(IN_KNOWLEDGE in) = 0;

private:
	long period;
	TaskHandle_t handle;

	/** Helper for launching task code from RTOS C environment */
	static void taskBodyLauncher(PeriodicTask<IN_KNOWLEDGE, OUT_KNOWLEDGE> *task) {
		//task->taskBodyImplementation();
		// Do not let the task run to the end
		while(1);
	}

	void taskBodyImplementation() {
		// Schedule the task periodically
		while(1) {
			// Run the task
			this->execute();

			// Wait for next execution time
			vTaskDelay(period / portTICK_PERIOD_MS);
		}
	}
};

#endif /* PERIODICTASK_H_ */
