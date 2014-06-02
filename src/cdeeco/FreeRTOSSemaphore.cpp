/*
 * FreeRTOSSemaphore.cpp
 *
 *  Created on: 2.6 2014
 *      Author: Vladimír Matěna
 */

#include "FreeRTOSSemaphore.h"

FreeRTOSSemaphore::FreeRTOSSemaphore(const size_t max) :
		sem(xSemaphoreCreateCounting(max, 0)) {
}

void FreeRTOSSemaphore::take() {
	xSemaphoreTake(sem, portMAX_DELAY);
}

void FreeRTOSSemaphore::give() {
	xSemaphoreGive(sem);
}

