/*
 * FreeRTOSMutex.cpp
 *
 *  Created on: 17. 5. 2014
 *      Author: vlada
 */

#include "FreeRTOSMutex.h"

#include "Console.h"

FreeRTOSMutex::FreeRTOSMutex() :
		sem(xSemaphoreCreateMutex()) {
}

void FreeRTOSMutex::lock() {
	xSemaphoreTake(sem, portMAX_DELAY);
}

void FreeRTOSMutex::unlock() {
	xSemaphoreGive(sem);
}

