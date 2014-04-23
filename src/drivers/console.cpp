#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include <string.h>

#include "console.h"

void Console::init() {
#ifdef CONSOLE_LCD
	lcd.init();
#endif

#ifdef CONSOLE_SERIAL
	serial.init();
#endif
}
void Console::log(const char *text) {
	vTaskSuspendAll();

#ifdef CONSOLE_LCD
	lcd.clear();
	lcd.writeXY(stored, 0, 0);
	strcpy(stored, text);
	lcd.writeXY(text, 0, 1);
#endif

#ifdef CONSOLE_SERIAL
	for(const char* c = text; *c != NULL; c++) {
		while(!serial.canSend());
		serial.send(*c);
	}
	while(!serial.canSend());
	serial.send('\n');
#endif

	xTaskResumeAll();
}
