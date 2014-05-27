/*
 * PortableThermometer.h
 *
 *  Created on: 21. 5. 2014
 *      Author: Vladimír Matěna
 *
 *  CDEECO++ component monitoring temperature provided by Thermometers
 *
 */

#ifndef ALARM_H_
#define ALARM_H_

#include <array>
#include <algorithm>

#include "cdeeco/Component.h"

namespace Alarm {

/// Portable thermometer knowledge
struct Knowledge: CDEECO::Knowledge {
	struct Position {
		int x;
		int y;
	} position;

	struct Temp {
		KnowledgeFragment::Id id;
		float temp;
	};

	typedef std::array<Temp, 10> Temps;

	Temps tempsNearby;

	bool tempCritical;
};

/// Temperature check task
class Check: public CDEECO::PeriodicTask<Knowledge, bool> {
public:
	Check(CDEECO::Component<Knowledge> &component, bool &out) :
			PeriodicTask(3000, component, out) {
	}

protected:
	bool run(const Knowledge in) {
		// Check temperatures for dangerous conditions
		const float threshold = 50.0f;

		for(size_t i = 0 ; i < in.tempsNearby.size(); ++i)
			if(in.tempsNearby[i].temp > threshold)
				return true;

		return false;
	}
};

/**
 * PortableThermometer component class
 */
class Component: public CDEECO::Component<Knowledge> {
public:
	KnowledgeFragment::Type Type = 0x00000002;
	Check check;

	Component(System &system, KnowledgeFragment::Id id) :
			CDEECO::Component<Knowledge>(Type, id, system), check(*this, this->knowledge.tempCritical) {
		// Initialize knowledge
		memset(&knowledge, 0, sizeof(Knowledge));
	}
};

}

namespace CDEECO {

/// Allowed offsets to guarantee knowledge consistency
template<>
struct KnowledgeTrait<Alarm::Knowledge> {
	static constexpr std::array<size_t, 1> offsets = { { offsetof(Alarm::Knowledge, position) } };
};
// This declaration do not require array size to be specified twice, but drives eclipse crazy.
//constexpr decltype(KnowledgeTrait<TestKnowledge>::offsets) KnowledgeTrait<TestKnowledge>::offsets;
constexpr std::array<size_t, 1> KnowledgeTrait<Alarm::Knowledge>::offsets;

}

#endif // PORTABLE_THERMOMETER_H_
