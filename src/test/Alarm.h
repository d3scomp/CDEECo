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
#include <climits>

#include "cdeeco/Component.h"

namespace Alarm {

	/**
	 * Portable thermometer knowledge
	 */
	struct Knowledge: CDEECO::Knowledge {
		struct Position {
			int x;
			int y;
		} position;

		static const KnowledgeFragment::Id NO_MEMBER = ULONG_MAX;
		struct SensorInfo {
			KnowledgeFragment::Id id;
			Sensor::Knowledge::Value value;
		};

		typedef std::array<SensorInfo, 10> SensorData;
		SensorData nearbySensors;

		bool tempCritical;
	};

	/**
	 * Temperature check task
	 */
	class Check: public CDEECO::PeriodicTask<Knowledge, bool> {
	public:
		Check(CDEECO::Component<Knowledge> &component, bool &out) :
				PeriodicTask(3000, component, out) {
		}

	protected:
		bool run(const Knowledge in) {
			Console::print(TaskInfo, "\n\n\n>>>> Running alarm temp check\n");
			Console::print(TaskInfo, ">>>> Registered sensor and values:\n");
			for(auto info : in.nearbySensors) {
				if(info.id != Knowledge::NO_MEMBER) {
					Console::print(TaskInfo, ">>>>>> Id: %x ", info.id);
					float t = info.value.temperature;
					float h = info.value.humidity;
					Console::print(TaskInfo, "Temp: %d.%d°C ", (int16_t) t, ((int16_t) (t * 100) % 100));
					Console::print(TaskInfo, "Humi: %d.%d%%\n", (int16_t) h, ((int16_t) (h * 100) % 100));
				}
			}
			Console::print(TaskInfo, "\n\n\n");

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
	class Critical: public CDEECO::TriggeredTask<Knowledge, bool, Knowledge::Position> {
	public:
		Critical(CDEECO::Component<Knowledge> &component, bool &trigger, Knowledge::Position &out):
			TriggeredTask(trigger, component, out) {
		}
	protected:
		Knowledge::Position run(const Knowledge in) {
			if(in.tempCritical) {
				Console::print(TaskInfo, "##############################################################\n");
				Console::print(TaskInfo, "# Critical task triggered on change and temp is CRITICAL !!! #\n");
				Console::print(TaskInfo, "##############################################################\n");
			}

			return in.position;
		}
	};

	/**
	 * PortableThermometer component class
	 */
	class Component: public CDEECO::Component<Knowledge> {
	public:
		KnowledgeFragment::Type Type = 0x00000002;
		Check check = Check(*this, this->knowledge.tempCritical);
		Critical critical = Critical(*this, this->knowledge.tempCritical, this->knowledge.position);

		Component(CDEECO::System &system) :
				CDEECO::Component<Knowledge>(Type, system) {
			// Initialize knowledge
			memset(&knowledge, 0, sizeof(Knowledge));
			knowledge.nearbySensors.fill( { Knowledge::NO_MEMBER, { 0, 0 } });
		}
	};

}

namespace CDEECO {

	/**
	 * Allowed offsets to guarantee knowledge consistency
	 */
	template<>
	struct KnowledgeTrait<Alarm::Knowledge> {
		static constexpr std::array<size_t, 3> offsets = { offsetof(Alarm::Knowledge, position), offsetof(
				Alarm::Knowledge, nearbySensors),
		offsetof(Alarm::Knowledge, nearbySensors) + 100 };
	};
	/**
	 * This declaration do not require array size to be specified twice, but drives eclipse crazy.
	 * constexpr decltype(KnowledgeTrait<TestKnowledge>::offsets) KnowledgeTrait<TestKnowledge>::offsets;
	 */
	constexpr std::array<size_t, 3> KnowledgeTrait<Alarm::Knowledge>::offsets;

}

#endif // PORTABLE_THERMOMETER_H_
