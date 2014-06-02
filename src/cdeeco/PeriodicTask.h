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

#include "Console.h"
#include "FreeRTOSTask.h"

#include <stdio.h>
#include <sstream>
#include <string>

using namespace std;

namespace CDEECO {

template<typename KNOWLEDGE, typename OUT_KNOWLEDGE>
class PeriodicTask: Task<KNOWLEDGE, OUT_KNOWLEDGE>, FreeRTOSTask {
public:
	// Create the periodic task
	PeriodicTask(long period, Component<KNOWLEDGE> &component, OUT_KNOWLEDGE &outKnowledge):
			Task<KNOWLEDGE, OUT_KNOWLEDGE>(component, outKnowledge), period(period) {
		Console::log(">> PeriodicTask constructor");
	}
private:
	long period;

	/** Periodic task body implementation, responsible for periodic scheduling */
	void run() {
		// Schedule the task periodically
		while (1) {
			// Run the task
			this->execute();

			// Wait for next execution time
			vTaskDelay(this->period / portTICK_PERIOD_MS);
		}
	}
};

}

#endif /* PERIODICTASK_H_ */
