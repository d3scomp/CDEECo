/**
 * Vehicle component definition
 *
 * CDEECO++ component for vehicle crossing the intelligent cross-road
 *
 * \date 17. 8. 2015
 * \author Vladimír Matěna <matena@d3s.mff.cuni.cz>
 *
 */

#ifndef VEHICLE
#define VEHICLE

#include <array>
#include <random>
#include <climits>

#include "cdeeco/Component.h"
#include "cdeeco/PeriodicTask.h"
#include "drivers/SHT1x.h"

/**
 * Vehicle component
 */
namespace Vehicle {
	/**
	 * Vehicle knowledge
	 */
	struct Knowledge: CDEECO::Knowledge {
		/// Sensor position
		struct Position {
			float lat;
			float lon;
		} position;

		// TODO: More knowledge
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

#endif // VEHICLE
