/**
 * KnowledgeFragment.h
 *
 *  Created on: 28. 4. 2014
 *      Author: Vladimír Matěna
 */

#ifndef FRAGMENT_H_
#define FRAGMENT_H_

struct KnowledgeFragment {
	ComponentType type;
	ComponentId id;
	size_t size;
	size_t offset;
	char* data;
};

#endif // FRAGMENT_H_
