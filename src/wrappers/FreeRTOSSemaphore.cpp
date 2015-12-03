/**
 * \ingroup wrappers
 * @file FreeRTOSSemaphore.cpp
 *
 * C++ FreeRTOS semaphore wrapper implementation
 *
 * Implementation for FreeRTOS semaphore wrapper. This contains trivial mapping to FreeRTOS API calls.
 *
 * \date 2.6 2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 *
 * @see FreeRTOSSemaphore
 *
 */

#include "FreeRTOSSemaphore.h"

#include "FreeRTOS.h"
#include "semphr.h"

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
	// TODO: Do possible yield on ISR end
	signed long xHigherPriorityTaskWoken;
	xSemaphoreGiveFromISR(sem, &xHigherPriorityTaskWoken);
}
