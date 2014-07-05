/*
 * TempExchange.h
 *
 *  Created on: 21. 5. 2014
 *      Author: Vladimír Matěna
 */

#ifndef TEMP_EXCHANGE_H
#define TEMP_EXCHANGE_H

#include <random>

#include "cdeeco/Component.h"
#include "cdeeco/Ensemble.h"
#include "cdeeco/KnowledgeCache.h"
#include "Alarm.h"
#include "PortableSensor.h"

/**
 * Temperature exchange ensemble
 *
 * Used to provide Alarms with sensor data
 */
namespace TempExchange {
	/// Base Ensemble type definition
	typedef CDEECO::Ensemble<Alarm::Knowledge, Alarm::Knowledge::SensorData, PortableSensor::Knowledge,
			PortableSensor::Knowledge::CoordId> EnsembleType;

	/**
	 * Temperature exchange ensemble class
	 */
	class Ensemble: EnsembleType {
	public:
		/// Map try period
		static const auto PERIOD_MS = 2027;

		/**
		 * Temperature exchange constructor for node where coordinator is hosted
		 *
		 * @param coordinator Coordinator component
		 * @param library Library of member component knowledge
		 */
		Ensemble(CDEECO::Component<Alarm::Knowledge> &coordinator, auto &library) :
				EnsembleType(&coordinator, &coordinator.knowledge.nearbySensors, &library, PERIOD_MS) {
		}

		/**
		 * Temperature exchange constructor for node where member is hosted
		 *
		 * @param member Member component
		 * @param library Library of coordinator component knowledge
		 */
		Ensemble(CDEECO::Component<PortableSensor::Knowledge> &member, auto &library) :
				EnsembleType(&member, &member.knowledge.coordId, &library, PERIOD_MS) {
		}

	protected:
		/**
		 * Check whenever supplied member and coordinators are in the ensemble
		 *
		 * @param coordId Coordinator id
		 * @param coordKnowledge Coordinator knowledge
		 * @param memberId Member id
		 * @param memberKnowledge Member knowledge
		 *
		 * @return Whenever the supplied member and coordinator belong to the ensemble
		 */
		bool isMember(const CDEECO::Id coordId, const Alarm::Knowledge coordKnowledge, const CDEECO::Id memeberId,
				const PortableSensor::Knowledge memberKnowledge) {
			// For debugging purposes we consider all sensors are members
			return true;

			// Components with the same rough position are considered members
			// return (int) (coordKnowledge.position.lat) == (int) (memberKnowledge.position.lat)
			//		&& (int) (coordKnowledge.position.lon) == (int) (memberKnowledge.position.lon);
		}

		/**
		 *  Map temperatures from Thermometers to Alarm
		 *
		 *  @param coord Coordinator knowledge
		 *  @param memberId Member id
		 *  @param memberKnowledge Member knowledge
		 *  @return Sensor data array (ensemble output)
		 */
		Alarm::Knowledge::SensorData memberToCoordMap(const Alarm::Knowledge coord, const CDEECO::Id memberId,
				const PortableSensor::Knowledge memberKnowledge) {
			auto values = coord.nearbySensors;

			// Try to update record
			for(auto &info : values) {
				if(info.id == memberId) {
					info.value = memberKnowledge.value;
					info.position = memberKnowledge.position;

					return values;
				}
			}

			// Try to replace free record
			for(auto &info : values) {
				if(info.id == Alarm::Knowledge::NO_MEMBER) {
					info.id = memberId;
					info.value = memberKnowledge.value;
					info.position = memberKnowledge.position;

					return values;
				}
			}

			// Replace random record
			console.print(None, "Random\n", memberId);
			size_t index = gen() % values.size();
			values[index].id = memberId;
			values[index].value = memberKnowledge.value;
			return values;
		}

		/**
		 * Map data from Alarm to Thermometer
		 *
		 * @param member member knowledge
		 * @param coordId Coordinator id
		 * @param coordKnowledge Coordinator knowledge
		 * @return Coordinator id (ensemble output)
		 */
		PortableSensor::Knowledge::CoordId coordToMemberMap(const PortableSensor::Knowledge member,
				const CDEECO::Id coordId, const Alarm::Knowledge coordKnowledge) {
			return coordId;
		}

	private:
		/**
		 * Random number engine
		 *
		 * Used to update random record when out of storage.
		 */
		std::default_random_engine gen;
	};
}

#endif // TEMP_EXCHANGE_H
