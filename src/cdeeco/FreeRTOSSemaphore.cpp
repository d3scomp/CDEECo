/*
 * FreeRTOSSemaphore.cpp
 *
 *  Created on: 2.6 2014
 *      Author: Vladimír Matěna
 */

#include "FreeRTOSSemaphore.h"

FreeRTOSSemaphore::FreeRTOSSemaphore(const size_t max, const size_t start) :
		sem(xSemaphoreCreateCounting(max, start)) {
}

void FreeRTOSSemaphore::take() {
	xSemaphoreTake(sem, portMAX_DELAY);
}

void FreeRTOSSemaphore::give() {
	xSemaphoreGive(sem);
}

void FreeRTOSSemaphore::giveFromISR() {
	signed long xHigherPriorityTaskWoken;
	xSemaphoreGiveFromISR(sem, &xHigherPriorityTaskWoken);
}

