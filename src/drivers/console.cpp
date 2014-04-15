#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include <string.h>

#include "console.h"

void Console::init() {
	lcd.init();
}
void Console::log(const char *text) {
	vTaskSuspendAll();
	lcd.clear();
	lcd.writeXY(stored, 0, 0);
	strcpy(stored, text);
	lcd.writeXY(text, 0, 1);
	xTaskResumeAll();
}
