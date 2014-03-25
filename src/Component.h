/*
 * Component.h
 *
 *  Created on: 25. 3. 2014
 *      Author: vlada
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_

#include "FreeRTOS.h"

#include "knowledge.h"

/** System component template */
template<class KNOWLEDGE>
class Component {
public:
	/// Task code
	typedef void (*TaskCode)(KNOWLEDGE);

	/// Task trigger definition
	typedef Knowledge* Trigger;

	/// Knowledge of the component
	KNOWLEDGE knowledge;

	Component();

private:
	/// Default task stack size
	static const size_t DefaultStackSize = 512;

	/// Parameters to carry periodic task information
	struct PerodicTaskParams {
		long period;
		TaskCode *code;
	};

	/// Parameters to carry triggered task information
	struct TrigerredTaskParams {
		TaskCode *code;
	};

	void periodicTaskBody(PerodicTaskParams *params);
	void triggeredTaskBody(TrigerredTaskParams *params);

protected:
	void createPeriodicTask(TaskCode *code, size_t stackSize = DefaultStackSize);
	void createTriggeredTask(TaskCode *code, Trigger, size_t stackSize = DefaultStackSize);
};

#endif /* COMPONENT_H_ */
