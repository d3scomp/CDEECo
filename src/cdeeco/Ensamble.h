/*
 * Ensamble.h
 *
 *  Created on: 21. 5. 2014
 *      Author: Vladimír Matěna
 */

#ifndef ENSAMBLE_H_
#define ENSAMBLE_H_

#include <type_traits>

#include "Component.h"
#include "KnowledgeCache.h"
#include "cdeeco/FreeRTOSTask.h"

namespace CDEECO {

	template<typename COORD_KNOWLEDGE, typename COORD_OUT_KNOWLEDGE, typename MEMBER_KNOWLEDGE,
	typename MEMBER_OUT_KNOWLEDGE>
	class Ensamble: FreeRTOSTask {
	public:
		Ensamble(Component<COORD_KNOWLEDGE> *coordinator, COORD_OUT_KNOWLEDGE *coordOutKnowledge,
				KnowledgeLibrary<MEMBER_KNOWLEDGE> *memberLibrary, long period) :
				period(period), coordinator(coordinator), member(NULL), coordOutKnowledge(coordOutKnowledge), memberOutKnowledge(
				NULL), memberLibrary(memberLibrary), coordLibrary(NULL) {
		}

		Ensamble(Component<MEMBER_KNOWLEDGE> *member, MEMBER_OUT_KNOWLEDGE *memberOutKnowledge,
				KnowledgeLibrary<COORD_KNOWLEDGE> *coordLibrary, long period) :
				period(period), coordinator(NULL), member(member), coordOutKnowledge(NULL), memberOutKnowledge(
						memberOutKnowledge), memberLibrary(NULL), coordLibrary(coordLibrary) {
		}

		virtual ~Ensamble() {
		}

	protected:
		/**
		 * Membership to function to be implemented
		 *
		 * @param coord Coordinator knowledge
		 * @param member Member knowledge
		 * @return Whenever the member is part of Ensemble
		 */
		virtual bool isMember(const COORD_KNOWLEDGE coord, const MEMBER_KNOWLEDGE member) = 0;

		/**
		 * Member to Coordinator knowledge map function to be implemented
		 *
		 * @param coord Coordinator knowledge
		 * @param member Member knowledge
		 * @return Output knowledge for coordinator
		 */
		virtual COORD_OUT_KNOWLEDGE memberToCoordMap(const COORD_KNOWLEDGE coord, const Id memberId,
				const MEMBER_KNOWLEDGE memberKnowledge) = 0;

		/**
		 * Coordinator to member knowledge map function to be implemented
		 *
		 * @param member Member knowledge
		 * @param coord Coordinator knowledge
		 * @return Output knowledge for coordinator
		 */
		virtual MEMBER_OUT_KNOWLEDGE coordToMemberMap(const MEMBER_KNOWLEDGE member,
				const Id coordId, const COORD_KNOWLEDGE coordKnowledge) = 0;

	private:
		long period;
		Component<COORD_KNOWLEDGE> *coordinator;
		Component<MEMBER_KNOWLEDGE> *member;
		COORD_OUT_KNOWLEDGE *coordOutKnowledge;
		MEMBER_OUT_KNOWLEDGE *memberOutKnowledge;
		KnowledgeLibrary<MEMBER_KNOWLEDGE> *memberLibrary;
		KnowledgeLibrary<COORD_KNOWLEDGE> *coordLibrary;

		/** Ensamble periodic task */
		void run() {
			// Schedule the task periodically
			while(1) {
				// For all knowledge from the cache check member and execute map
				Console::log(">>>> Ensamble task running now\n");

				runExchange();

				// Wait for next execution time
				vTaskDelay(period / portTICK_PERIOD_MS);
			}
		}

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
			Console::log("Error mixed ensamble setup");
			assert_param(false);
		}

		// Map member -> coord
		template <typename T>
		typename std::enable_if<!std::is_void<T>::value, void>::type runMemberToCoordExchange() {
			Console::print(Debug, ">>>> Trying member->coord exchange\n");
			COORD_KNOWLEDGE coordKnowledge = coordinator->lockReadKnowledge();
			for(const typename KnowledgeLibrary<MEMBER_KNOWLEDGE>::CacheRecord &record : *memberLibrary) {
				if(record.complete) {
					Console::log(">>>> Found complete record, trying membership <<<<\n");
					if(isMember(coordKnowledge, record.knowledge)) {
						Console::log(">>>> Record's knowledge is member of this Ensable, running member->coord exchange\n");
						COORD_OUT_KNOWLEDGE out = memberToCoordMap(coordKnowledge, record.id, record.knowledge);
						coordinator->lockWriteKnowledge(*coordOutKnowledge, out);
					} else {
						Console::log(">>>> Record's knowledge is not member <<<<\n");
					}
				}
			}
		}
		template <typename T>
		typename std::enable_if<std::is_void<T>::value, void>::type runMemberToCoordExchange() {
			// COORD_OUT_KNOWLEDGE is void
		}

		// Map coord -> member
		template <typename T>
		typename std::enable_if<!std::is_void<T>::value, void>::type runCoordToMemberExchange() {
			Console::print(Debug, ">>>> Trying coord->member exchange\n");
			MEMBER_KNOWLEDGE memberKnowledge = member->lockReadKnowledge();
			for(const typename KnowledgeLibrary<COORD_KNOWLEDGE>::CacheRecord &record : *coordLibrary) {
				if(record.complete) {
					Console::log(">>>> Found complete record, trying membership <<<<\n");
					if(isMember(record.knowledge, memberKnowledge)) {
						Console::log(">>>> Record's knowledge is member of this Ensable, running coord->member exchange");

						MEMBER_OUT_KNOWLEDGE out = coordToMemberMap(memberKnowledge, record.id, record.knowledge);
						member->lockWriteKnowledge(*memberOutKnowledge, out);

					} else {
						Console::log(">>>> Record's knowledge is not member <<<<\n");
					}
				}
			}
		}
		template <typename T>
		typename std::enable_if<std::is_void<T>::value, void>::type runCoordToMemberExchange() {
			// MEMBER_OUT_KNOWLEDGE is void
		}
	};
}

#endif /* ENSAMBLE_H_ */
