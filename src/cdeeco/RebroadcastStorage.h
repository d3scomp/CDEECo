#ifndef REBROADCAST_STORAGE_H_
#define REBROADCAST_STORAGE_H_

#include "FreeRTOS.h"
#include "task.h"

#include <array>

#include "Console.h"
#include "KnowledgeFragment.h"
#include "Broadcaster.h"
#include "FreeRTOSTask.h"
#include "FreeRTOSMutex.h"

template<size_t SIZE>
class RebroadcastStorage: FreeRTOSTask {
private:
	typedef TickType_t Timestamp;
	typedef size_t Index;
	static const Timestamp PERIOD = 1000;

	struct ReboadcastRecord {
		bool used;
		Timestamp received;
		Timestamp rebroadcast;
		KnowledgeFragment fragment;
	};

public:
	RebroadcastStorage(Broadcaster &broadcaster) :
			broadcaster(broadcaster) {
		memset(&records, 0, sizeof(records));
	}

	void storeFragment(const KnowledgeFragment fragment) {
		// TODO: Implement some smarter rebroadcast mechanism
		const Timestamp RebroadcastInterval = 5000;

		// TODO: Do not rebroadcast locally hosted knowledge ?

		recordsMutex.lock();
		Index free = getFree();
		records[free].used = true;
		records[free].received = xTaskGetTickCount();
		records[free].rebroadcast = xTaskGetTickCount() + RebroadcastInterval / portTICK_PERIOD_MS;
		records[free].fragment = fragment;
		recordsMutex.unlock();
	}

	Index getFree() {
		for(Index i = 0; i < records.size(); ++i)
			if(!records[i].used)
				return i;
		return pushRecord();
	}

	Index pushRecord() {
		Index oldest = 0;
		for(Index i = 0; i < records.size(); ++i)
			if(records[i].received < records[oldest].received)
				oldest = i;
		rebroadcast(oldest);
		return oldest;
	}

	void rebroadcast(Index index) {
		Console::log(">>> Rebroadcasting fragment");
		broadcaster.broadcastFragment(records[index].fragment);
		records[index].used = false;
	}

	/**
	 * Check records and rebroadcast them every PERIOD
	 */
	void run() {
		while(1) {
			vTaskDelay(PERIOD / portTICK_PERIOD_MS);

			const Timestamp now = xTaskGetTickCount();
			recordsMutex.lock();
			for(Index i = 0; i < records.size(); ++i)
				if(records[i].used && records[i].rebroadcast <= now)
					rebroadcast(i);
			recordsMutex.unlock();
		}
	}

private:
	Broadcaster &broadcaster;

	FreeRTOSMutex recordsMutex;
	std::array<RebroadcastStorage::ReboadcastRecord, SIZE> records;
};

#endif // REBROADCAST_STORAGE_H_
