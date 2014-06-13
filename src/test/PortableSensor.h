/*
 * PortableSensor.h
 *
 *  Created on: 21. 5. 2014
 *      Author: Vladimír Matěna
 *
 *  CDEECO++ component providing temperature and humidity at given position.
 *
 */

#ifndef PORTABLE_SENSOR
#define PORTABLE_SENSOR

#include <array>
#include <random>
#include <climits>

#include "cdeeco/Component.h"
#include "drivers/SHT1x.h"

namespace PortableSensor {
	/// Sensor knowledge
	struct Knowledge: CDEECO::Knowledge {
		struct Position {
			float lat;
			float lon;
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
		Sense(auto &component, auto &out) :
				PeriodicTask(1800, component, out) {
			sensor.init();
		}

	private:
		SHT1x::Properties sensorProps {
		GPIOB, RCC_AHB1Periph_GPIOB, GPIO_Pin_8, GPIO_Pin_7 };
		SHT1x sensor = SHT1x(sensorProps);

		Knowledge::Value run(const Knowledge in) {
			float temp = sensor.readTemperature();
			float humid = sensor.readHumidity();
			console.print(TaskInfo, "\n\n\n>>>> Sensor task:\n");
			console.print(TaskInfo, ">>>>>> Temperature: %d.%d°C\n", (int16_t) temp, ((int16_t) (temp * 100) % 100));
			console.print(TaskInfo, ">>>>>> Rela. humid: %d.%d%%\n", (int16_t) humid, ((int16_t) (humid * 100) % 100));
			console.print(TaskInfo, ">>>>>> AlarmId: %x\n\n\n\n", in.coordId);

			return {temp, humid};
		}
	};

	/**
	 * Position task
	 */
	class Position: public CDEECO::PeriodicTask<Knowledge, Knowledge::Position> {
	public:
		Position(auto &component, auto &out) :
				PeriodicTask(1500, component, out) {
		}

	private:
		Knowledge::Position run(const Knowledge in) {
			GPSL10::GPSFix fix = gps.getGPSFix();

			console.print(TaskInfo, "\n\n\n>>>> Position task:\n");
			console.print(TaskInfo, "GPS: valid:%d, date:%d.%d.%d %d:%d:%d pos: ", fix.valid, fix.day, fix.month,
					fix.year, fix.hour, fix.minute, fix.second);
			console.printFloat(TaskInfo, fix.latitude, 6);
			console.print(TaskInfo, " ");
			console.printFloat(TaskInfo, fix.longitude, 6);
			console.print(TaskInfo, "\n\n\n\n");

			if(fix.valid)
				return {fix.latitude, fix.longitude};
			else
				return in.position;
		}
	};

	/**
	 * PortableThermometer component class
	 */
	class Component: public CDEECO::Component<Knowledge> {
	public:
		static const CDEECO::Type Type = 0x00000001;

		Sense sense = Sense(*this, this->knowledge.value);
		Position position = Position(*this, this->knowledge.position);

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
	struct KnowledgeTrait<PortableSensor::Knowledge> {
		static constexpr std::array<size_t, 1> offsets = { { offsetof(PortableSensor::Knowledge, position) } };
	};
	// This declaration do not require array size to be specified twice, but drives eclipse crazy.
	//constexpr decltype(KnowledgeTrait<TestKnowledge>::offsets) KnowledgeTrait<TestKnowledge>::offsets;
	constexpr std::array<size_t, 1> KnowledgeTrait<PortableSensor::Knowledge>::offsets;
}

#endif // PORTABLE_SENSOR_H
