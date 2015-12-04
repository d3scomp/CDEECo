/**
 * \ingroup cdeeco
 * @file Ensemble.h
 *
 * CDEECo++ base Ensemble implementation
 *
 * \date 21. 5. 2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 */

#ifndef ENSEMBLE_H
#define ENSEMBLE_H

#include <type_traits>

#include "Component.h"
#include "KnowledgeCache.h"
#include "wrappers/FreeRTOSTask.h"

namespace CDEECO {
	/**
	 * Ensemble base class template
	 *
	 * @tparam COORD_KNOWLEDGE Type of coordinator knowledge
	 * @tparam COORD_OUT_KNOWLEDGE Type of coordinator output knowledge
	 * @tparam MEMEBR_KNOWLEDGE Type of member knowledge
	 * @tparam MEMBER_OUT_KNOWLEDGE Type of member output knowledge
	 *
	 * \ingroup cdeeco
	 */
	template<typename COORD_KNOWLEDGE, typename COORD_OUT_KNOWLEDGE, typename MEMBER_KNOWLEDGE,
			typename MEMBER_OUT_KNOWLEDGE>
	class Ensemble: FreeRTOSTask {
	public:
		/**
		 * Ensemble constructor to be used on the node hosting coordinator
		 *
		 * @param coordinator Pointer to coordinator component
		 * @param coordOutKnowledge Pointer to coordinator output knowledge
		 * @param memberLibrary Pointer to library of member knowledge
		 * @param period Interval between knowledge mapping tries
		 */
		Ensemble(Component<COORD_KNOWLEDGE> *coordinator, COORD_OUT_KNOWLEDGE *coordOutKnowledge,
				KnowledgeLibrary<MEMBER_KNOWLEDGE> *memberLibrary, long period) :
				period(period), coordinator(coordinator), member(NULL), coordOutKnowledge(coordOutKnowledge), memberOutKnowledge(
				NULL), memberLibrary(memberLibrary), coordLibrary(NULL) {
		}

		/**
		 * Ensemble constructor to be used on the node hosting member
		 *
		 * @param member Pointer to member component
		 * @param memberOutKnowledge Pointer to member output knowledge
		 * @param coordLibrary Library of coordinator knowledge
		 * @param period Interval between knowledge mapping tries
		 */
		Ensemble(Component<MEMBER_KNOWLEDGE> *member, MEMBER_OUT_KNOWLEDGE *memberOutKnowledge,
				KnowledgeLibrary<COORD_KNOWLEDGE> *coordLibrary, long period) :
				period(period), coordinator(NULL), member(member), coordOutKnowledge(NULL), memberOutKnowledge(
						memberOutKnowledge), memberLibrary(NULL), coordLibrary(coordLibrary) {
		}

	protected:
		/**
		 * Membership to function to be implemented
		 *
		 * @param coordKnowledge Coordinator knowledge
		 * @param coordId Id of the coordinator
		 * @param memberKnowledge Member knowledge
		 * @param memberId Id of the member
		 * @return Whenever the member is part of Ensemble
		 */
		virtual bool isMember(const Id coordId, const COORD_KNOWLEDGE coordKnowledge, const Id memberId,
				const MEMBER_KNOWLEDGE memberKnowledge) = 0;

		/**
		 * Member to Coordinator knowledge map function to be implemented
		 *
		 * @param coordKnowledge Coordinator knowledge
		 * @param memberId Id of the member
		 * @param memberKnowledge Member knowledge
		 * @return Output knowledge for coordinator
		 */
		virtual COORD_OUT_KNOWLEDGE memberToCoordMap(const COORD_KNOWLEDGE coordKnowledge, const Id memberId,
				const MEMBER_KNOWLEDGE memberKnowledge) = 0;

		/**
		 * Coordinator to member knowledge map function to be implemented
		 *
		 * @param memberKnowledge Member knowledge
		 * @param coordId Id of the coordinator
		 * @param coordKnowledge Coordinator knowledge
		 * @return Output knowledge for coordinator
		 */
		virtual MEMBER_OUT_KNOWLEDGE coordToMemberMap(const MEMBER_KNOWLEDGE memberKnowledge, const Id coordId,
				const COORD_KNOWLEDGE coordKnowledge) = 0;

