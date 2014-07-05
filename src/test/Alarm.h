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
		Check(auto &component) :
				PeriodicTask(3000, component, component.knowledge.tempCritical) {
		}

	private:
		/**
		 * Temperature check task code
		 *
		 * @param in Copy of Alarm knowledge
		 * @return Whenever temperature is critical (written to components knowledge)
		 */
		bool run(const Knowledge in) {
			console.print(TaskInfo, "Alarm check task\n");
			for(auto info : in.nearbySensors) {
				if(info.id != Knowledge::NO_MEMBER) {
					console.print(TaskInfo, "> Id: %x", info.id);

					console.print(TaskInfo, "\tTemp: ");
					console.printFloat(TaskInfo, info.value.temperature, 2);
					console.print(TaskInfo, "°C");

					console.print(TaskInfo, "\tHumi: ");
					console.printFloat(TaskInfo, info.value.humidity, 2);
					console.print(TaskInfo, "%%");

					console.print(TaskInfo, "\tPos: ");
					console.printFloat(TaskInfo, info.position.lat, 6);
					console.print(TaskInfo, " ");
					console.printFloat(TaskInfo, info.position.lon, 6);

					console.print(TaskInfo, "\n");
				} else {
					console.print(TaskInfo, "> No data in this slot\n");
				}
			}
			console.print(TaskInfo, "\n");

			// Check temperatures for dangerous conditions
			const float threshold = 26.0f;

			for(auto info : in.nearbySensors)
				if(info.value.temperature > threshold)
					return true;
			return false;
		}
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
		Critical(auto &component) :
				TriggeredTask(component.knowledge.tempCritical, component) {
		}
	protected:
		/**
		 * Critical triggered task code
		 *
		 * Executed on tempCritical knowledge member change. Output type declared as void -> has no output.
		 *
		 * @param in Copy of Alarm component's knowledge
		 *
		 */
		void run(const Knowledge in) {
			if(in.tempCritical) {
				console.print(TaskInfo, "##############################################################\n");
				console.print(TaskInfo, "# Critical task triggered on change and temp is CRITICAL !!! #\n");
				console.print(TaskInfo, "##############################################################\n");

				orangeLED.on();
			} else {
				orangeLED.off();
			}
		}
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
		 * @param system Reference to instance used for system access in Component base.
		 */
		Component(auto &system, const CDEECO::Id id) :
				CDEECO::Component<Knowledge>(id, Type, system) {
			// Initialize knowledge - zero and set all sensors as unused
			memset(&knowledge, 0, sizeof(Knowledge));
			knowledge.nearbySensors.fill( { Knowledge::NO_MEMBER, { 0, 0 } });
		}
	};
}

#endif // ALARM_H
