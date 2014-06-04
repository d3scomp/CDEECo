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
#include "drivers/SHT1x.h"

namespace Thermometer {

/// Thermometer knowledge
struct Knowledge: CDEECO::Knowledge {
	struct Position {
		int x;
		int y;
	} position;

	float temperature;
};

/// Temperature measure task
class Temp: public CDEECO::PeriodicTask<Knowledge, float> {
public:
	Temp(CDEECO::Component<Knowledge> &component, float &out) :
			PeriodicTask(1800, component, out), sensor(sensorProps) {
		sensor.init();
	}

protected:
	SHT1x::Properties sensorProps {
		GPIOB, RCC_AHB1Periph_GPIOB, GPIO_Pin_8, GPIO_Pin_7
	};
	SHT1x sensor;

	float run(const Knowledge in) {
		float temp = sensor.readTemperature();
		Console::log("\n\n\nTemp: %d.%d°C\n\n\n\n", (int16_t)temp, ((int16_t)(temp * 100) % 100));
		return temp;
	}
};

/**
 * PortableThermometer component class
 */
class Component: public CDEECO::Component<Knowledge> {
public:
	static const KnowledgeFragment::Type Type = 0x00000001;
	Temp temp;

	Component(CDEECO::System &system) :
			CDEECO::Component<Knowledge>(Type, system), temp(*this, this->knowledge.temperature) {
		// Initialize knowledge
		memset(&knowledge, 0, sizeof(Knowledge));
	}
};

}

namespace CDEECO {

/// Allowed offsets to guarantee knowledge consistency
template<>
struct KnowledgeTrait<Thermometer::Knowledge> {
	static constexpr std::array<size_t, 1> offsets = { { offsetof(Thermometer::Knowledge, position) } };
};
// This declaration do not require array size to be specified twice, but drives eclipse crazy.
//constexpr decltype(KnowledgeTrait<TestKnowledge>::offsets) KnowledgeTrait<TestKnowledge>::offsets;
constexpr std::array<size_t, 1> KnowledgeTrait<Thermometer::Knowledge>::offsets;

}

#endif // PORTABLE_THERMOMETER_H_
