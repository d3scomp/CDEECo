/**
 * \ingroup cdeeco
 * @file Component.h
 *
 * CDEECo++ base component implemnetation
 *
 * \date 25. 3. 2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 */

#ifndef COMPONENT_H
#define COMPONENT_H

#include "FreeRTOS.h"
#include "semphr.h"

#include <climits>

#include "System.h"
#include "Knowledge.h"
#include "ListedTriggerTask.h"
#include "KnowledgeFragment.h"
#include "wrappers/FreeRTOSMutex.h"

namespace CDEECO {
	/**
	 * Component class template
	 *
	 * @tparam KNOWLEDGE Component's knowledge type
	 *
	 * \ingroup cdeeco
	 */
	template<typename KNOWLEDGE>
	class Component: FreeRTOSTask {
	public:
		/**
		 * Component constructor
		 *
		 * @param id Id of the component instance
		 * @param type Component's magic (identifies knowledge in packets)
		 * @param broadcaster Reference to broadcaster to broadcast component's knowledge
		 * @param broadcastPeriodMs Knowledge broadcast period in milliseconds
		 */
		Component(const CDEECO::Id id, const CDEECO::Type type, Broadcaster &broadcaster,
				const uint32_t broadcastPeriodMs = 3000) :
				id(id), type(type), broadcaster(broadcaster), rootTriggerTask(NULL), broadcastPeriodMs(
						broadcastPeriodMs) {
		}

		/**
		 * Lock knowledge and obtain copy of it
		 *
		 * @return Consistent copy of components knowledge
		 */
		KNOWLEDGE lockReadKnowledge() {
			knowledgeMutex.lock();
			KNOWLEDGE in = knowledge;
			knowledgeMutex.unlock();

			return in;
		}

		/**
		 * Lock knowledge and update it
		 *
		 * @tparam OUT_KNOWLEDGE Type of knowledge being written
		 * @param outKnowledge Reference to output knowledge to be written
		 * @param knowledgeData KNowedlge data to consistently write into outKnowledge
		 */
		template<typename OUT_KNOWLEDGE>
		void lockWriteKnowledge(OUT_KNOWLEDGE &outKnowledge, const OUT_KNOWLEDGE knowledgeData) {
			assert_param(
					(size_t) &outKnowledge >= (size_t) &knowledge
							&& (size_t) &outKnowledge + sizeof(OUT_KNOWLEDGE)
									<= (size_t) &knowledge + sizeof(KNOWLEDGE));

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

		/**
		 * Add triggerd task to this component
		 *
		 * Component will call checkTriggerCOndition on this task when knowledge changes
		 *
		 * @param task Task to add
		 */
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

		/**
		 * Get component's id
		 *
		 * @return Id of the component
		 */
		Id getId() {
			return id;
		}

		/**
		 * Get component type
		 *
		 * @return Component's type
		 */
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
		/**
		 * Reference to this component's broadcaster
		 *
		 * Used to broadcast component's knowledge.
		 */
		Broadcaster &broadcaster;
		/// Knowledge access mutex
		FreeRTOSMutex knowledgeMutex;
		/// Root of linked list of triggered tasks
		ListedTriggerTask *rootTriggerTask;
		/// Interval between knowledge broadcasts
		const uint32_t broadcastPeriodMs;

		/**
		 * Run triggered tasks
		 *
		 * Run tasks that are triggered by specified change in the knowledge.
		 *
		 * @tparam T Type of changed knowledge member
		 * @param changed Reference to changed knowledge member
		 */
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
			console.print(Debug, "Broadcasting local knowledge\n");

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

			broadcaster.broadcastFragment(fragment);

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

#endif // COMPONENT_H
