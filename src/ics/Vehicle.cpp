#include "Vehicle.h"
#include "ICSUtils.h"

namespace Vehicle {
	StoreCurrentTime::StoreCurrentTime(auto &component) {
		// TODO: Period
		PeriodicTask(10, component, component.knowledge.time);
	}

	Time StoreCurrentTime::run(const Knowledge in) {
		return GetCurrentTimeMs();
	}
}
