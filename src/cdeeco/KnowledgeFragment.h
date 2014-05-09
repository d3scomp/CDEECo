/**
 * KnowledgeFragment.h
 *
 *  Created on: 28. 4. 2014
 *      Author: Vladimír Matěna
 */

#ifndef FRAGMENT_H_
#define FRAGMENT_H_

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

#endif // FRAGMENT_H_
