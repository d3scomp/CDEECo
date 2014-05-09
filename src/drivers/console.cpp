#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include <string.h>

#include "console.h"

#ifdef CONSOLE_LCD
GMD1602 Console::lcd(GPIOE, RCC_AHB1Periph_GPIOE);
char Console::stored[20];
#endif
#ifdef CONSOLE_SERIAL
UART::Properties serialProps {
GPIOA, USART2,
GPIO_Pin_2, GPIO_Pin_3, GPIO_PinSource2, GPIO_PinSource3, RCC_APB1PeriphClockCmd, RCC_AHB1Periph_GPIOA,
		RCC_APB1Periph_USART2, GPIO_AF_USART2, USART2_IRQn, 921600 };
UART Console::serial(serialProps);
#endif

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
	for(const char* c = text; *c != 0; c++) {
		while(!serial.canSend());
		serial.send(*c);
	}
	while(!serial.canSend());
	serial.send('\n');
#endif

	xTaskResumeAll();
}
