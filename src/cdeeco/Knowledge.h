#ifndef KNOWLEDGE_H
#define KNOWLEDGE_H

#include <array>

namespace CDEECO {
	/// Component/knowledge type
	typedef uint32_t Type;
	/// Component/knowledge id
	typedef uint32_t Id;

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
		static constexpr std::array<size_t, 0> offsets = {};
	};
	template<class KNOWLEDGE>
	constexpr std::array<size_t, 0> KnowledgeTrait<KNOWLEDGE>::offsets;
}

#endif // KNOWLEDGE_H
