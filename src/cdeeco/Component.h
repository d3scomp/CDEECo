/*
 * Component.h
 *
 *  Created on: 25. 3. 2014
 *      Author: Vladimir Matěna
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

	template <typename IN_KNOWLEDGE>
	IN_KNOWLEDGE lockReadKnowledge(IN_KNOWLEDGE &inKnowledge) {
		lockKnowledge();
		IN_KNOWLEDGE in = inKnowledge;
		unlockKnowledge();

		return in;
	}

	template <typename OUT_KNOWLEDGE>
	void lockWriteKnowledge(OUT_KNOWLEDGE &outKnowledge, OUT_KNOWLEDGE knowledgeData) {
		lockKnowledge();
		// TODO: Check and run triggered tasks
		outKnowledge = knowledgeData;
		unlockKnowledge();
	}

protected:
	/// Knowledge of the component
	KNOWLEDGE knowledge;

private:
	SemaphoreHandle_t knowledgeSem;

	void lockKnowledge() {
		xSemaphoreTake(knowledgeSem, portMAX_DELAY);
	}
	void unlockKnowledge() {
		xSemaphoreGive(knowledgeSem);
	}
};

#endif /* COMPONENT_H_ */
