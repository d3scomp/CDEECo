/*
 * FreeRTOSSemaphore.h
 *
 *  Created on: 2.6 2014
 *      Author: Vladimír Matěna
 *
 * @see FreeRTOSSemaphore
 *
 */

#ifndef FREE_RTOS_SEMAPHORE_H
#define FREE_RTOS_SEMAPHORE_H

#include "FreeRTOS.h"
#include "semphr.h"

/**
 * C++ wrapper for FreeRTOS semaphore
 */
class FreeRTOSSemaphore {
public:
	/**
	 * Create FreeRTOS semaphore
	 *
	 * FreeRTOS will allocate memory for smaphore structures.
	 *
	 * @param max Maximum semaphore value
	 * @param start Starting semaphore value
	 */
	FreeRTOSSemaphore(const size_t max, const size_t start = 0);

	/**
	 * Take semaphore
	 *
	 * This will block if the semaphore is already at 0.
	 */
	void take();

	/**
	 * Give semaphore
	 *
	 * This will block if the semaphore already at maximum value.
	 */
	void give();

	/**
	 * Give semaphore from ISR
	 *
	 * Special version for giving the semaphore from interrupt servicing routine.
	 *
	 * @see give
	 */
	void giveFromISR();
private:
	/// FreeRTOS semaphore handle
	SemaphoreHandle_t sem;
};

#endif // FREE_RTOS_SEMAPHORE_H

