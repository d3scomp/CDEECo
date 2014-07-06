/*
 * Alarm.h
 *
 *  Created on: 21. 5. 2014
 *      Author: Vladimír Matěna
 *
 */

#ifndef ALARM_H
#define ALARM_H

#include <array>
#include <algorithm>
#include <climits>

#include "cdeeco/Component.h"
#include "cdeeco/PeriodicTask.h"
#include "cdeeco/TriggeredTask.h"

#include "test/PortableSensor.h"

/**
 * CDEECO++ component monitoring temperatures provided by PortableSensors.
 */
namespace Alarm {
	/**
	 * Alarm knowledge
	 */
	struct Knowledge: CDEECO::Knowledge {
		/// Alarm position
		struct Position {
			int lat;
			int lon;
		} position;

		/// Id valued used to signal no id
		static const CDEECO::Id NO_MEMBER = ULONG_MAX;

		/// Information about single sensor
		struct SensorInfo {
			CDEECO::Id id;
			PortableSensor::Knowledge::Value value;
			PortableSensor::Knowledge::Position position;
		};

		/// Fixed size array of sensors
		typedef std::array<SensorInfo, 10> SensorData;

		/// Nearby sensors
		SensorData nearbySensors;

		// Whenever nearby sensor temperature reached critical
		bool tempCritical;
	};

	/**
	 * Temperature check task
	 */
	class Check: public CDEECO::PeriodicTask<Knowledge, bool> {
	public:
		/**
		 * Temperature check task constructor
		 *
		 * @param component Component this task is part of
		 */
		Check(auto &component);

	private:
		/**
		 * Temperature check task code
		 *
		 * @param in Copy of Alarm knowledge
		 * @return Whenever temperature is critical (written to components knowledge)
		 */
		bool run(const Knowledge in);
	};

	/**
	 * Temperature critical trigger task
	 */
	class Critical: public CDEECO::TriggeredTask<Knowledge, bool, void> {
	public:
		/**
		 * Critical trigger task constructor
		 *
		 * @param component Component this task is part of
		 */
		Critical(auto &component);
	protected:
		/**
		 * Critical triggered task code
		 *
		 * Executed on tempCritical knowledge member change. Output type declared as void -> has no output.
		 *
		 * @param in Copy of Alarm component's knowledge
		 *
		 */
		void run(const Knowledge in);
	};

	/**
	 * Alarm component class
	 */
	class Component: public CDEECO::Component<Knowledge> {
	public:
		/// Alarm component magic number (identifies knowledge type fragments)
		static const CDEECO::Type Type = 0x00000002;

		// Check task instance
		Check check = Check(*this);

		// Critical task instance
		Critical critical = Critical(*this);

		/**
		 * Costruct Alarm component
		 *
		 * @param broadcaster Reference to instance used for system access in Component base.
		 */
		Component(CDEECO::Broadcaster &broadcaster, const CDEECO::Id id);
	};
}

#endif // ALARM_H
