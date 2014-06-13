#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <cstdio>
#include <stdio.h>
#include <stdarg.h>

#include "cdeeco/KnowledgeFragment.h"
#include "cdeeco/Receiver.h"

#include "UART.h"

enum Level {
	None = 5,
	Error = 4,
	TaskInfo = 3,
	Info = 2,
	Debug = 1,
	All = 0
};

class Console {
public:
	static const size_t MAX_LENGTH = 500;

	Console(UART &serial);

	void init();
	void logFragment(const CDEECO::KnowledgeFragment fragment);
	void setFragmentReceiver(CDEECO::Receiver *receiver);
	void log(const char * format, ...);
	void print(const Level level, const char * format, ...);
	void printFloat(const Level level, const float value, const int decimals = 2);
	void setLevel(const Level level);
	void toggleLevel();

private:
	UART &serial;
	char buffer[MAX_LENGTH];
	CDEECO::Receiver *receiver = NULL;
	static void staticReceiveListener(void* data);
	void receiveListener();
	Level level = All;

	template<typename T>
	T recv() {
		T data;
		for(size_t i = 0; i < sizeof(T); ++i)
			((uint8_t*) &data)[i] = recv<uint8_t>();
		return data;
	}

	void putString(const char *text);

	uint8_t recvHexVal();
};

#endif // CONSOLE_H_
