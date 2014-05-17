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

typedef uint32_t ComponentType;
typedef uint32_t ComponentId;

const size_t MaxPacketSize = 128;
const size_t MaxDataSize = 100;

struct KnowledgeFragment {
	ComponentType type;
	ComponentId id;
	size_t size;
	size_t offset;
	char data[MaxDataSize];
};

static_assert(MaxPacketSize >= sizeof(KnowledgeFragment), "Knowledge fragment too big to fit into packet.");

#endif // KNOWLEDGE_FRAGMENT_H_
