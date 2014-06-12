/*
 * PortableSensor.h
 *
 *  Created on: 21. 5. 2014
 *      Author: Vladimír Matěna
 *
 *  CDEECO++ component providing temperature and humidity at given position.
 *
 */

#ifndef PORTABLE_THERMOMETER_H_
#define PORTABLE_THERMOMETER_H_

#include <array>
#include <random>
#include <climits>

#include "cdeeco/Component.h"
#include "drivers/SHT1x.h"

namespace Sensor {
	/// Sensor knowledge
	struct Knowledge: CDEECO::Knowledge {
		struct Position {
			int x;
			int y;
		} position;

		// Coordinator identification
		typedef CDEECO::Id CoordId;
		static const CoordId NO_COORD = ULONG_MAX;
		CoordId coordId;

		// Values
		struct Value {
			float temperature;
			float humidity;
		} value;
	};

	/// Sensor task
	class Sense: public CDEECO::PeriodicTask<Knowledge, Knowledge::Value> {
	public:
		Sense(CDEECO::Component<Knowledge> &component, Knowledge::Value &out) :
				PeriodicTask(1800, component, out), sensor(sensorProps) {
			sensor.init();
		}

	protected:
		SHT1x::Properties sensorProps {
		GPIOB, RCC_AHB1Periph_GPIOB, GPIO_Pin_8, GPIO_Pin_7 };
		SHT1x sensor;

		Knowledge::Value run(const Knowledge in) {
			float temp = sensor.readTemperature();
			float humid = sensor.readHumidity();

			Console::print(TaskInfo, "\n\n\n>>>> Sensor task:\n");
			Console::print(TaskInfo, ">>>>>> Temperature: %d.%d°C\n", (int16_t) temp, ((int16_t) (temp * 100) % 100));
			Console::print(TaskInfo, ">>>>>> Rela. humid: %d.%d%%\n", (int16_t) humid, ((int16_t) (humid * 100) % 100));
			Console::print(TaskInfo, ">>>>>> AlarmId: %d\n\n\n\n", in.coordId);

			return {temp, humid};
		}
	};

	/**
	 * PortableThermometer component class
	 */
	class Component: public CDEECO::Component<Knowledge> {
	public:
		static const CDEECO::Type Type = 0x00000001;
		Sense sense = Sense(*this, this->knowledge.value);

		Component(CDEECO::System &system, const CDEECO::Id id) :
				CDEECO::Component<Knowledge>(id, Type, system) {
			// Initialize knowledge
			memset(&knowledge, 0, sizeof(Knowledge));
		}
	};

}

namespace CDEECO {
	/// Allowed offsets to guarantee knowledge consistency
	template<>
	struct KnowledgeTrait<Sensor::Knowledge> {
		static constexpr std::array<size_t, 1> offsets = { { offsetof(Sensor::Knowledge, position) } };
	};
	// This declaration do not require array size to be specified twice, but drives eclipse crazy.
	//constexpr decltype(KnowledgeTrait<TestKnowledge>::offsets) KnowledgeTrait<TestKnowledge>::offsets;
	constexpr std::array<size_t, 1> KnowledgeTrait<Sensor::Knowledge>::offsets;
}

#endif // PORTABLE_THERMOMETER_H_
