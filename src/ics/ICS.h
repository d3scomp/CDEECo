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
	/**
	 * Intelligent cross-road system knowledge
	 */
	struct Knowledge: CDEECO::Knowledge {
		// Whenever cross-road is running in intelligent mode
		bool operational;

		// TODO: More knowledge
	};

	// TODO: Processes

	/**
	 * Intelligent cross-road component class
	 */
	class Component: public CDEECO::Component<Knowledge> {
	public:
		/// ICS component magic
		static const CDEECO::Type Type = 0x00000002;

		/**
		 * Costruct Alarm component
		 *
		 * @param broadcaster Reference to instance used for system access in Component base.
		 * @param id Id of this component instance
		 */
		Component(CDEECO::Broadcaster &broadcaster, const CDEECO::Id id);
	};
}

#endif // ICS_H
