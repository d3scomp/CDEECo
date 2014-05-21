#ifndef COMPONENT_KNOWLEDGE_H_
#define COMPONENT_KNOWLEDGE_H_

#include "Console.h"

namespace CDEECO {

/**
 * Knowledge interface
 */
struct Knowledge {
};

/**
 * Knowledge trait
 *
 * Used for allowed fragment offset definition
 */
template<class KNOWLEDGE>
struct KnowledgeTrait {
	static constexpr void* offsets[] = { };
	static void humanPrint() {
		Console::log("No human printing defined for this knowledge");
	}
};

}

#endif // COMPONENT_KNOWLEDGE_H