	private:
		/// INterval between mapping tries
		long period;
		/// Pointer to coordinator component
		Component<COORD_KNOWLEDGE> *coordinator;
		/// Pointer to member component
		Component<MEMBER_KNOWLEDGE> *member;
		/// Pointer to coordinator output knowledge
		COORD_OUT_KNOWLEDGE *coordOutKnowledge;
		/// Pointer to member output knowledge
		MEMBER_OUT_KNOWLEDGE *memberOutKnowledge;
		/// Pointer to member library
		KnowledgeLibrary<MEMBER_KNOWLEDGE> *memberLibrary;
		/// Pointer to coordinator library
		KnowledgeLibrary<COORD_KNOWLEDGE> *coordLibrary;

		/** Ensemble periodic task */
		void run() {
			// Schedule the task periodically
			while(1) {
				// For all knowledge from the cache check member and execute map
				console.print(Debug, ">>>> Ensemble task running now\n");

				runExchange();

				// Wait for next execution time
				FreeRTOSTask::mDelay(period);
			}
		}

		/**
		 * Try to run knowledge exchange
		 *
		 * Executed periodically.
		 */
		void runExchange() {
			if(coordinator != NULL && memberLibrary != NULL && member == NULL && coordLibrary == NULL) {
				runMemberToCoordExchange<COORD_OUT_KNOWLEDGE>();
				return;
			}

			if(coordinator == NULL && memberLibrary == NULL && member != NULL && coordLibrary != NULL) {
				runCoordToMemberExchange<MEMBER_OUT_KNOWLEDGE>();
				return;
			}

			// Error mixed setup
			console.print(Error, "Error mixed ensemble setup");
			assert_param(false);
		}

		/**
		 * Map from each member to coordinator
		 *
		 * Real version used when output is not void.
		 */
		template<typename T>
		typename std::enable_if<!std::is_void<T>::value, void>::type runMemberToCoordExchange() {
			console.print(Debug, ">>>> Trying member->coord exchange\n");
			for(const auto &record : *memberLibrary) {
				if(record.complete) {
					console.print(Debug, ">>>> Found complete record, trying membership <<<<\n");
					COORD_KNOWLEDGE coordKnowledge = coordinator->lockReadKnowledge();
					if(isMember(coordinator->getId(), coordKnowledge, record.id, record.knowledge)) {
						console.print(Debug,
								">>>> Record is member of this Ensemble, running member->coord exchange\n");

						COORD_OUT_KNOWLEDGE out = memberToCoordMap(coordKnowledge, record.id, record.knowledge);

						coordinator->lockWriteKnowledge(*coordOutKnowledge, out);
					} else {
						console.print(Debug, ">>>> Record's knowledge is not member <<<<\n");
					}
				}
			}
		}
		/**
		 * Map from member to coordinator
		 *
		 * Dummy version. Used when output is void.
		 */
		template<typename T>
		typename std::enable_if<std::is_void<T>::value, void>::type runMemberToCoordExchange() {
			// COORD_OUT_KNOWLEDGE is void
		}

		/**
		 * Map from coordinator to member
		 *
		 * Real version used when output is not void.
		 */
		template<typename T>
		typename std::enable_if<!std::is_void<T>::value, void>::type runCoordToMemberExchange() {
			console.print(Debug, ">>>> Trying coord->member exchange\n");
			for(const auto &record : *coordLibrary) {
				if(record.complete) {
					console.print(Debug, ">>>> Found complete record, trying membership <<<<\n");
					MEMBER_KNOWLEDGE memberKnowledge = member->lockReadKnowledge();
					if(isMember(record.id, record.knowledge, member->getId(), memberKnowledge)) {
						console.print(Debug, ">>>> Record is member of this Ensable, running coord->member exchange");

						MEMBER_OUT_KNOWLEDGE out = coordToMemberMap(memberKnowledge, record.id, record.knowledge);

						member->lockWriteKnowledge(*memberOutKnowledge, out);

					} else {
						console.print(Debug, ">>>> Record's knowledge is not member <<<<\n");
					}
				}
			}
		}
		/**
		 *  Map from coordinator to member
		 *
		 * Dummy version. Used when output is void.
		 */
		template<typename T>
		typename std::enable_if<std::is_void<T>::value, void>::type runCoordToMemberExchange() {
			// MEMBER_OUT_KNOWLEDGE is void
		}
	};
}

#endif // ENSEMBLE_H
