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
			// TODO: Implement membership method
			return false;
		}

		Alarm::Knowledge::Temps map(const Alarm::Knowledge coord, const Thermometer::Knowledge member) {
			Alarm::Knowledge::Temps temps;

			// TODO: Map knowledge from member to coordinator

			return temps;
		}
	};

}

#endif // TEMP_EXCHANGE_H_
