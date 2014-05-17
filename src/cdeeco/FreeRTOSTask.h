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
template <size_t STACK = 4096, unsigned long PRIORITY = tskIDLE_PRIORITY + 1UL>
class FreeRTOSTask {
public:
	FreeRTOSTask() {
		xTaskCreate(taskBody, "Task", STACK, this, PRIORITY, &handle);
	}
	virtual ~FreeRTOSTask() {
		vTaskDelete(handle);
	}

	/**
	 * Code to be executed by task
	 */
	virtual void run() = 0;

private:
	/// Task handle used by FreeRTOS
	TaskHandle_t handle;

	/// FreeRTOS task entry point
	static void taskBody(void *data) {
		static_cast<FreeRTOSTask<STACK, PRIORITY>*>(data)->run();
	}
};

#endif /* FREERTOSTASK_H_ */
