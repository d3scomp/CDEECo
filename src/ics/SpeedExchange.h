#ifndef SPEED_EXCHANGE_H
#define SPEED_EXCHANGE_H

#include "cdeeco/Component.h"
#include "cdeeco/Ensemble.h"
#include "cdeeco/KnowledgeCache.h"

#include "ICS.h"
#include "Vehicle.h"

/**
 * Speed and info exchange at intelligent cross road
 */
namespace SpeedExchange {
	/**
	 * Base Ensemble type definition
	 *
	 * Coordinator: ICS
	 * type: ICS knowledge
	 * output: Vehicle info array
	 *
	 * Member: Vehicle
	 * type: Vehicle::Knowledge
	 * output: Desired vehicle Speed
	 */
	typedef CDEECO::Ensemble<ICS::Knowledge, Vehicle::Knowledge*, Vehicle::Knowledge, Speed> EnsembleType;

	/**
	 * Speed exchange ensemble class
	 */
	class Ensemble: EnsembleType {
	public:
		/// Map try period
		/// TODO: Period
		static const auto PERIOD_MS = 2027;

		Ensemble(CDEECO::Component<ICS::Knowledge> &coordinator,
				CDEECO::KnowledgeLibrary<Vehicle::Knowledge> &library);

		Ensemble(CDEECO::Component<Vehicle::Knowledge> &member,
				CDEECO::KnowledgeLibrary<ICS::Knowledge> &library);

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
		bool isMember(const CDEECO::Id coordId, const ICS::Knowledge coordKnowledge, const CDEECO::Id memberId,
				const Vehicle::Knowledge memberKnowledge);

		/**
		 *  Map from Vehicle to ICS
		 *
		 *  @param coord Coordinator knowledge
		 *  @param memberId Member id
		 *  @param memberKnowledge Member knowledge
		 *  @return Ensemble output
		 */
		Vehicle::Knowledge* memberToCoordMap(const ICS::Knowledge coord, const CDEECO::Id memberId,
				const Vehicle::Knowledge memberKnowledge);

		/**
		 * Map data ICS to Vehicle
		 *
		 * @param member member knowledge
		 * @param coordId Coordinator id
		 * @param coordKnowledge Coordinator knowledge
		 * @return Ensemble output
		 */
		Speed coordToMemberMap(const Vehicle::Knowledge member,
				const CDEECO::Id coordId, const ICS::Knowledge coordKnowledge);
	};
}

#endif // SPEED_EXCHANGE_H
