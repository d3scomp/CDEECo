/*
 * PortableThermometer.h
 *
 *  Created on: 21. 5. 2014
 *      Author: Vladimír Matěna
 *
 *  CDEECO++ component providing temperature at given position.
 *
 */

#ifndef PORTABLE_THERMOMETER_H_
#define PORTABLE_THERMOMETER_H_

#include <array>
#include <random>

#include "cdeeco/Component.h"

/// Portable thermometer knowledge
struct PortableThermometerKnowledge: Knowledge {
	struct Position {
		int x;
		int y;
	} position;

	float temperature;
};

/// Allowed offsets to guarantee knowledge consistency
template<>
struct KnowledgeTrait<PortableThermometerKnowledge> {
	static constexpr std::array<size_t, 1> offsets = { { offsetof(PortableThermometerKnowledge, position) } };
};
// This declaration do not require array size to be specified twice, but drives eclipse crazy.
//constexpr decltype(KnowledgeTrait<TestKnowledge>::offsets) KnowledgeTrait<TestKnowledge>::offsets;
constexpr std::array<size_t, 1> KnowledgeTrait<PortableThermometerKnowledge>::offsets;

/**
 * PortableThermometer component class
 */
class PortableThermometer: public Component<PortableThermometerKnowledge> {
private:
	class Temp: public PeriodicTask<PortableThermometerKnowledge, float> {
	public:
		Temp(Component<PortableThermometerKnowledge> &component, float &out) :
				PeriodicTask(2000, component, out) {
		}

	protected:
		float run(const PortableThermometerKnowledge in) {
			std::default_random_engine gen;
			return in.temperature + gen() % 5;
		}
	};

public:
	ComponentType Type = 0x00000001;
	Temp temp;

	PortableThermometer(System &system, ComponentId id) :
			Component<PortableThermometerKnowledge>(1, id, system), temp(*this, this->knowledge.temperature) {
		// Initialize knowledge
		memset(&knowledge, 0, sizeof(PortableThermometerKnowledge));
	}
};

#endif // PORTABLE_THERMOMETER_H_
