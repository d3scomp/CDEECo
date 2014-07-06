/*
 * Alarm.cpp
 *
 *  Created on: 6. 7. 2014
 *      Author: Vladimír Matěna
 *
 */

#include "Alarm.h"

/**
 * CDEECO++ component monitoring temperatures provided by PortableSensors.
 */
namespace Alarm {
	Check::Check(auto &component) :
			PeriodicTask(3000, component, component.knowledge.tempCritical) {
	}

	bool Check::run(const Knowledge in) {
		console.print(TaskInfo, "Alarm check task\n");
		for(auto info : in.nearbySensors) {
			if(info.id != Knowledge::NO_MEMBER) {
				console.print(TaskInfo, "> Id: %x", info.id);

				console.print(TaskInfo, "\tTemp: ");
				console.printFloat(TaskInfo, info.value.temperature, 2);
				console.print(TaskInfo, "°C");

				console.print(TaskInfo, "\tHumi: ");
				console.printFloat(TaskInfo, info.value.humidity, 2);
				console.print(TaskInfo, "%%");

				console.print(TaskInfo, "\tPos: ");
				console.printFloat(TaskInfo, info.position.lat, 6);
				console.print(TaskInfo, " ");
				console.printFloat(TaskInfo, info.position.lon, 6);

				console.print(TaskInfo, "\n");
			} else {
				console.print(TaskInfo, "> No data in this slot\n");
			}
		}
		console.print(TaskInfo, "\n");

		// Check temperatures for dangerous conditions
		const float threshold = 26.0f;

		for(auto info : in.nearbySensors)
			if(info.value.temperature > threshold)
				return true;
		return false;
	}

	Critical::Critical(auto &component) :
			TriggeredTask(component.knowledge.tempCritical, component) {
	}

	void Critical::run(const Knowledge in) {
		if(in.tempCritical) {
			console.print(TaskInfo, "##############################################################\n");
			console.print(TaskInfo, "# Critical task triggered on change and temp is CRITICAL !!! #\n");
			console.print(TaskInfo, "##############################################################\n");

			orangeLED.on();
		} else {
			orangeLED.off();
		}
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
		// Initialize knowledge - zero and set all sensors as unused
		memset(&knowledge, 0, sizeof(Knowledge));
		knowledge.nearbySensors.fill( { Knowledge::NO_MEMBER, { 0, 0 } });
	}
}
