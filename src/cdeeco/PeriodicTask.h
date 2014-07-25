/**
 * \ingroup cdeeco
 * @file PeriodicTask.h
 *
 * CDEECo++ base periodic task implementation
 *
 * \date 28. 3. 2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 */

#ifndef PERIODICTASK_H
#define PERIODICTASK_H

#include "FreeRTOS.h"
#include "task.h"

#include "Task.h"
#include "Console.h"
#include "wrappers/FreeRTOSTask.h"

namespace CDEECO {
	/**
	 * Periodic task implementation with output knowledge
	 *
	 * @tparam KNOWLEDGE Type of the this task's component's knowledge
	 * @tparam OUT_KNOWLEDGE Type of this task output knowledge
	 *
	 * \ingroup cdeeco
	 */
	template<typename KNOWLEDGE, typename OUT_KNOWLEDGE>
	class PeriodicTask: Task<KNOWLEDGE, OUT_KNOWLEDGE>, FreeRTOSTask {
	public:
		/**
		 * Create the periodic task
		 *
		 * @param period task execution period in milliseconds
		 * @param outKnowledge Reference to output knowledge.
		 * 			Output knowledge should be member of component's knowledge.
		 * @param stack Stack size of the task
		 * @param priority Task priority
		 */
		PeriodicTask(long period, auto &component, auto &outKnowledge, size_t stack = FreeRTOSTask::DEFAULT_STACK_SIZE,
				unsigned long priority = FreeRTOSTask::DEFAULT_PRIORITY) :
				Task<KNOWLEDGE, OUT_KNOWLEDGE>(component, outKnowledge), FreeRTOSTask(stack, priority), period(period) {
			console.print(Debug, ">> PeriodicTask constructor\n");
		}

		/**
		 * Create the periodic task
		 *
		 * @param period task execution period in milliseconds
		 * @param stack Stack size of the task
		 * @param priority Task priority
		 */
		PeriodicTask(long period, auto &component, size_t stack = FreeRTOSTask::DEFAULT_STACK_SIZE,
				unsigned long priority = FreeRTOSTask::DEFAULT_PRIORITY) :
				Task<KNOWLEDGE, OUT_KNOWLEDGE>(component), FreeRTOSTask(stack, priority), period(period) {
			console.print(Debug, ">> PeriodicTask constructor\n");
		}

	private:
		/// Period of this task in milliseconds
		long period;

		/**
		 * Periodic task body implementation
		 *
		 * Responsible for periodic scheduling.
		 */
		void run() {
			// Schedule the task periodically
			while(1) {
				// Run the task
				this->execute();

				// Wait for next execution time
				vTaskDelay(this->period / portTICK_PERIOD_MS);
			}
		}
	};
}

#endif // PERIODICTASK_H
