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

struct KnowledgeFragment {
	static const size_t MaxDataSize = 100;

	ComponentType type;
	ComponentId id;
	size_t size;
	size_t offset;
	char data[MaxDataSize];
};

#endif // FRAGMENT_H_
