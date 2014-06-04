#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <cstdio>

#include "cdeeco/KnowledgeFragment.h"
#include "cdeeco/Receiver.h"

#include "UART.h"

class Console {
public:
	static void init();
	static void log(const char *text);
	static void logFragment(const KnowledgeFragment fragment);
	static void setFragmentReceiver(CDEECO::Receiver *receiver);
	static void interrupt();

	template<typename T0>
	static void log(const char *text, const T0 v0) {
		char buff[40];
		sprintf(buff, text, v0);
		log(buff);
	}

	template<typename T0, typename T1>
	static void log(const char *text, const T0 v0, const T1 v1) {
		char buff[80];
		sprintf(buff, text, v0, v1);
		log(buff);
	}

private:
	static UART serial;
	static CDEECO::Receiver *receiver;
	static void receiveListener(void* data);

	template<typename T>
	static T recv() {
		T data;
		for(size_t i = 0; i < sizeof(T); ++i)
			((uint8_t*)&data)[i] = recv<uint8_t>();
		return data;
	}

	static uint8_t recvHexVal();
};

#endif // CONSOLE_H_
