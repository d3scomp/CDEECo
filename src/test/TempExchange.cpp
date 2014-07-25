/**
 * \ingroup example
 * @file TempExchange.cpp
 *
 * Temperature exchange ensemble implementation
 *
 * \date 6. 7. 2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 */

#include "TempExchange.h"

namespace TempExchange {
	Ensemble::Ensemble(CDEECO::Component<Alarm::Knowledge> &coordinator,
			CDEECO::KnowledgeLibrary<PortableSensor::Knowledge> &library) :
			EnsembleType(&coordinator, &coordinator.knowledge.nearbySensors, &library, PERIOD_MS) {
	}

	Ensemble::Ensemble(CDEECO::Component<PortableSensor::Knowledge> &member,
			CDEECO::KnowledgeLibrary<Alarm::Knowledge> &library) :
			EnsembleType(&member, &member.knowledge.coordId, &library, PERIOD_MS) {
	}

	bool Ensemble::isMember(const CDEECO::Id coordId, const Alarm::Knowledge coordKnowledge, const CDEECO::Id memeberId,
			const PortableSensor::Knowledge memberKnowledge) {
		// For debugging purposes we consider all sensors are members
		return true;

		// Components with the same rough position are considered members
		// return (int) (coordKnowledge.position.lat) == (int) (memberKnowledge.position.lat)
		//		&& (int) (coordKnowledge.position.lon) == (int) (memberKnowledge.position.lon);
	}

	Alarm::Knowledge::SensorData Ensemble::memberToCoordMap(const Alarm::Knowledge coord, const CDEECO::Id memberId,
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

		// Try to replace free record
		for(auto &info : values) {
			if(info.id == Alarm::Knowledge::NO_MEMBER) {
				info.id = memberId;
				info.value = memberKnowledge.value;
				info.position = memberKnowledge.position;

				return values;
			}
		}

		// Replace random record
		console.print(None, "Random\n", memberId);
		size_t index = gen() % values.size();
		values[index].id = memberId;
		values[index].value = memberKnowledge.value;
		return values;
	}

	PortableSensor::Knowledge::CoordId Ensemble::coordToMemberMap(const PortableSensor::Knowledge member,
			const CDEECO::Id coordId, const Alarm::Knowledge coordKnowledge) {
		return coordId;
	}
}
