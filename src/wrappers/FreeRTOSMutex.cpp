/*
 * FreeRTOSMutex.cpp
 *
 * Created on: 17. 5. 2014
 * 		Author: Vladimír Matěna
 *
 * This file includes FreeRTOS wrapper for mutex implementation. It contains only trivial calls to FreeRTOS API.
 *
 */

#include "FreeRTOSMutex.h"

FreeRTOSMutex::FreeRTOSMutex() :
		sem(xSemaphoreCreateMutex()) {
}

void FreeRTOSMutex::lock() {
	xSemaphoreTake(sem, portMAX_DELAY);
}

void FreeRTOSMutex::unlock() {
	xSemaphoreGive(sem);
}
