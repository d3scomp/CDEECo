/*
 * TempExchange.h
 *
 *  Created on: 21. 5. 2014
 *      Author: Vladimír Matěna
 */

#ifndef TEMP_EXCHANGE_H_
#define TEMP_EXCHANGE_H_

#include "cdeeco/Component.h"
#include "cdeeco/Ensamble.h"
#include "cdeeco/KnowledgeCache.h"
#include "Alarm.h"
#include "Thermometer.h"

namespace TempExchange {

	class Ensamble: CDEECO::Ensamble<Alarm::Knowledge, Alarm::Knowledge::Temps, Thermometer::Knowledge> {
	public:
		Ensamble(CDEECO::Component<Alarm::Knowledge> &coordinator,
				KnowledgeLibrary<Thermometer::Knowledge> &library) :
				CDEECO::Ensamble<Alarm::Knowledge, Alarm::Knowledge::Temps, Thermometer::Knowledge>(coordinator,
						coordinator.knowledge.tempsNearby, library, 5000) {

		}

	protected:
		bool member(const Alarm::Knowledge coord, const Thermometer::Knowledge member) {
			// TODO: Implement membership method. For now we assume all temperatures are members.
			return true;
		}

		// Map temperatures from Thermometers to Alarm
		Alarm::Knowledge::Temps map(const Alarm::Knowledge coord, const Thermometer::Knowledge member) {
			Alarm::Knowledge::Temps temps = coord.tempsNearby;

			// TODO: Do some proper mapping
			temps[0].id = 42;
			temps[0].temp = member.temperature;

			return temps;
		}
	};

}

#endif // TEMP_EXCHANGE_H_
