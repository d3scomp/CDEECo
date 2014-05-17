/*
 * FreeRTOSMutex.h
 *
 *  Created on: 17. 5. 2014
 *      Author: vlada
 */

#ifndef FREERTOSMUTEX_H_
#define FREERTOSMUTEX_H_

#include "FreeRTOS.h"
#include "semphr.h"

class FreeRTOSMutex {
public:
	FreeRTOSMutex();
	void lock();
	void unlock();

private:
	SemaphoreHandle_t sem;
};

#endif /* FREERTOSMUTEX_H_ */
