/*
 * Task.h
 *
 *  Created on: 28. 3. 2014
 *      Author: Vladimir Matěna
 */

#ifndef TASK_H
#define TASK_H

#include "FreeRTOS.h"
#include "task.h"

#include <stdlib.h>

#include "Component.h"

namespace CDEECO {
	/**
	 * Common base for the task implementation
	 *
	 * This class contains common code for the standard and read-only (void output type) task.
	 *
	 * @tparam KNOWLEDGE Type of component's knowledge this task it part of
	 */
	template<typename KNOWLEDGE, typename OUT_KNOWLEDGE>
	class TaskBase {
	public:
		/**
		 * Construct base task
		 *
		 * @param component Reference to task's component
		 */
		TaskBase(auto &component) :
				component(component) {
		}

		/**
		 * Task suer code
		 *
		 * To be implemented by user defined task
		 *
		 * @param knowledge Input copy of component's knowledge
		 * @return Output knowledge to be written to the component's knowledge
		 *
		 */
		virtual OUT_KNOWLEDGE run(const KNOWLEDGE knowledge) = 0;

		/// Default stack size for the task
		const size_t DEFAULT_STACK_SIZE = 4096;
		/// Default task priority
		const unsigned long DEFAULT_PRIORITY = tskIDLE_PRIORITY + 1UL;

	protected:
		/// Reference to task's component
		Component<KNOWLEDGE> &component;
		/**
		 * Execute the task now
		 *
		 * Responsible for data passing
		 */
		virtual void execute() = 0;
	};

	/**
	 * Component's tasks with output
	 *
	 * This is task code for task that has output (output type is non void)
	 *
	 * @tparam KNOWLEDGE Type of component's knowledge this task it part of
	 * @tparam OUT_KNOWLEDGE Type of tasks's output knowledge
	 */
	template<typename KNOWLEDGE, typename OUT_KNOWLEDGE>
	class Task: TaskBase<KNOWLEDGE, OUT_KNOWLEDGE> {
	public:
		/**
		 * Task with output constructor
		 *
		 * @param component Task's component reference
		 * @param outKnowledge Reference to output knowledge in the component
		 */
		Task(auto &component, auto &outKnowledge) :
				TaskBase<KNOWLEDGE, OUT_KNOWLEDGE>(component), outKnowledge(outKnowledge) {
		}

	private:
		/// Reference to output knowledge in the component
		OUT_KNOWLEDGE &outKnowledge;

	protected:
		/**
		 * Execute the task now
		 *
		 * This method is responsible for task execution and passing data to and from user method.
		 */
		void execute() {
			// Lock and copy input data
			KNOWLEDGE in = this->component.lockReadKnowledge();

			// Execute user code defined for the task
			OUT_KNOWLEDGE out = this->run(in);

			// Lock and copy output data
			this->component.lockWriteKnowledge(outKnowledge, out);
		}
	};

	/**
	 * Component's tasks without output
	 *
	 * This is task code for task that has no output (output type is void)
	 *
	 * @tparam KNOWLEDGE Type of component's knowledge this task it part of
	 */
	template<typename KNOWLEDGE>
	class Task<KNOWLEDGE, void> : TaskBase<KNOWLEDGE, void> {
	public:
		/**
		 * Task with output constructor
		 *
		 * @param component Task's component reference
		 */
		Task(auto &component) :
				TaskBase<KNOWLEDGE, void>(component) {
		}

	protected:
		/**
		 * Execute the task now
		 *
		 * This method is responsible for task execution and passing data to user method.
		 */
		void execute() {
			// Lock and copy input data
			KNOWLEDGE in = this->component.lockReadKnowledge();

			// Execute user code defined for the task
			this->run(in);
		}
	};
}

#endif /* TASK_H */
