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

#include <assert.h>

#include "System.h"
#include "Knowledge.h"
#include "PeriodicTask.h"
#include "ListedTriggerTask.h"
#include "KnowledgeFragment.h"
#include "FreeRTOSMutex.h"

namespace CDEECO {

/** System component template */
template<typename KNOWLEDGE>
class Component {
public:
	Component(const KnowledgeFragment::Type type, const KnowledgeFragment::Id id, System &system) :
			type(type), id(id), system(system), rootTriggerTask(NULL) {
	}

	KNOWLEDGE lockReadKnowledge() {
		knowledgeMutex.lock();
		KNOWLEDGE in = knowledge;
		knowledgeMutex.unlock();

		return in;
	}

	template<typename OUT_KNOWLEDGE>
	void lockWriteKnowledge(OUT_KNOWLEDGE &outKnowledge, OUT_KNOWLEDGE knowledgeData) {
		assert((size_t)&outKnowledge >= (size_t)&knowledge
				&& (size_t)&outKnowledge + sizeof(OUT_KNOWLEDGE) <= (size_t)&knowledge + sizeof(KNOWLEDGE));

		knowledgeMutex.lock();
		// Update knowledge
		outKnowledge = knowledgeData;

		// Broadcast updated knowledge fragments
		broadcastChange(((size_t)&outKnowledge) - ((size_t)&knowledge), sizeof(OUT_KNOWLEDGE));

		knowledgeMutex.unlock();

		checkAndRunTriggeredTasks(outKnowledge);
	}

	void addTriggeredTask(ListedTriggerTask &task) {
		// Install new list head
		if(rootTriggerTask == NULL) {
			rootTriggerTask = &task;
			return;
		}

		// Add to the end of the list
		ListedTriggerTask *root = rootTriggerTask;
		while(root->next != NULL)
			root = root->next;
		root->next = &task;
	}

	/// Knowledge of the component
	KNOWLEDGE knowledge;

protected:
	/// Component type
	const uint32_t type;

	/// Component identification
	const uint32_t id;

private:
	System &system;
	FreeRTOSMutex knowledgeMutex;
	ListedTriggerTask *rootTriggerTask;

	template<typename T>
	void checkAndRunTriggeredTasks(T &changed) {
		for(ListedTriggerTask *task = rootTriggerTask; task != NULL; task = task->next) {
			task->checkTriggerCondition(changed);
		}
	}

	/**
	 * Broadcast knowledge change
	 *
	 * @param start Change start relative to knowledge
	 * @param size Change size
	 */
	void broadcastChange(size_t start, size_t size) {
		Console::log("Broadcasting change");

		size_t end = start + size;
		assert(end <= sizeof(KNOWLEDGE));

		// Until the change is broadcasted
		do {
			// Find start offset
			size_t brdStart = 0;
			bool found = false;
			for(int i = KnowledgeTrait<KNOWLEDGE>::offsets.size() - 1; i >= 0; --i) {
				if(KnowledgeTrait<KNOWLEDGE>::offsets[i] <= start) {
					found = true;
					brdStart = KnowledgeTrait<KNOWLEDGE>::offsets[i];
					break;
				}
			}
			assert(found);

			// Broadcast fragment
			start = brdStart + broadcastFragment(brdStart);
		} while (start < end);
	}

	/**
	 * Broadcast knowledge fragment
	 *
	 * @param start Start fragment offset
	 * @return Broadcasted knowledge data size
	 */
	size_t broadcastFragment(size_t start) {
		KnowledgeFragment fragment;
		fragment.id = id;
		fragment.type = type;
		fragment.offset = start;
		fragment.size = std::min(sizeof(KNOWLEDGE) - start, sizeof(fragment.data));
		memcpy(fragment.data, &((char*)&knowledge)[start], fragment.size);

		system.broadcastFragment(fragment);

		return fragment.size;
	}
};

}

#endif /* COMPONENT_H_ */
