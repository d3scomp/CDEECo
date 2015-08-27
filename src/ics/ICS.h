/**
 * Intelligent cross-road system DEECo component
 *
 * \date 17. 8. 2015
 * \author Vladimír Matěna <matena@d3s.mff.cuni.cz>
 *
 */

#ifndef ICS_H
#define ICS_H

#include <array>
#include <algorithm>
#include <climits>

#include "cdeeco/Component.h"
#include "cdeeco/PeriodicTask.h"
#include "cdeeco/TriggeredTask.h"

#include "ics/Vehicle.h"

/**
 * CDEECO++ component handling intelligent cross-road
 */
namespace ICS {
	const int MAX_VEHICLES = 70;
	const long MAX_LATENCY_MS = 50;

	/**
	 * Intelligent cross-road system knowledge
	 */
	struct Knowledge: CDEECO::Knowledge {
		typedef struct {
			VehicleId id;
			ArrivalTime desiredArrivalTime;
		} DesiredArrivalTime;

		/**
		 * Crossing id
		 */
		CrossingId id;

		/**
		 * Current time
		 */
		Time time;

		/**
		 * Whenever cross-road is running in intelligent mode
		 */
		Mode mode;

		/**
		 * Information about vehicles
		 *
		 * Information about vehicles approaching from all directions sorted by proximity to the crossing
		 *
		 * Vehicles with id = 0 are considered invalid and should be ignored
		 */
		Vehicle::Knowledge vehicles[MAX_VEHICLES];

		/**
		 * Desired arrival times for vehicles
		 */
		DesiredArrivalTime arrivalTimes[MAX_VEHICLES];
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
	 * Check operational state
	 *
	 * Checks whenever all vehicles in the crossing range support remote operation
	 * and have good enough latency in order to run the crossing in smart mode.
	 */
	class CheckOperational: public CDEECO::PeriodicTask<Knowledge, bool> {
	public:
		CheckOperational(auto &component);

	private:
		bool run(const Knowledge in);
		std::set<VehicleId> getOutOfBandRegisteredVehicles();
	};

	/**
	 * Remove vehicles that already left the crossing
	 */
	class RemoveOldVehicles: public CDEECO::PeriodicTask<Knowledge, Vehicle::Knowledge*> {
	public:
		RemoveOldVehicles(auto &component);

	private:
		Vehicle::Knowledge* run(const Knowledge in);
	};

	/**
	 * Schedules vehicle arrival to optimize crossing throughput
	 */
	class ScheduleVehicles: public CDEECO::PeriodicTask<Knowledge, Knowledge::DesiredArrivalTime*> {
	public:
		ScheduleVehicles(auto &component);

	private:
		Knowledge::DesiredArrivalTime* run(const Knowledge in);
	};

	/**
	 * Intelligent cross-road component class
	 */
	class Component: public CDEECO::Component<Knowledge> {
	public:
		/// ICS component magic
		static const CDEECO::Type Type = 0x00000002;

		// Process instances
		StoreCurrentTime storeCurrentTime = StoreCurrentTime(*this);
		CheckOperational checkOperational = CheckOperational(*this);
		RemoveOldVehicles removeOldVehicles = RemoveOldVehicles(*this);
		ScheduleVehicles scheduleVehicles = ScheduleVehicles(*this);

		/**
		 * Construct ICS component
		 *
		 * @param broadcaster Reference to instance used for system access in Component base.
		 * @param id Id of this component instance
		 */
		Component(CDEECO::Broadcaster &broadcaster, const CDEECO::Id id);
	};
}

#endif // ICS_H
