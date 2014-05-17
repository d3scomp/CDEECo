#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "cdeeco/KnowledgeFragment.h"

//#define CONSOLE_LCD
#define CONSOLE_SERIAL

#ifdef CONSOLE_LCD
#include "gmd1602.h"
#endif

#ifdef CONSOLE_SERIAL
#include "UART.h"
#endif

class Console {
public:
	typedef void (*Listener)(void* data, const KnowledgeFragment fragment);
	static void init();
	static void log(const char *text);
	static void logFragment(const KnowledgeFragment fragment);
	static void setRecvFragmentListener(Listener, void* data);
	static void interrupt();
private:
#ifdef CONSOLE_LCD
	static GMD1602 lcd;
	static char stored[20];
#endif
#ifdef CONSOLE_SERIAL
	static UART serial;
#endif
	static void *listenerData;
	static Listener listenerFunc;
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
