#ifndef REBROADCAST_STORAGE_H_
#define REBORADCAST_STORAGE_H_

#include "KnowledgeFragment.h"
#include "System.h"

class System;

template<size_t SIZE>
class RebroadcastStorage {
public:
	RebroadcastStorage(System &system): system(system) {
		memset(&records, 0, sizeof(records));
	}

	void storeFragment(const KnowledgeFragment fragment) {
		Console::log("TODO: Store fragment in rebroadcast storage");
	}

private:
	typedef uint32_t Timestamp;

	struct ReboadcastRecord {
		Timestamp recieved;
		Timestamp rebroadcast;
		KnowledgeFragment fragment;
	};

	System &system;

	std::array<RebroadcastStorage::ReboadcastRecord, SIZE> records;
};

#endif // REBROADCAST_STORAGE_H_
