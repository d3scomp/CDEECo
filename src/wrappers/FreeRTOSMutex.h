/*
 * FreeRTOSMutex.h
 *
 *  Created on: 17. 5. 2014
 *      Author: Vladimír Matěna
 *
 * FreeRTOS C++ wrapper for mutex.
 *
 */

#ifndef FREERTOSMUTEX_H
#define FREERTOSMUTEX_H

#include "FreeRTOS.h"
#include "semphr.h"

/**
 * C++ wrapper for FreeRTOS mutex
 */
class FreeRTOSMutex {
public:
	/**
	 * FreeRTOS mutex constructor
	 *
	 * Takes no parameters. Creates new FreeRTOS mutex. FreeRTOS system will allocate memory for the mutex structures.
	 */
	FreeRTOSMutex();

	/**
	 * Lock the mutex
	 *
	 * This will block if the mutex is already locked.
	 */
	void lock();

	/**
	 * Unlock the mutex
	 */
	void unlock();

private:
	/**
	 * FreeRTOS semaphore handle
	 *
	 * Used for mutex implementation.
	 */
	SemaphoreHandle_t sem;
};

#endif // FREERTOSMUTEX_H
