/*
 * ListedTrigerredTask.h
 *
 *  Created on: 14.4. 2014
 *      Author: Vladimir Matěna
 */

#ifndef LISTEDTRIGGERTASK_H_
#define LISTEDTRIGGERTASK_H_

class ListedTriggerTask {
public:
	ListedTriggerTask *next;

	ListedTriggerTask(): next(NULL) {};
	virtual ~ListedTriggerTask() {};

	template <typename UPDATED_KNOWLEDGE>
	void checkTriggerCondition(UPDATED_KNOWLEDGE &updated) {
		checkTriggerConditionData(&updated, &updated + sizeof(updated));
	};

protected:
	virtual void checkTriggerConditionData(void *updateStart, void* updateEnd) = 0;
};

#endif // LISTEDTRIGGERTASK_H_
