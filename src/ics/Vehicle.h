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
		 * Info about crossing distance
		 */
		typedef struct {
			/**
			 * Current time to crossing
			 *
			 * Takes into account current position and speed.
			 */
			ArrivalTime timeToCrossing;

			/**
			 * Minimal time to crossing
			 *
			 * Takes into account current position, speed, maximal acceleration and top vehicle speed.
			 */
			ArrivalTime minTimeToCrossing;

			/**
			 * Distance to crossing
			 *
			 * Distance to crossing point in meters. Used to maintain queue of cars waiting for crossing.
			 * This is needed as cars can possibly increase speed, but this ability is limited by cars in front of them.
			 */
			DistanceToCrossing distanceToCrossing;
		} CrossingDistanceInfo;

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
		CrossingDistanceInfo crossingDistanceInfo;

		/**
		 * Desired time to crossing planed by ICS
		 */
		ArrivalTime desiredArrivalTime;
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
	 * Plans route and sets current crossing
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
	class UpdateCrossingInfo: public CDEECO::PeriodicTask<Knowledge, Knowledge::CrossingDistanceInfo> {
	public:
		UpdateCrossingInfo(auto &component);

	private:
		Knowledge::CrossingDistanceInfo run(const Knowledge in);
	};

	/**
	 * Controls throttle in order to match desired time and updates current time to crossing
	 */
	class UpdateTimeAndDrive: public CDEECO::PeriodicTask<Knowledge, Time> {
	public:
		UpdateTimeAndDrive(auto &component);

	private:
		 run(const Knowledge in);
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

		// Process instances
		StoreCurrentTime storeCurrentTime = StoreCurrentTime(*this);

		/**
		 * Vehicle constructor
		 *
		 * @param broadcaster Reference to broadcaster to be used by base Component
		 * @param id id of the component instance
		 */
		Component(CDEECO::Broadcaster &broadcaster, const CDEECO::Id id, bool remotelyOperable);
	};
}

#endif // VEHICLE_H
