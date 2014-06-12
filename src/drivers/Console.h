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
	static void init();
	static void logFragment(const CDEECO::KnowledgeFragment fragment);
	static void setFragmentReceiver(CDEECO::Receiver *receiver);
	static void interrupt();
	static void log(const char * format, ...);
	static void print(const Level level, const char * format, ...);
	static void setLevel(const Level level);
	static void toggleLevel();

private:
	static UART serial;
	static char buffer[MAX_LENGTH];
	static CDEECO::Receiver *receiver;
	static void receiveListener(void* data);
	static Level level;

	template<typename T>
	static T recv() {
		T data;
		for(size_t i = 0; i < sizeof(T); ++i)
			((uint8_t*) &data)[i] = recv<uint8_t>();
		return data;
	}

	static void putString(const char *text);

	static uint8_t recvHexVal();
};

struct Hex {
};

extern Hex hex;


class ConsoleStream {
public:
	ConsoleStream& operator<<(const char* text) {
		Console::log(text);
		return *this;
	}

	ConsoleStream& operator<<(const uint32_t num) {
		Console::log(forms[nextForm], num);
		nextForm = 0;
		return *this;
	}

	ConsoleStream& operator<<(const Hex) {
		nextForm = 1;
		return *this;
	}

private:
	static constexpr const char* forms[] = {"%d", "%x"};
	int nextForm = 0;
};

#endif // CONSOLE_H_
