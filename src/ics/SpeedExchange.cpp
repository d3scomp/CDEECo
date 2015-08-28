#include "SpeedExchange.h"
#include <limits>

namespace SpeedExchange {
	Ensemble::Ensemble(CDEECO::Component<ICS::Knowledge> &coordinator,
			CDEECO::KnowledgeLibrary<Vehicle::Knowledge> &library) :
			EnsembleType(&coordinator, &coordinator.knowledge.vehicles, &library,
					PERIOD_MS) {
	}

	Ensemble::Ensemble(CDEECO::Component<Vehicle::Knowledge> &member,
			CDEECO::KnowledgeLibrary<ICS::Knowledge> &library) :
			EnsembleType(&member, &member.knowledge.speed, &library,
					PERIOD_MS) {
	}

	bool Ensemble::isMember(const CDEECO::Id coordId,
			const ICS::Knowledge coordKnowledge, const CDEECO::Id memeberId,
			const Vehicle::Knowledge memberKnowledge) {
		// Membership condition, vehicle is about to cross at the crossing
		return memberKnowledge.crossingId == coordKnowledge.id && memberKnowledge.crossingDistance < 50;
	}

	Vehicle::Knowledge* Ensemble::memberToCoordMap(const ICS::Knowledge coord,
			const CDEECO::Id memberId, const Vehicle::Knowledge memberKnowledge) {
		// Copy vehicle array
		Vehicle::Knowledge vehicles[] = new Vehicle::Knowledge[ICS::MAX_VEHICLES];
		memcpy(vehicles, coord.vehicles,
				sizeof(Vehicle::Knowledge) * ICS::MAX_VEHICLES);

		// Add info about new vehicle
		bool stored = false;
		// Try to update existing record for vehicle
		for (int i = 0; i < ICS::MAX_VEHICLES; ++i) {
			if (vehicles[i].id == memberKnowledge.id) {
				vehicles[i] = memberKnowledge;
				stored = true;
				break;
			}
		}
		// Try to store record in new field
		if (!stored) {
			for (int i = 0; i < ICS::MAX_VEHICLES; ++i) {
				if (vehicles[i].id == 0) {
					vehicles[i] = memberKnowledge;
					stored = true;
					break;
				}
			}
		}

		if (!stored) {
			// TODO: Report error, we were not able to store the record
		}

		return vehicles;
	}

	Speed Ensemble::coordToMemberMap(const Vehicle::Knowledge member,
			const CDEECO::Id coordId, const ICS::Knowledge coordKnowledge) {
		// Try to find desired arrival time in desired arrival time array
		for (int i = 0; i < ICS::MAX_VEHICLES; ++i) {
			if (coordKnowledge.speeds[i].id == member.id) {
				return coordKnowledge.speeds[i].speed;
			}
		}

		// Set some value when ICS has not yet set the desired arrival time for the vehicle
		return {0, 0, std::numeric_limits<Speed>::max()};
	}
}
