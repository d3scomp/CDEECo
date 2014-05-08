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

#include "System.h"
#include "Knowledge.h"
#include "PeriodicTask.h"
#include "ListedTriggerTask.h"

/** System component template */
template<typename KNOWLEDGE>
class Component {
public:
	Component(const ComponentType type, const ComponentId id, System &system) :
			system(system), type(type), id(id), knowledgeSem(xSemaphoreCreateMutex()), rootTriggerTask(NULL) {
	}

	KNOWLEDGE lockReadKnowledge() {
		lockKnowledge();
		KNOWLEDGE in = knowledge;
		unlockKnowledge();

		return in;
	}

	template<typename OUT_KNOWLEDGE>
	void lockWriteKnowledge(OUT_KNOWLEDGE &outKnowledge, OUT_KNOWLEDGE knowledgeData) {
		lockKnowledge();
		outKnowledge = knowledgeData;
		unlockKnowledge();

		checkAndRunTriggeredTasks(outKnowledge);
	}

	void addTriggeredTask(ListedTriggerTask &task) {
		// Install new list head
		if (rootTriggerTask == NULL) {
			rootTriggerTask = &task;
			return;
		}

		// Add to the end of the list
		ListedTriggerTask *root = rootTriggerTask;
		while (root->next != NULL)
			root = root->next;
		root->next = &task;
	}

protected:
	/// Component type
	const uint32_t type;

	/// Component identification
	const uint32_t id;

	/// Knowledge of the component
	KNOWLEDGE knowledge;

private:
	System &system;
	SemaphoreHandle_t knowledgeSem;
	ListedTriggerTask *rootTriggerTask;

	void lockKnowledge() {
		xSemaphoreTake(knowledgeSem, portMAX_DELAY);
	}
	void unlockKnowledge() {
		xSemaphoreGive(knowledgeSem);
	}

	template<typename T>
	void checkAndRunTriggeredTasks(T &changed) {
		for (ListedTriggerTask *task = rootTriggerTask; task != NULL; task = task->next) {
			task->checkTriggerCondition(changed);
		}
	}
};

#endif /* COMPONENT_H_ */
