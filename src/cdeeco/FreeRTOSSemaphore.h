/*
 * FreeRTOSSemaphore.h
 *
 *  Created on: 2.6 2014
 *      Author: Vladimír Matěna
 */

#ifndef FREE_RTOS_SEMAPHORE_H_
#define FREE_RTOS_SEMAPHORE_H_

#include "FreeRTOS.h"
#include "semphr.h"

class FreeRTOSSemaphore {
public:
	FreeRTOSSemaphore(const size_t max, const size_t start = 0);
	void take();
	void give();
	void giveFromISR();
private:
	SemaphoreHandle_t sem;
};

#endif // FREE_RTOS_SEMAPHORE_H_

