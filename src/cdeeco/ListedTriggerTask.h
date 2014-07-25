/**
 * \ingroup cdeeco
 * @file ListedTriggerTask.h
 *
 * Interface for listing triggered tasks in linked list
 *
 * \date 14.4. 2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 */

#ifndef LISTEDTRIGGERTASK_H
#define LISTEDTRIGGERTASK_H

namespace CDEECO {
	/**
	 * Interface for listing triggered tasks
	 *
	 * This provide members that can be used to link a list of triggered tasks. An interface
	 * for conditional trigger execution is also included.
	 *
	 * \ingroup cdeeco
	 */
	class ListedTriggerTask {
	public:
		/// Pointer to next listed trigger task
		ListedTriggerTask *next;

		/**
		 * Construct linked trigger task
		 */
		ListedTriggerTask() :
				next(NULL) {
		}

		/**
		 * Check trigger condition on knowledge
		 *
		 * @tparam UPDATED_KNOWLEDGE Changed knowledge type
		 * @param updated Reference to update knowledge
		 */
		template<typename UPDATED_KNOWLEDGE>
		void checkTriggerCondition(const UPDATED_KNOWLEDGE &updated) {
			checkTriggerConditionData(&updated, &updated + sizeof(updated));
		}

	protected:
		/**
		 * Check trigger condition on knowledge pointers
		 *
		 * @param updateStart Start of updated region in the knowledge
		 * @param updateEnd End of updated region in the knowledge
		 */
		virtual void checkTriggerConditionData(const void *updateStart, const void* updateEnd) = 0;
	};
}

#endif // LISTEDTRIGGERTASK_H
