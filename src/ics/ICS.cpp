#include "../ics/ICS.h"

/**
 * CDEECO++ component handling intelligent cross-road
 */
namespace ICS {

	Component::Component(CDEECO::Broadcaster &broadcaster, const CDEECO::Id id) :
			CDEECO::Component<Knowledge>(id, Type, broadcaster) {
		// Initialize knowledge - zero and set all sensors as unused
		memset(&knowledge, 0, sizeof(Knowledge));

	}
}
