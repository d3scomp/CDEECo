#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "gmd1602.h"

class Console {
public:
	static void init();
	static void log(const char *text);
private:
	static GMD1602 lcd;
	static char stored[16];
};

#endif // CONSOLE_H_
