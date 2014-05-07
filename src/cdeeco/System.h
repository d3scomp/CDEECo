/*
 * System.h
 *
 *  Created on: 7. 5. 2014
 *      Author: Vladimír Matěna
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "KnowledgeCache.h"

template<size_t CACHES>
class System {
public:
	System() {
		// Erase caches
		memset(&caches, 0, sizeof(caches));
	}

private:
	KnowledgeCache *caches[CACHES];
};

#endif /* SYSTEM_H_ */
