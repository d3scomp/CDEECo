/**
 * \ingroup example
 * @file PortableSensor.cpp
 *
 * Portable sensor component definition
 *
 * CDEECO++ component providing temperature and humidity at given position.
 *
 * \date 6. 7. 2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 *
 */

#include "PortableSensor.h"

#include "../drivers/GPS.h"

namespace PortableSensor {
	Sense::Sense(auto &component) :
			PeriodicTask(1800, component, component.knowledge.value) {
		sensor.init();
	}

	Knowledge::Value Sense::run(const Knowledge in) {
		float temp = sensor.readTemperature();
		float humi = sensor.readHumidity();
		console.print(TaskInfo, "Sensor task:\n");
		console.print(TaskInfo, "> Temp: ");
		console.printFloat(TaskInfo, temp, 2);
		console.print(TaskInfo, "°C\n");
		console.print(TaskInfo, "> Humi: ");
		console.printFloat(TaskInfo, humi, 2);
		console.print(TaskInfo, "%%\n");
		console.print(TaskInfo, "> AlarmId: %x\n\n", in.coordId);

		return {temp, humi};
	}

	Position::Position(auto &component) :
			PeriodicTask(1259, component, component.knowledge.position) {
	}

	Knowledge::Position Position::run(const Knowledge in) {
		GPSL10::GPSFix fix = gps.getGPSFix();

		console.print(TaskInfo, "Position task:\n");
		console.print(TaskInfo, "> valid:%d\n> date:%d.%d.%d\n> time:%d:%d:%d\n> pos: ", fix.valid, fix.day, fix.month,
				fix.year, fix.hour, fix.minute, fix.second);
		console.printFloat(TaskInfo, fix.latitude, 6);
		console.print(TaskInfo, " ");
		console.printFloat(TaskInfo, fix.longitude, 6);
		console.print(TaskInfo, "\n\n");

		if(fix.valid)
			return {fix.latitude, fix.longitude};
		else
			return in.position;
	}

	Component::Component(CDEECO::Broadcaster &broadcaster, const CDEECO::Id id) :
			CDEECO::Component<Knowledge>(id, Type, broadcaster) {
		// Initialize knowledge
		memset(&knowledge, 0, sizeof(Knowledge));
	}
}

namespace CDEECO {
	/// Allowed offsets to guarantee knowledge consistency
	template<>
	struct KnowledgeTrait<PortableSensor::Knowledge> {
		static constexpr std::array<size_t, 1> offsets = { { offsetof(PortableSensor::Knowledge, position) } };
	};
	// This declaration do not require array size to be specified twice, but drives eclipse crazy.
	//constexpr decltype(KnowledgeTrait<TestKnowledge>::offsets) KnowledgeTrait<TestKnowledge>::offsets;
	constexpr std::array<size_t, 1> KnowledgeTrait<PortableSensor::Knowledge>::offsets;
}
