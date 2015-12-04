/**
 * \ingroup wrappers
 * @file FreeRTOSTask.cpp
 *
 * C++ FreeRTOS task wrapper implementation
 *
 * \date 17. 5. 2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 *
 * @see FreeRTOSTask
 *
 */

#include "FreeRTOSTask.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

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

long FreeRTOSTask::getTickCount() {
	return xTaskGetTickCount();
}


