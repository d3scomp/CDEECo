/*
 * Component.h
 *
 *  Created on: 25. 3. 2014
 *      Author: vlada
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_

#include "FreeRTOS.h"
#include "semphr.h"

#include "Knowledge.h"
#include "PeriodicTask.h"

/** System component template */
template<typename KNOWLEDGE>
class Component {
public:
	Component(): knowledgeSem(xSemaphoreCreateMutex()) {}

	void lockKnowledge() {
		xSemaphoreTake(knowledgeSem, portMAX_DELAY);
	}
	void unlockKnowledge() {
		xSemaphoreGive(knowledgeSem);
	}

protected:
	/// Knowledge of the component
	KNOWLEDGE knowledge;

private:
	SemaphoreHandle_t knowledgeSem;
};

#endif /* COMPONENT_H_ */
