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
	FreeRTOSTask(size_t stackSize = 2048, unsigned long priority = 1UL) {
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
