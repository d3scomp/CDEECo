/**
 * Vehicle component definition
 *
 * CDEECO++ component for vehicle crossing the intelligent cross-road
 *
 * \date 17. 8. 2015
 * \author Vladimír Matěna <matena@d3s.mff.cuni.cz>
 *
 */

#ifndef VEHICLE_H
#define VEHICLE_H

#include <array>
#include <random>
#include <climits>

#include "cdeeco/Component.h"
#include "cdeeco/PeriodicTask.h"
#include "drivers/SHT1x.h"
#include "ICSUtils.h"

/**
 * Vehicle component
 */
namespace Vehicle {
	/**
	 * Vehicle knowledge
	 */
	struct Knowledge: CDEECO::Knowledge {
		/**
		 * Vehicle identification
		 */
		VehicleId id;

		/**
		 * Current time
		 *
		 * Used to judge whether the vehicle is complaint with the system.
		 */
		Time time;

		/**
		 * Whenever vehicle supports remote operation
		 */
		bool remotelyOperable;

		/**
		 * Current vehicle mode
		 */
		Mode mode;

		/**
		 * Current crossing
		 *
		 * The next crossing this vehicle is going to cross
		 */
		CrossingId crossingId;

		/**
		 * Approach direction
		 *
		 * Direction from which the car is approaching the crossing.
		 */
		Direction approachDirection;

		/**
		 * Info about crossing distance
		 */
		Distance crossingDistance;

		/**
		 * Speed assigned by ICS
		 */
		Speed sped;

		/**
		 * Speed timestamp created by ICS
		 */
		Time speedTimestamp;

		/**
		 * Whenever the vehicle is privileged
		 */
		bool priviledged;
	};

	/**
	 * Stores current time in the knowledge
	 */
	class StoreCurrentTime: public CDEECO::PeriodicTask<Knowledge, Time> {
	public:
		StoreCurrentTime(auto &component);

	private:
		Time run(const Knowledge in);
	};

	/**
	 * Monitors current system
	 *
	 * Checks latency of input data and ICS operational state
	 */
	class Monitor: public CDEECO::PeriodicTask<Knowledge, Knowledge> {
	public:
		Monitor(auto &component);

	private:
		Mode run(const Knowledge in);
	};

	/**
	 * Plans route and sets current crossing
	 *
	 * This process uses GPS map and schedule to assign crossing identification
	 * when the vehicle is about to reach the range of the crossing.
	 */
	class PlanRoute: public CDEECO::PeriodicTask<Knowledge, CrossingId> {
	public:
		PlanRoute(auto &component);

	private:
		CrossingId run(const Knowledge in);
	};

	/**
	 * Updates information about crossing distance and controls vehicle speed
	 */
	class UpdateCrossingDistance: public CDEECO::PeriodicTask<Knowledge, Distance> {
	public:
		UpdateCrossingDistance(auto &component);

	private:
		Distance run(const Knowledge in);
	};

	/**
	 * Controls speed in order to keep the speed assigned by intelligent crossing system
	 */
	class SetSpeed: public CDEECO::PeriodicTask<Knowledge, void> {
	public:
		SetSpeed(auto &component);

	private:
		void run(const Knowledge in);
	};

	/**
	 * Vehicle component class
	 */
	class Component: public CDEECO::Component<Knowledge> {
	public:
		/**
		 * Vehicle knowledge
		 *
		 * Identifies knowledge in knowledge fragments.
		 */
		static const CDEECO::Type Type = 0x00000001;

		// Private component data
		VehicleInterface &vehicleInterface;

		// Process instances
		StoreCurrentTime storeCurrentTime = StoreCurrentTime(*this);
		Monitor monitor = Monitor(*this);
		PlanRoute planRoute = PlanRoute(*this);
		UpdateCrossingDistance updateCrossingInfo = UpdateCrossingDistance(*this);
		SetSpeed updateTimeAndDrive = SetSpeed(*this);

		/**
		 * Vehicle constructor
		 *
		 * @param broadcaster Reference to broadcaster to be used by base Component
		 * @param id id of the component instance
		 */
		Component(CDEECO::Broadcaster &broadcaster, const CDEECO::Id id, VehicleInterface &vehicleInterface);
	};
}

#endif // VEHICLE_H
