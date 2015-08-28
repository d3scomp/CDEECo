#include "Vehicle.h"
#include "ICSUtils.h"

namespace Vehicle {
	Component::Component(CDEECO::Broadcaster &broadcaster, const CDEECO::Id id, VehicleInterface &vehicleInterface) :
			CDEECO::Component<Knowledge>(id, Type, broadcaster), vehicleInterface(vehicleInterface) {
		// Initialize knowledge
		memset(&knowledge, 0, sizeof(Knowledge));
		knowledge.id = id;
		knowledge.remotelyOperable = vehicleInterface.isInteligentModeSupported();
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

	Mode Monitor::run(const Knowledge in) {
		if(std::abs(in.speedTimestamp - in.time) < LATENCY_THRESHOLD_MS) {
			return Mode::Automatic;
		} else {
			return Mode::Manual;
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

	UpdateCrossingDistance::UpdateCrossingDistance(auto &component): component(component) {
		// TODO: Period
		PeriodicTask(1000, component, component.knowledge.crossingDistance);

		// Reference to component
		auto &component;
	}

	Distance UpdateCrossingDistance::run(const Knowledge in) {
		return getCrossingDistance(component.getPosition(), in.crossingId);
	}

	SetSpeed::SetSpeed(auto &component): component(component) {
		// TODO: Period
		PeriodicTask(1000, component);

		// Reference to component
		auto &component;
	}

	void SetSpeed::run(const Knowledge in) {
		component.vehicleInterface.setSpeed(in.speed);
	}
}
