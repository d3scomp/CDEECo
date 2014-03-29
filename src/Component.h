/*
 * Component.h
 *
 *  Created on: 25. 3. 2014
 *      Author: vlada
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_

#include "FreeRTOS.h"

#include "Knowledge.h"
#include "PeriodicTask.h"

/** System component template */
template<typename KNOWLEDGE>
class Component {
public:
	/// Task code
	typedef void (*TaskCode)(KNOWLEDGE);

	/// Task trigger definition
	typedef Knowledge* Trigger;

	/// Knowledge of the component
	KNOWLEDGE knowledge;

	Component() {}

protected:
	void createPeriodicTask(PeriodicTask<KNOWLEDGE, KNOWLEDGE> *task);
//	void createTriggeredTask(TaskCode *code, Trigger, size_t stackSize = DefaultStackSize);
};

#endif /* COMPONENT_H_ */
