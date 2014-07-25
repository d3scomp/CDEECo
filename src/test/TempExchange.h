/**
 * \ingroup example
 * @file TempExchange.h
 *
 * Temperature exchange ensemble definition
 *
 * \date 21. 5. 2014
 * \author Vladimír Matěna <vlada@mattty.cz>
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
 *
 * \ingroup example
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
		Ensemble(CDEECO::Component<Alarm::Knowledge> &coordinator,
				CDEECO::KnowledgeLibrary<PortableSensor::Knowledge> &library);

		/**
		 * Temperature exchange constructor for node where member is hosted
		 *
		 * @param member Member component
		 * @param library Library of coordinator component knowledge
		 */
		Ensemble(CDEECO::Component<PortableSensor::Knowledge> &member,
				CDEECO::KnowledgeLibrary<Alarm::Knowledge> &library);

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
				const PortableSensor::Knowledge memberKnowledge);

		/**
		 *  Map temperatures from Thermometers to Alarm
		 *
		 *  @param coord Coordinator knowledge
		 *  @param memberId Member id
		 *  @param memberKnowledge Member knowledge
		 *  @return Sensor data array (ensemble output)
		 */
		Alarm::Knowledge::SensorData memberToCoordMap(const Alarm::Knowledge coord, const CDEECO::Id memberId,
				const PortableSensor::Knowledge memberKnowledge);

		/**
		 * Map data from Alarm to Thermometer
		 *
		 * @param member member knowledge
		 * @param coordId Coordinator id
		 * @param coordKnowledge Coordinator knowledge
		 * @return Coordinator id (ensemble output)
		 */
		PortableSensor::Knowledge::CoordId coordToMemberMap(const PortableSensor::Knowledge member,
				const CDEECO::Id coordId, const Alarm::Knowledge coordKnowledge);

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
