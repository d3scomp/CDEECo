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

#include <climits>

#include "System.h"
#include "Knowledge.h"
#include "ListedTriggerTask.h"
#include "KnowledgeFragment.h"
#include "FreeRTOSMutex.h"

namespace CDEECO {
	/** System component template */
	template<typename KNOWLEDGE>
	class Component: FreeRTOSTask {
	public:
		Component(const CDEECO::Id id, const CDEECO::Type type, System &system, const uint32_t broadcastPeriodMs = 3000) :
				id(id), type(type), system(system), rootTriggerTask(NULL), broadcastPeriodMs(broadcastPeriodMs) {
		}

		KNOWLEDGE lockReadKnowledge() {
			knowledgeMutex.lock();
			KNOWLEDGE in = knowledge;
			knowledgeMutex.unlock();

			return in;
		}

		template<typename OUT_KNOWLEDGE>
		void lockWriteKnowledge(OUT_KNOWLEDGE &outKnowledge, OUT_KNOWLEDGE knowledgeData) {
			assert_param(
					(size_t )&outKnowledge >= (size_t )&knowledge
							&& (size_t )&outKnowledge + sizeof(OUT_KNOWLEDGE)
									<= (size_t )&knowledge + sizeof(KNOWLEDGE));

			knowledgeMutex.lock();

			// Check and update knowledge
			bool changed = memcmp(&outKnowledge, &knowledgeData, sizeof(OUT_KNOWLEDGE)) != 0;
			if(changed) {
				// Update knowledge
				outKnowledge = knowledgeData;

				// Broadcast updated knowledge fragments
				broadcastChange(((size_t) &outKnowledge) - ((size_t) &knowledge), sizeof(OUT_KNOWLEDGE));
			}

			knowledgeMutex.unlock();

			if(changed)
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

		Id getId() {
			return id;
		}

		Type getType() {
			return type;
		}

		/// Knowledge of the component
		KNOWLEDGE knowledge;

	protected:
		/// Component Id
		const Id id;
		/// Component type
		const Type type;

	private:
		System &system;
		FreeRTOSMutex knowledgeMutex;
		ListedTriggerTask *rootTriggerTask;
		const uint32_t broadcastPeriodMs;

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
			console.log("Broadcasting local knowledge\n");

			size_t end = start + size;
			assert_param(end <= sizeof(KNOWLEDGE));

			// Until the change is broadcasted
			size_t cnt = 0;
			size_t last = ULONG_MAX;
			do {
				// Find start offset
				size_t brdStart = 0;

				// If allowed offsets are defined then use only them
				if(!KnowledgeTrait<KNOWLEDGE>::offsets.empty()) {
					bool found = false;
					for(size_t i = KnowledgeTrait<KNOWLEDGE>::offsets.size() - 1; i >= 0; --i) {
						if(KnowledgeTrait<KNOWLEDGE>::offsets[i] <= start) {
							assert_param(last != i);
							last = i;
							found = true;
							brdStart = KnowledgeTrait<KNOWLEDGE>::offsets[i];
							break;
						}
					}
					assert_param(found);

					// If allowed offsets are not defined then broadcast at will
				} else {
					brdStart = start;
				}

				// Check and notify
				console.print(Debug, ">>>> Created fragment %d of the changed knowledge\n", cnt++);

				// Broadcast fragment
				start = brdStart + broadcastFragment(brdStart);
			} while(start < end);
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
			memcpy(fragment.data, &((char*) &knowledge)[start], fragment.size);

			system.broadcastFragment(fragment);

			/* TODO: This is local loop-back for registering fragment from local components. Doing this makes no
			 *  sense in real application. */
			system.storeFragment(fragment);

			return fragment.size;
		}

		/**
		 * Periodic full knowledge broadcast
		 */
		void run() {
			while(true) {
				// Wait for next execution time
				vTaskDelay(this->broadcastPeriodMs / portTICK_PERIOD_MS);
				broadcastChange(0, sizeof(KNOWLEDGE));
			}
		}
	};
}

#endif /* COMPONENT_H_ */
