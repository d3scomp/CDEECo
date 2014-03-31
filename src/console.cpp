#include "console.h"

#include <string.h>


void Console::init() {
	lcd.init();
}
void Console::log(const char *text) {
	lcd.clear();
	lcd.writeXY(stored, 0, 0);
	strcpy(stored, text);
	lcd.writeXY(text, 0, 1);
}
