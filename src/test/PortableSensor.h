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
#include "cdeeco/PeriodicTask.h"
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
		Sense(auto &component);

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
		Knowledge::Value run(const Knowledge in);
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
		Position(auto &component);

	private:
		/**
		 * Position task code
		 *
		 * @param in Copy of component's knowledge
		 * @return New position (task output)
		 */
		Knowledge::Position run(const Knowledge in);
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
		Component(CDEECO::Broadcaster &broadcaster, const CDEECO::Id id);
	};
}

#endif // PORTABLE_SENSOR_H
