/*
 * Ensamble.h
 *
 *  Created on: 21. 5. 2014
 *      Author: Vladimír Matěna
 */

#ifndef ENSAMBLE_H_
#define ENSAMBLE_H_

#include "Component.h"
#include "KnowledgeCache.h"
#include "cdeeco/FreeRTOSTask.h"

namespace CDEECO {

	template<typename COORD_KNOWLEDGE, typename OUT_KNOWLEDGE, typename MEMBER_KNOWLEDGE>
	class Ensamble: FreeRTOSTask<> {
	public:
		Ensamble(Component<COORD_KNOWLEDGE> &coordinator, OUT_KNOWLEDGE &outKnowledge,
				KnowledgeLibrary<MEMBER_KNOWLEDGE> &cache, long period) :
				period(period), coordinator(coordinator), outKnowledge(outKnowledge), library(cache) {
		}
		;

		virtual ~Ensamble() {
		}

	protected:
		/**
		 * Membership to function to be implemented
		 *
		 * @param coord Coordinator knowledge
		 * @param member Member knowledge
		 * @return Whenever the member is part of Ensamble
		 */
		virtual bool member(const COORD_KNOWLEDGE coord, const MEMBER_KNOWLEDGE member) = 0;

		/**
		 * Knowledge map function to be implemented
		 *
		 * @param coord Coordinator knowledge
		 * @param member Member knowledge
		 * @return Output knowledge for coordinator
		 */
		virtual OUT_KNOWLEDGE map(const COORD_KNOWLEDGE coord, const MEMBER_KNOWLEDGE member) = 0;

	private:
		long period;
		Component<COORD_KNOWLEDGE> &coordinator;
		OUT_KNOWLEDGE &outKnowledge;
		KnowledgeLibrary<MEMBER_KNOWLEDGE> &library;

		/** Ensamble periodic task */
		void run() {
			// Schedule the task periodically
			while(1) {
				// TODO: For all knowledge from the cache check member and execute map
				Console::log(">>>> Ensamble task running now");

				for(const typename KnowledgeLibrary<MEMBER_KNOWLEDGE>::CacheRecord &record : library) {
					if(record.complete)
						Console::log("Found valid cache record");
					else
						Console::log("Found INvalid cache record");
				}

				// Wait for next execution time
				vTaskDelay(period / portTICK_PERIOD_MS);
			}
		}
	};

}

#endif /* ENSAMBLE_H_ */
