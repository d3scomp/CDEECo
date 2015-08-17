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
		 * Approach direction
		 *
		 * Direction from which the car is approaching the crossing.
		 */
		Direction approachDirection;

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
	};

	// TODO: Processes

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

		// TODO: Tasks

		/**
		 * Vehicle constructor
		 *
		 * @param broadcaster Refernce to broadcaster to be used by base Component
		 * @param id id of the component instance
		 */
		Component(CDEECO::Broadcaster &broadcaster, const CDEECO::Id id);
	};
}

#endif // VEHICLE_H
