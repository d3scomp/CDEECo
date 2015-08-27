#include "Vehicle.h"
#include "ICSUtils.h"

namespace Vehicle {
	Component::Component(CDEECO::Broadcaster &broadcaster, const CDEECO::Id id, bool remotelyOperable) :
			CDEECO::Component<Knowledge>(id, Type, broadcaster) {
		// Initialize knowledge
		memset(&knowledge, 0, sizeof(Knowledge));
		knowledge.id = id;
		knowledge.remotelyOperable = remotelyOperable;
		// TODO: Set approach direction based on the current crossing
		knowledge.approachDirection = NORTH_EAST;
	}

	StoreCurrentTime::StoreCurrentTime(auto &component) {
		// TODO: Period
		PeriodicTask(10, component, component.knowledge.time);
	}

	Time StoreCurrentTime::run(const Knowledge in) {
		return GetCurrentTimeMs();
	}

	Monitor::Monitor(auto &component) {
		// TODO: Period
		Monitor(10, component, component.knowledge.mode);
	}

	Knowledge::Mode Monitor::run(const Knowledge in) {
		if(in.desiredArrivalTime.crossingId == in.crossingId &&
				std::abs(in.desiredArrivalTime.time - in.time) < LATENCY_THRESHOLD_MS) {
			return Knowledge::Mode::Automatic;
		} else {
			return Knowledge::Mode::Manual;
		}
	}

	PlanRoute::PlanRoute(auto &component) {
		// TODO: Period
		PeriodicTask(1000, component, component.knowledge.crossingId);
	}

	CrossingId PlanRoute::run(const Knowledge in) {
		// TODO: return next crossing on the path to destination
		return 42;
	}

	UpdateCrossingInfo::UpdateCrossingInfo(auto &component) {
		// TODO: Period
		PeriodicTask(1000, component, component.knowledge.crossingDistanceInfo);
	}

	Knowledge::CrossingDistanceInfo UpdateCrossingInfo::UpdateCrossingInfo(const Knowledge in) {
		// Desired arrival time set by ICS
		ArrivalTime desiredArivalTime = in.desiredArrivalTime;

		// Calculate new throttle value and set it to vehicle

		// Calculate new distance and time to crossing
		// TODO: Implement this
		ArrivalTime timeToCrossing = 42;
		ArrivalTime minTimeToCrossing = 42;
		DistanceToCrossing distanceToCrossing = 42;

		return {
			timeToCrossing,
			minTimeToCrossing,
			distanceToCrossing
		};
	}
}
