/*
 * Task.h
 *
 *  Created on: 28. 3. 2014
 *      Author: vlada
 */

#ifndef TASK_H_
#define TASK_H_

#include "FreeRTOS.h"
#include "task.h"

#include <stdlib.h>

/** Prototype for component tasks */
template<typename IN_KNOWLEDGE, typename OUT_KNOWLEDGE>
class Task {
public:
	Task(IN_KNOWLEDGE *inKnowledge, OUT_KNOWLEDGE *outKnowledge):
		inKnowledge(inKnowledge), outKnowledge(outKnowledge) {
	};
	virtual ~Task() {};

	/** Task user code */
	virtual OUT_KNOWLEDGE run(IN_KNOWLEDGE in) = 0;

	const size_t DefaultStackSize = 1024;
	const unsigned long DefaultPriority = tskIDLE_PRIORITY + 1UL;

private:
	IN_KNOWLEDGE *inKnowledge;
	OUT_KNOWLEDGE *outKnowledge;

protected:
	/** Task execution code, responsible for data passing */
	void execute() {
		// TODO: This should lock and copy input data
		IN_KNOWLEDGE in = *inKnowledge;

		// Execute user code defined for the task
		OUT_KNOWLEDGE out = run(in);

		// TODO: Lock and copy output data
	}
};

#endif /* TASK_H_ */
