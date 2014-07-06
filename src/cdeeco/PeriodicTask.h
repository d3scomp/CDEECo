/*
 * PeriodicTask.h
 *
 *  Created on: 28. 3. 2014
 *      Author: Vladimir Matěna
 */

#ifndef PERIODICTASK_H
#define PERIODICTASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "LED.h"

#include "Task.h"

#include "Console.h"
#include "wrappers/FreeRTOSTask.h"

#include <stdio.h>
#include <sstream>
#include <string>

namespace CDEECO {
	/**
	 * Periodic task implementation with output knowledge
	 */
	template<typename KNOWLEDGE, typename OUT_KNOWLEDGE>
	class PeriodicTask: Task<KNOWLEDGE, OUT_KNOWLEDGE>, FreeRTOSTask {
	public:
		// Create the periodic task
		PeriodicTask(long period, auto &component, auto &outKnowledge, size_t stack = FreeRTOSTask::DEFAULT_STACK_SIZE,
				unsigned long priority = FreeRTOSTask::DEFAULT_PRIORITY) :
				Task<KNOWLEDGE, OUT_KNOWLEDGE>(component, outKnowledge), FreeRTOSTask(stack, priority), period(period) {
			console.print(Debug, ">> PeriodicTask constructor\n");
		}

		// Create the periodic task
		PeriodicTask(long period, auto &component, size_t stack = FreeRTOSTask::DEFAULT_STACK_SIZE,
				unsigned long priority = FreeRTOSTask::DEFAULT_PRIORITY) :
				Task<KNOWLEDGE, OUT_KNOWLEDGE>(component), FreeRTOSTask(stack, priority), period(period) {
			console.print(Debug, ">> PeriodicTask constructor\n");
		}

	private:
		long period;

		/** Periodic task body implementation, responsible for periodic scheduling */
		void run() {
			// Schedule the task periodically
			while(1) {
				// Run the task
				this->execute();

				// Wait for next execution time
				vTaskDelay(this->period / portTICK_PERIOD_MS);
			}
		}
	};
}

#endif // PERIODICTASK_H
