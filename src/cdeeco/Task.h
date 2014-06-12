/*
 * Task.h
 *
 *  Created on: 28. 3. 2014
 *      Author: Vladimir Matěna
 */

#ifndef TASK_H_
#define TASK_H_

#include "FreeRTOS.h"
#include "task.h"

#include <stdlib.h>

#include "Component.h"

namespace CDEECO {
	/** Prototype for component tasks */
	template<typename KNOWLEDGE, typename OUT_KNOWLEDGE>
	class TaskBase {
	public:
		TaskBase(auto &component) :
				component(component) {
		}

		/** Task user code */
		virtual OUT_KNOWLEDGE run(const KNOWLEDGE knowledge) = 0;

		const size_t DefaultStackSize = 4096;
		const unsigned long DefaultPriority = tskIDLE_PRIORITY + 1UL;

	protected:
		Component<KNOWLEDGE> &component;
		/** Task execution code, responsible for data passing */
		virtual void execute() = 0;
	};

	/** Prototype for component tasks */
	template<typename KNOWLEDGE, typename OUT_KNOWLEDGE>
	class Task: TaskBase<KNOWLEDGE, OUT_KNOWLEDGE> {
	public:
		Task(auto &component, auto &outKnowledge) :
				TaskBase<KNOWLEDGE, OUT_KNOWLEDGE>(component), outKnowledge(outKnowledge) {
		}

	private:
		OUT_KNOWLEDGE &outKnowledge;

	protected:
		/** Task execution code, responsible for data passing */
		void execute() {
			// Lock and copy input data
			KNOWLEDGE in = this->component.lockReadKnowledge();

			// Execute user code defined for the task
			OUT_KNOWLEDGE out = this->run(in);

			// Lock and copy output data
			this->component.lockWriteKnowledge(outKnowledge, out);
		}
	};

	/** Prototype for component tasks */
	template<typename KNOWLEDGE>
	class Task<KNOWLEDGE, void> : TaskBase<KNOWLEDGE, void> {
	public:
		Task(auto &component) :
				TaskBase<KNOWLEDGE, void>(component) {
		}

	protected:
		/** Task execution code, responsible for data passing */
		void execute() {
			// Lock and copy input data
			KNOWLEDGE in = this->component.lockReadKnowledge();

			// Execute user code defined for the task
			this->run(in);
		}
	};
}

#endif /* TASK_H_ */
