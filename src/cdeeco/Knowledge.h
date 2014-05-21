#ifndef COMPONENT_KNOWLEDGE_H_
#define COMPONENT_KNOWLEDGE_H_

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
};

}

#endif // COMPONENT_KNOWLEDGE_H
