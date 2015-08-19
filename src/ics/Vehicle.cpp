#include "Vehicle.h"
#include "ICSUtils.h"

namespace Vehicle {
	Component::Component(CDEECO::Broadcaster &broadcaster, const CDEECO::Id id, bool remotelyOperable) :
			CDEECO::Component<Knowledge>(id, Type, broadcaster) {
		// Initialize knowledge
		memset(&knowledge, 0, sizeof(Knowledge));
		// TODO: Set approach direction based on the current crossing
		knowledge.id = id;
		knowledge.approachDirection = NORTH_EAST;
		knowledge.remotelyOperable = remotelyOperable;
	}

	StoreCurrentTime::StoreCurrentTime(auto &component) {
		// TODO: Period
		PeriodicTask(10, component, component.knowledge.time);
	}

	Time StoreCurrentTime::run(const Knowledge in) {
		return GetCurrentTimeMs();
	}
}
