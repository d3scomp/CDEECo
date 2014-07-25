/**
 * \ingroup cdeeco
 * @file Knowledge.h
 *
 * CDEECo++ base knowledge definition
 *
 * \date 20.4.2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 */

#ifndef KNOWLEDGE_H
#define KNOWLEDGE_H

#include <array>

namespace CDEECO {
	/**
	 * Component/knowledge type
	 * \ingroup cdeeco
	 */
	typedef uint32_t Type;
	/**
	 * Component/knowledge id
	 * \ingroup cdeeco
	 */
	typedef uint32_t Id;

	/**
	 * Knowledge interface
	 * \ingroup cdeeco
	 */
	struct Knowledge {
	};

	/**
	 * Knowledge trait
	 *
	 * Used for allowed fragment offset definition
	 *
	 * \ingroup cdeeco
	 */
	template<class KNOWLEDGE>
	struct KnowledgeTrait {
		static constexpr std::array<size_t, 0> offsets = {};
	};
}

#endif // KNOWLEDGE_H
