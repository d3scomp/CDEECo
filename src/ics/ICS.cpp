#include "ICS.h"
#include "ICSUtils.h"

#include <set>

/**
 * CDEECO++ component handling intelligent cross-road
 */
namespace ICS {
	Component::Component(CDEECO::Broadcaster &broadcaster, const CDEECO::Id id) :
			CDEECO::Component<Knowledge>(id, Type, broadcaster) {
		// Initialize knowledge - zero and set all sensors as unused
		memset(&knowledge, 0, sizeof(Knowledge));
		knowledge.id = id;
	}

	CheckOperational::CheckOperational(auto &component):
		// TODO: Period ???
		PeriodicTask(3000, component, component.knowledge.mode) {
	}

	/**
	 * Magic check for all vehicles registered by pressure and optical sensors
	 */
	std::set<VehicleId> CheckOperational::getOutOfBandRegisteredVehicles() {
		/// TODO: Implement out of band vehicle id registration
		return std::set<VehicleId>();
	}

	/**
	 * Checks whenever the crossing can run in smart mode
	 */
	Mode CheckOperational::run(const Knowledge in) {
		// Check if there are vehicles that are not registered by the system
		std::set<VehicleId> outOfBandVehicles = getOutOfBandRegisteredVehicles();
		for(unsigned int i = 0; i < MAX_VEHICLES; ++i) {
			outOfBandVehicles.erase(in.vehicles[i].id);
		}
		if(!outOfBandVehicles.empty()) {
			return Mode::Manual;
		}

		// Check if there are unsupported vehicles in the system
		for(unsigned int i = 0; i < MAX_VEHICLES; ++i) {
			if(!in.vehicles[i].mode == Mode::Automatic) {
				return Mode::Manual;
			}
		}

		// Check if there are vehicles with high latency in the system
		for(unsigned int i = 0; i < MAX_VEHICLES; ++i) {
			Vehicle::Knowledge vehicle = in.vehicles[i];

			if(vehicle.id != 0 && vehicle.crossingDistance != 0 &&
					std::abs(GetCurrentTimeMs() - vehicle.time) > MAX_LATENCY_MS) {
				return Mode::Manual;
			}
		}

		return Mode::Automatic;
	}

	RemoveOldVehicles::RemoveOldVehicles(auto &component) {
		// TODO: Period
		PeriodicTask(1000, component, component.knowledge.vehicles);
	}

	Vehicle::Knowledge* RemoveOldVehicles::run(const Knowledge in) {
		// Copy live vehicles to new array
		Vehicle::Knowledge vehicles[] = new Vehicle::Knowledge[MAX_VEHICLES];
		int used = 0;
		for(int i = 0; i < MAX_VEHICLES; ++i) {
			Vehicle::Knowledge vehicle = in.vehicles[i];
			if(vehicle.id != 0 && vehicle.crossingDistance != 0) {
				vehicles[used++] = vehicle;
			}
		}

		// Fill the remaining space with invalid entries
		for(; used < MAX_VEHICLES; ++used) {
			vehicles[used].id = 0;
		}
	}

	ScheduleVehicles::ScheduleVehicles(auto &component) {
		// TODO: Period
		PeriodicTask(2000, component, component.knowledge.speeds);
	}

	Knowledge::SpeedInfo* ScheduleVehicles::run(const Knowledge in) {
		// Arrival times from last schedule
		const Speed inSpeeds[MAX_VEHICLES] = in.speeds;
		// Current information about vehicles
		const Vehicle::Knowledge inVehicleData[MAX_VEHICLES] = in.vehicles;
		// New schedule to be computed
		Knowledge::SpeedInfo outSpeeds[] = new Knowledge::SpeedInfo[MAX_VEHICLES];

		/*
		 * TODO: Compute new schedule, take into account current schedule,
		 * conflicting directions and arrival time estimates.
		 *
		 * outSPeeds = schedule(inSpeeds, inVehicleData);
		 */

		return outSpeeds;
	}

	StoreCurrentTime::StoreCurrentTime(auto &component) {
		// TODO: Period
		PeriodicTask(10, component, component.knowledge.time);
	}

	Time StoreCurrentTime::run(const Knowledge in) {
		return GetCurrentTimeMs();
	}
}
