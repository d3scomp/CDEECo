#ifndef CONSOLE_H_
#define CONSOLE_H_

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
	static void init();
	static void log(const char *text);
private:
#ifdef CONSOLE_LCD
	static GMD1602 lcd;
	static char stored[20];
#endif

#ifdef CONSOLE_SERIAL
	static UART serial;
#endif

};

#endif // CONSOLE_H_
