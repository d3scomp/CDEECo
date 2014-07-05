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

/**
 * Portable sensor component
 */
namespace PortableSensor {
	/**
	 * Portable sensor knowledge
	 */
	struct Knowledge: CDEECO::Knowledge {
		/// Sensor position
		struct Position {
			float lat;
			float lon;
		} position;

		/// Coordinator identification
		typedef CDEECO::Id CoordId;
		/// Coordinator value meaning there is no coordinator
		static const CoordId NO_COORD = ULONG_MAX;
		/// Sensor's coordinator
		CoordId coordId;

		/// Local sensor values
		struct Value {
			float temperature;
			float humidity;
		} value;
	};

	/// Sensor value gather task
	class Sense: public CDEECO::PeriodicTask<Knowledge, Knowledge::Value> {
	public:
		/**
		 * Sensor task constructor
		 *
		 * @param component Task's component
		 */
		Sense(auto &component) :
				PeriodicTask(1800, component, component.knowledge.value) {
			sensor.init();
		}

	private:
		/// SHT1x sensor properties
		SHT1x::Properties sensorProps {
			GPIOB, RCC_AHB1Periph_GPIOB, GPIO_Pin_8, GPIO_Pin_7
		};
		/// SHT1x sensor driver
		SHT1x sensor = SHT1x(sensorProps);

		/**
		 * Sensor task code
		 *
		 * @param in Copy of component's knowledge
		 * @return Sensor values (task output)
		 */
		Knowledge::Value run(const Knowledge in) {
			float temp = sensor.readTemperature();
			float humi = sensor.readHumidity();
			console.print(TaskInfo, "Sensor task:\n");
			console.print(TaskInfo, "> Temp: ");
			console.printFloat(TaskInfo, temp, 2);
			console.print(TaskInfo, "°C\n");
			console.print(TaskInfo, "> Humi: ");
			console.printFloat(TaskInfo, humi, 2);
			console.print(TaskInfo, "%%\n");
			console.print(TaskInfo, "> AlarmId: %x\n\n", in.coordId);

			return {temp, humi};
		}
	};

	/**
	 * Position task
	 */
	class Position: public CDEECO::PeriodicTask<Knowledge, Knowledge::Position> {
	public:
		/**
		 * Position task constructor
		 *
		 * @param component Task's component
		 */
		Position(auto &component) :
				PeriodicTask(1259, component, component.knowledge.position) {
		}

	private:
		/**
		 * Position task code
		 *
		 * @param in Copy of component's knowledge
		 * @return New position (task output)
		 */
		Knowledge::Position run(const Knowledge in) {
			GPSL10::GPSFix fix = gps.getGPSFix();

			console.print(TaskInfo, "Position task:\n");
			console.print(TaskInfo, "> valid:%d\n> date:%d.%d.%d\n> time:%d:%d:%d\n> pos: ", fix.valid, fix.day,
					fix.month, fix.year, fix.hour, fix.minute, fix.second);
			console.printFloat(TaskInfo, fix.latitude, 6);
			console.print(TaskInfo, " ");
			console.printFloat(TaskInfo, fix.longitude, 6);
			console.print(TaskInfo, "\n\n");

			if(fix.valid)
				return {fix.latitude, fix.longitude};
			else
				return in.position;
		}
	};

	/**
	 * PortableSensor component class
	 */
	class Component: public CDEECO::Component<Knowledge> {
	public:
		/**
		 * PortabletSensor knowledge magic
		 *
		 * Identifies knowledge in knowledge fragments.
		 */
		static const CDEECO::Type Type = 0x00000001;

		/// Sense task instance
		Sense sense = Sense(*this);
		/// Position task instance
		Position position = Position(*this);

		/**
		 * PortableSensor constructor
		 *
		 * @param broadcaster Refernce to broadcaster to be used by base Component
		 * @param id id of the component instance
		 */
		Component(CDEECO::Broadcaster &broadcaster, const CDEECO::Id id) :
				CDEECO::Component<Knowledge>(id, Type, broadcaster) {
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
