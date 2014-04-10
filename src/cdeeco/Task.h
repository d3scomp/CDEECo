/*
 * Task.h
 *
 *  Created on: 28. 3. 2014
 *      Author: Vladimir Matěna
 */

#ifndef TASK_H_
#define TASK_H_

template<typename KNOWLEDGE>
class Component;

#include "FreeRTOS.h"
#include "task.h"

#include <stdlib.h>

/** Prototype for component tasks */
template<typename KNOWLEDGE, typename IN_KNOWLEDGE, typename OUT_KNOWLEDGE>
class Task {
public:
	Task(Component<KNOWLEDGE> &component, const IN_KNOWLEDGE &inKnowledge, OUT_KNOWLEDGE &outKnowledge):
		component(component), inKnowledge(inKnowledge), outKnowledge(outKnowledge) {
	};
	virtual ~Task() {};

	/** Task user code */
	virtual OUT_KNOWLEDGE run(IN_KNOWLEDGE in) = 0;

	const size_t DefaultStackSize = 4096;
	const unsigned long DefaultPriority = tskIDLE_PRIORITY + 1UL;

private:
	Component<KNOWLEDGE> &component;
	const IN_KNOWLEDGE &inKnowledge;
	OUT_KNOWLEDGE &outKnowledge;

protected:
	/** Task execution code, responsible for data passing */
	void execute() {
		// Lock and copy input data
		IN_KNOWLEDGE in = component.lockReadKnowledge(inKnowledge);

		// Execute user code defined for the task
		OUT_KNOWLEDGE out = run(in);

		// Lock and copy output data
		component.lockWriteKnowledge(outKnowledge, out);
	}
};

#endif /* TASK_H_ */
