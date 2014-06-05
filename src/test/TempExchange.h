/*
 * TempExchange.h
 *
 *  Created on: 21. 5. 2014
 *      Author: Vladimír Matěna
 */

#ifndef TEMP_EXCHANGE_H_
#define TEMP_EXCHANGE_H_

#include <random>

#include "cdeeco/Component.h"
#include "cdeeco/Ensamble.h"
#include "cdeeco/KnowledgeCache.h"
#include "Alarm.h"
#include "PortableSensor.h"

namespace TempExchange {
	class Ensamble: CDEECO::Ensamble<Alarm::Knowledge, Alarm::Knowledge::SensorData, Sensor::Knowledge, void> {
	public:
		Ensamble(CDEECO::Component<Alarm::Knowledge> &coordinator, KnowledgeLibrary<Sensor::Knowledge> &library) :
				CDEECO::Ensamble<Alarm::Knowledge, Alarm::Knowledge::SensorData, Sensor::Knowledge, void>(&coordinator,
						&coordinator.knowledge.nearbySensors, &library, 5000) {
		}

	protected:
		bool isMember(const Alarm::Knowledge coord, const Sensor::Knowledge memberKnowledge) {
			// TODO: Implement membership method. For now we assume all temperatures are members.
			return true;
		}

		// Map temperatures from Thermometers to Alarm
		Alarm::Knowledge::SensorData memberToCoordMap(const Alarm::Knowledge coord, const KnowledgeFragment::Id memberId,
				const Sensor::Knowledge memberKnowledge) {
			auto values = coord.nearbySensors;

			// Try to update record
			for(auto &info : values)
				if(info.id == memberId) {
					info.value = memberKnowledge.value;
					return values;
				}

			// Replace random record
			size_t index = gen() % values.size();
			values[index].id = memberId;
			values[index].value = memberKnowledge.value;
			return values;
		}

		// Map data from Alarm to Thermometer
		void coordToMemberMap(const Sensor::Knowledge member, const KnowledgeFragment::Id coordId, const Alarm::Knowledge coordKnowledge) {
			// This does nothing
		}

	private:
		std::default_random_engine gen;
	};
}

#endif // TEMP_EXCHANGE_H_
