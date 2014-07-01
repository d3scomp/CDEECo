/*
 * FreeRTOSTask.h
 *
 *  Created on: 17. 5. 2014
 *      Author: Vladimír Matěna
 */

#ifndef FREERTOSTASK_H_
#define FREERTOSTASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/**
 * C++ wrapper for FreeRTOS task
 */
class FreeRTOSTask {
public:
	static const size_t DEFAULT_STACK_SIZE = 1024;
	static const unsigned long DEFAULT_PRIORITY = 1UL;

	FreeRTOSTask(size_t stackSize = DEFAULT_STACK_SIZE, unsigned long priority = DEFAULT_PRIORITY) {
		xTaskCreate(taskBody, "Task", stackSize, this, tskIDLE_PRIORITY + priority, &handle);
	}
	virtual ~FreeRTOSTask() {
		vTaskDelete(handle);
	}

	/**
	 * Code to be executed by task
	 */
	virtual void run() = 0;

	void mDelay(uint16_t ms) {
		vTaskDelay(ms / portTICK_PERIOD_MS);
	}

	void suspend() {
		vTaskSuspend(handle);
	}

private:
	/// Task handle used by FreeRTOS
	TaskHandle_t handle;

	/// FreeRTOS task entry point
	static void taskBody(void *data) {
		static_cast<FreeRTOSTask*>(data)->run();
	}
};

#endif /* FREERTOSTASK_H_ */
