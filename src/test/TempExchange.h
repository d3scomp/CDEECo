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
#include "cdeeco/Ensemble.h"
#include "cdeeco/KnowledgeCache.h"
#include "Alarm.h"
#include "PortableSensor.h"

namespace TempExchange {
	typedef CDEECO::Ensemble<Alarm::Knowledge, Alarm::Knowledge::SensorData, PortableSensor::Knowledge,
			PortableSensor::Knowledge::CoordId> EnsembleType;

	class Ensemble: EnsembleType {
	public:
		static const auto PERIOD_MS = 2027;

		Ensemble(CDEECO::Component<Alarm::Knowledge> &coordinator, auto &library) :
				EnsembleType(&coordinator, &coordinator.knowledge.nearbySensors, &library, PERIOD_MS) {
		}

		Ensemble(CDEECO::Component<PortableSensor::Knowledge> &member, auto &library) :
				EnsembleType(&member, &member.knowledge.coordId, &library, PERIOD_MS) {
		}

	protected:
		bool isMember(const CDEECO::Id coordId, const Alarm::Knowledge coordKnowledge, const CDEECO::Id memeberId,
				const PortableSensor::Knowledge memberKnowledge) {
			// TODO: Implement membership method. For now we assume all temperatures are members.
			return true;
		}

		// Map temperatures from Thermometers to Alarm
		Alarm::Knowledge::SensorData memberToCoordMap(const Alarm::Knowledge coord, const CDEECO::Id memberId,
				const PortableSensor::Knowledge memberKnowledge) {
			auto values = coord.nearbySensors;

			// Try to update record
			for(auto &info : values) {
				if(info.id == memberId) {
					info.value = memberKnowledge.value;
					info.position = memberKnowledge.position;

					return values;
				}
			}

			// Replace random record
			size_t index = gen() % values.size();
			values[index].id = memberId;
			values[index].value = memberKnowledge.value;
			return values;
		}

		// Map data from Alarm to Thermometer
		PortableSensor::Knowledge::CoordId coordToMemberMap(const PortableSensor::Knowledge member,
				const CDEECO::Id coordId, const Alarm::Knowledge coordKnowledge) {
			return coordId;
		}

	private:
		std::default_random_engine gen;
	};
}

#endif // TEMP_EXCHANGE_H_
