/*
 * FreeRTOSTask.cpp
 *
 *  Created on: 2. 7. 2014
 *      Author: Vladimír Matěna
 */

#include "FreeRTOSTask.h"

FreeRTOSTask::FreeRTOSTask(size_t stackSize, unsigned long priority) {
	xTaskCreate(taskBody, "Task", stackSize, this, tskIDLE_PRIORITY + priority, &handle);
}

FreeRTOSTask::~FreeRTOSTask() {
	vTaskDelete(handle);
}

void FreeRTOSTask::mDelay(uint16_t ms) {
	vTaskDelay(ms / portTICK_PERIOD_MS);
}

void FreeRTOSTask::suspend() {
	vTaskSuspend(handle);
}
void FreeRTOSTask::taskBody(void *data) {
	static_cast<FreeRTOSTask*>(data)->run();
}
