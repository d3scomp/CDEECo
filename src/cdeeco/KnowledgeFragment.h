/**
 * KnowledgeFragment.h
 *
 *  Created on: 28. 4. 2014
 *      Author: Vladimír Matěna
 */

#ifndef KNOWLEDGE_FRAGMENT_H_
#define KNOWLEDGE_FRAGMENT_H_

#include <cstdint>
#include <cstring>

struct KnowledgeFragment {
	typedef uint32_t Type;
	typedef uint32_t Id;

	static const size_t MAX_PACKET_SIZE = 128;
	static const size_t MAX_DATA_SIZE = 100;

	Type type;
	Id id;
	size_t size;
	size_t offset;
	char data[MAX_DATA_SIZE];

	size_t length() const {
		return sizeof(KnowledgeFragment) - MAX_DATA_SIZE + size;
	}
};

static_assert(KnowledgeFragment::MAX_PACKET_SIZE >= sizeof(KnowledgeFragment), "Knowledge fragment too big to fit into packet.");

#endif // KNOWLEDGE_FRAGMENT_H_
